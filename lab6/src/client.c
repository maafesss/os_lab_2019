#include "common.h"
#include <pthread.h>
#include <getopt.h>
#include <sys/time.h>

// Структура для передачи данных в поток
typedef struct {
    int server_id;
    char *server_ip;
    int server_port;
    int start;
    int end;
    int mod;
    long long result;
    int status;
} ServerTask;

// Массив серверов
ServerTask *servers;
int server_count = 0;

// Функция для чтения серверов из файла
int read_servers(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen failed");
        return -1;
    }
    
    char line[256];
    int count = 0;
    
    // Сначала считаем количество серверов
    while (fgets(line, sizeof(line), file)) {
        count++;
    }
    
    rewind(file);
    
    servers = (ServerTask*)malloc(count * sizeof(ServerTask));
    if (!servers) {
        perror("malloc failed");
        fclose(file);
        return -1;
    }
    
    // Читаем серверы
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        // Убираем символ новой строки
        line[strcspn(line, "\n")] = 0;
        
        // Парсим ip:port
        char *colon = strchr(line, ':');
        if (!colon) {
            printf("Invalid server format: %s (expected ip:port)\n", line);
            continue;
        }
        
        *colon = '\0';
        servers[i].server_ip = strdup(line);
        servers[i].server_port = atoi(colon + 1);
        servers[i].server_id = i;
        servers[i].status = 0;
        i++;
    }
    
    fclose(file);
    return i;
}

// Функция для подключения к серверу и отправки задания
void* compute_on_server(void *arg) {
    ServerTask *task = (ServerTask*)arg;
    
    // Создаем сокет
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket failed");
        task->status = -1;
        return NULL;
    }
    
    // Настраиваем адрес сервера
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(task->server_port);
    
    if (inet_pton(AF_INET, task->server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        close(sock);
        task->status = -1;
        return NULL;
    }
    
    // Подключаемся к серверу
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        close(sock);
        task->status = -1;
        return NULL;
    }
    
    printf("Connected to server %s:%d\n", task->server_ip, task->server_port);
    
    // Создаем задание
    Task t;
    t.start = task->start;
    t.end = task->end;
    t.mod = task->mod;
    
    printf("Sending task: [%d, %d] mod %d\n", t.start, t.end, t.mod);
    
    // Отправляем задание
    if (send(sock, &t, sizeof(Task), 0) < 0) {
        perror("send failed");
        close(sock);
        task->status = -1;
        return NULL;
    }
    
    // Получаем результат
    Response resp;
    if (recv(sock, &resp, sizeof(Response), 0) < 0) {
        perror("recv failed");
        close(sock);
        task->status = -1;
        return NULL;
    }
    
    task->result = resp.result;
    task->status = resp.status;
    
    printf("Received result: %lld\n", task->result);
    
    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    int k = -1;
    int mod = -1;
    char *servers_file = NULL;
    
    // Разбор аргументов командной строки
    while (1) {
        static struct option options[] = {
            {"k", required_argument, 0, 'k'},
            {"mod", required_argument, 0, 'm'},
            {"servers", required_argument, 0, 's'},
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        int c = getopt_long(argc, argv, "k:m:s:", options, &option_index);
        
        if (c == -1) break;
        
        switch (c) {
            case 'k':
                k = atoi(optarg);
                break;
            case 'm':
                mod = atoi(optarg);
                break;
            case 's':
                servers_file = optarg;
                break;
        }
    }
    
    // Проверка аргументов
    if (k == -1 || mod == -1 || servers_file == NULL) {
        printf("Usage: %s --k <num> --mod <num> --servers <file>\n", argv[0]);
        printf("Example: %s --k 10 --mod 100 --servers servers.txt\n", argv[0]);
        return 1;
    }
    
    // Читаем серверы из файла
    server_count = read_servers(servers_file);
    if (server_count <= 0) {
        printf("No servers found in file: %s\n", servers_file);
        return 1;
    }
    
    printf("\n========================================\n");
    printf("Parallel Factorial over Network\n");
    printf("========================================\n");
    printf("k = %d\n", k);
    printf("mod = %d\n", mod);
    printf("Number of servers: %d\n", server_count);
    printf("========================================\n\n");
    
    // Особый случай: 0! = 1
    if (k == 0) {
        printf("Result: %d! mod %d = 1\n", k, mod);
        return 0;
    }
    
    // Разбиваем диапазон [1..k] на части для серверов
    int numbers_per_server = k / server_count;
    int remainder = k % server_count;
    int current_start = 1;
    
    for (int i = 0; i < server_count; i++) {
        servers[i].start = current_start;
        int extra = (i < remainder) ? 1 : 0;
        servers[i].end = current_start + numbers_per_server + extra - 1;
        servers[i].mod = mod;
        servers[i].result = 1;
        servers[i].status = 0;
        
        current_start = servers[i].end + 1;
        
        printf("Server %d (%s:%d): range [%d, %d]\n", 
               i, servers[i].server_ip, servers[i].server_port, 
               servers[i].start, servers[i].end);
    }
    
    printf("\nStarting computations...\n\n");
    
    struct timeval start_time, finish_time;
    gettimeofday(&start_time, NULL);
    
    // Создаем потоки для каждого сервера
    pthread_t *threads = (pthread_t*)malloc(server_count * sizeof(pthread_t));
    
    for (int i = 0; i < server_count; i++) {
        pthread_create(&threads[i], NULL, compute_on_server, &servers[i]);
    }
    
    // Ждем завершения всех потоков
    for (int i = 0; i < server_count; i++) {
        pthread_join(threads[i], NULL);
    }
    
    gettimeofday(&finish_time, NULL);
    
    // Собираем результаты
    long long total_result = 1;
    int failed_servers = 0;
    
    for (int i = 0; i < server_count; i++) {
        if (servers[i].status == 0) {
            total_result = (total_result * servers[i].result) % mod;
        } else {
            failed_servers++;
            printf("Server %d failed!\n", i);
        }
    }
    
    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
    
    printf("\n========================================\n");
    printf("RESULTS:\n");
    printf("========================================\n");
    printf("%d! mod %d = %lld\n", k, mod, total_result);
    printf("Elapsed time: %.3f ms\n", elapsed_time);
    
    if (failed_servers > 0) {
        printf("WARNING: %d servers failed!\n", failed_servers);
    }
    
    // Проверка: последовательное вычисление
    long long verification = 1;
    for (int i = 1; i <= k; i++) {
        verification = (verification * i) % mod;
    }
    printf("\nVerification: %d! mod %d = %lld\n", k, mod, verification);
    
    if (total_result == verification) {
        printf("✓ SUCCESS: Results match!\n");
    } else {
        printf("✗ ERROR: Results do not match!\n");
    }
    printf("========================================\n");
    
    // Очистка
    for (int i = 0; i < server_count; i++) {
        free(servers[i].server_ip);
    }
    free(servers);
    free(threads);
    
    return 0;
}
