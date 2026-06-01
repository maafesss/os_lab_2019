#include "libnetwork.h"
#include <pthread.h>
#include <getopt.h>
#include <sys/time.h>

typedef struct {
    int id;
    char *ip;
    int port;
    int start;
    int end;
    int mod;
    long long result;
    int success;
} ServerTask;

ServerTask *servers;
int server_count = 0;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

int read_servers(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return -1;
    
    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), f)) count++;
    rewind(f);
    
    servers = malloc(count * sizeof(ServerTask));
    if (!servers) {
        fclose(f);
        return -1;
    }
    
    int i = 0;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == '\0') continue;
        
        char *colon = strchr(line, ':');
        if (!colon) continue;
        
        *colon = '\0';
        servers[i].ip = strdup(line);
        servers[i].port = atoi(colon + 1);
        servers[i].id = i;
        servers[i].success = 0;
        i++;
    }
    
    fclose(f);
    return i;
}

void* compute_on_server(void *arg) {
    ServerTask *task = (ServerTask*)arg;
    
    pthread_mutex_lock(&print_mutex);
    printf("[CLIENT] Connecting to %s:%d for [%d, %d]\n",
           task->ip, task->port, task->start, task->end);
    pthread_mutex_unlock(&print_mutex);
    
    int sock = connect_to_server(task->ip, task->port);
    if (sock < 0) {
        task->success = 0;
        return NULL;
    }
    
    Task t = {task->start, task->end, task->mod};
    if (send_task(sock, &t) < 0) {
        close(sock);
        task->success = 0;
        return NULL;
    }
    
    Response resp;
    if (receive_response(sock, &resp) < 0) {
        close(sock);
        task->success = 0;
        return NULL;
    }
    
    task->result = resp.result;
    task->success = (resp.status == 0);
    
    pthread_mutex_lock(&print_mutex);
    printf("[CLIENT] Result from %s:%d: %lld\n", task->ip, task->port, task->result);
    pthread_mutex_unlock(&print_mutex);
    
    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    int k = -1, mod = -1;
    char *servers_file = NULL;
    
    static struct option opts[] = {
        {"k", required_argument, 0, 'k'},
        {"mod", required_argument, 0, 'm'},
        {"servers", required_argument, 0, 's'},
        {0, 0, 0, 0}
    };
    
    int opt;
    while ((opt = getopt_long(argc, argv, "k:m:s:", opts, NULL)) != -1) {
        switch (opt) {
            case 'k': k = atoi(optarg); break;
            case 'm': mod = atoi(optarg); break;
            case 's': servers_file = optarg; break;
        }
    }
    
    if (k == -1 || mod == -1 || !servers_file) {
        printf("Usage: %s --k <num> --mod <num> --servers <file>\n", argv[0]);
        printf("Example: %s --k 20 --mod 1000 --servers servers.txt\n", argv[0]);
        return 1;
    }
    
    server_count = read_servers(servers_file);
    if (server_count <= 0) {
        printf("[CLIENT] No servers found in %s\n", servers_file);
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
    
    // Корректируем количество серверов
    if (server_count > k) server_count = k;
    
    // Разбиваем диапазон
    int per_server = k / server_count;
    int rem = k % server_count;
    int start = 1;
    
    for (int i = 0; i < server_count; i++) {
        servers[i].start = start;
        int extra = (i < rem) ? 1 : 0;
        servers[i].end = start + per_server + extra - 1;
        servers[i].mod = mod;
        start = servers[i].end + 1;
        printf("[CLIENT] Server %d: range [%d, %d]\n", i, servers[i].start, servers[i].end);
    }
    
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);
    
    pthread_t threads[server_count];
    for (int i = 0; i < server_count; i++) {
        pthread_create(&threads[i], NULL, compute_on_server, &servers[i]);
    }
    
    long long total = 1;
    int failed = 0;
    for (int i = 0; i < server_count; i++) {
        pthread_join(threads[i], NULL);
        if (servers[i].success) {
            total = (total * servers[i].result) % mod;
        } else {
            failed++;
        }
    }
    
    gettimeofday(&tv_end, NULL);
    double elapsed = (tv_end.tv_sec - tv_start.tv_sec) * 1000.0;
    elapsed += (tv_end.tv_usec - tv_start.tv_usec) / 1000.0;
    
    printf("\n========================================\n");
    printf("RESULT:\n");
    printf("%d! mod %d = %lld\n", k, mod, total);
    printf("Time: %.3f ms\n", elapsed);
    if (failed > 0) printf("WARNING: %d servers failed\n", failed);
    printf("========================================\n");
    
    for (int i = 0; i < server_count; i++) free(servers[i].ip);
    free(servers);
    pthread_mutex_destroy(&print_mutex);
    
    return 0;
}