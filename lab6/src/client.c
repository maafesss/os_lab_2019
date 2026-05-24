#include "libnetwork.h"
#include <pthread.h>
#include <getopt.h>
#include <sys/time.h>

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

ServerTask *servers;
int server_count = 0;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

int read_servers(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen failed");
        return -1;
    }
    
    char line[256];
    int count = 0;
    
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
    
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == '\0') continue;
        
        char *colon = strchr(line, ':');
        if (!colon) {
            printf("[CLIENT] Invalid server format: %s\n", line);
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

void* compute_on_server(void *arg) {
    ServerTask *task = (ServerTask*)arg;
    
    pthread_mutex_lock(&print_mutex);
    printf("[CLIENT] Connecting to %s:%d for range [%d, %d]...\n",
           task->server_ip, task->server_port, task->start, task->end);
    pthread_mutex_unlock(&print_mutex);
    
    int sock = create_socket();
    if (sock < 0) {
        task->status = -1;
        return NULL;
    }
    
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
    
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        close(sock);
        task->status = -1;
        return NULL;
    }
    
    pthread_mutex_lock(&print_mutex);
    printf("[CLIENT] Connected to %s:%d\n", task->server_ip, task->server_port);
    pthread_mutex_unlock(&print_mutex);
    
    Task t;
    t.start = task->start;
    t.end = task->end;
    t.mod = task->mod;
    
    if (send_task(sock, &t) < 0) {
        close(sock);
        task->status = -1;
        return NULL;
    }
    
    Response resp;
    if (receive_response(sock, &resp) < 0) {
        close(sock);
        task->status = -1;
        return NULL;
    }
    
    task->result = resp.result;
    task->status = resp.status;
    
    pthread_mutex_lock(&print_mutex);
    printf("[CLIENT] Result from %s:%d: %lld\n", 
           task->server_ip, task->server_port, task->result);
    pthread_mutex_unlock(&print_mutex);
    
    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    int k = -1, mod = -1;
    char *servers_file = NULL;
    
    static struct option options[] = {
        {"k", required_argument, 0, 'k'},
        {"mod", required_argument, 0, 'm'},
        {"servers", required_argument, 0, 's'},
        {0, 0, 0, 0}
    };
    
    int opt;
    while ((opt = getopt_long(argc, argv, "k:m:s:", options, NULL)) != -1) {
        switch (opt) {
            case 'k': k = atoi(optarg); break;
            case 'm': mod = atoi(optarg); break;
            case 's': servers_file = optarg; break;
        }
    }
    
    if (k == -1 || mod == -1 || servers_file == NULL) {
        printf("Usage: %s --k <num> --mod <num> --servers <file>\n", argv[0]);
        return 1;
    }
    
    server_count = read_servers(servers_file);
    if (server_count <= 0) {
        printf("[CLIENT] No servers found\n");
        return 1;
    }
    
    printf("\n========================================\n");
    printf("DISTRIBUTED FACTORIAL CALCULATOR\n");
    printf("k = %d, mod = %d, servers = %d\n", k, mod, server_count);
    printf("========================================\n\n");
    
    if (k == 0) {
        printf("Result: 0! mod %d = 1\n", mod);
        return 0;
    }
    
    if (server_count > k) server_count = k;
    
    int numbers_per_server = k / server_count;
    int remainder = k % server_count;
    int current_start = 1;
    
    for (int i = 0; i < server_count; i++) {
        servers[i].start = current_start;
        int extra = (i < remainder) ? 1 : 0;
        servers[i].end = current_start + numbers_per_server + extra - 1;
        servers[i].mod = mod;
        current_start = servers[i].end + 1;
        printf("[CLIENT] Server %d: range [%d, %d]\n", i, servers[i].start, servers[i].end);
    }
    
    struct timeval start_time, finish_time;
    gettimeofday(&start_time, NULL);
    
    pthread_t *threads = malloc(server_count * sizeof(pthread_t));
    for (int i = 0; i < server_count; i++) {
        pthread_create(&threads[i], NULL, compute_on_server, &servers[i]);
    }
    for (int i = 0; i < server_count; i++) {
        pthread_join(threads[i], NULL);
    }
    
    gettimeofday(&finish_time, NULL);
    
    long long total_result = 1;
    for (int i = 0; i < server_count; i++) {
        if (servers[i].status == 0) {
            total_result = (total_result * servers[i].result) % mod;
        }
    }
    
    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
    
    printf("\n========================================\n");
    printf("RESULT: %d! mod %d = %lld\n", k, mod, total_result);
    printf("Time: %.3f ms\n", elapsed_time);
    printf("========================================\n");
    
    free(threads);
    for (int i = 0; i < server_count; i++) free(servers[i].server_ip);
    free(servers);
    pthread_mutex_destroy(&print_mutex);
    
    return 0;
}
