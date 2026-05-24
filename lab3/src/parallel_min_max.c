#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

// Глобальные переменные для обработчика сигналов
pid_t *child_pids = NULL;
int child_count = 0;
int timeout_occurred = 0;

// Обработчик сигнала SIGALRM
void alarm_handler(int sig) {
    printf("\n[TIMEOUT] Timeout reached! Terminating child processes...\n");
    timeout_occurred = 1;
    
    // Посылаем SIGKILL всем дочерним процессам
    for (int i = 0; i < child_count; i++) {
        if (child_pids[i] > 0) {
            kill(child_pids[i], SIGKILL);
            printf("Killed child process: %d\n", child_pids[i]);
        }
    }
}

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int pnum = -1;
    int timeout = -1;  // таймаут в секундах (-1 = нет таймаута)
    bool with_files = false;

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {{"seed", required_argument, 0, 0},
                                          {"array_size", required_argument, 0, 0},
                                          {"pnum", required_argument, 0, 0},
                                          {"by_files", no_argument, 0, 'f'},
                                          {"timeout", required_argument, 0, 't'},
                                          {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        if (seed <= 0) {
                            printf("Seed must be a positive number\n");
                            return 1;
                        }
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        if (array_size <= 0) {
                            printf("Array size must be a positive number\n");
                            return 1;
                        }
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        if (pnum <= 0) {
                            printf("Number of processes must be a positive number\n");
                            return 1;
                        }
                        break;
                    case 3:
                        with_files = true;
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case 'f':
                with_files = true;
                break;
            case 't':  // обработка --timeout
                timeout = atoi(optarg);
                if (timeout <= 0) {
                    printf("Timeout must be a positive number\n");
                    return 1;
                }
                break;
            case '?':
                break;
            default:
                printf("getopt returned character code 0%o?\n", c);
        }
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"] [--by_files]\n",
               argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    
    // Создаем pipes если используем не файлы
    int (*pipes)[2] = NULL;
    if (!with_files) {
        pipes = (int (*)[2])malloc(pnum * sizeof(*pipes));
        for (int i = 0; i < pnum; i++) {
            if (pipe(pipes[i]) == -1) {
                printf("Pipe creation failed!\n");
                return 1;
            }
        }
    }
    
    // Массив для хранения PID дочерних процессов
    child_pids = (pid_t *)malloc(pnum * sizeof(pid_t));
    child_count = pnum;
    int active_child_processes = 0;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    // Устанавливаем обработчик сигнала SIGALRM
    if (timeout > 0) {
        signal(SIGALRM, alarm_handler);
        alarm(timeout);  // таймаут через timeout секунд
        printf("Timeout set to %d seconds\n", timeout);
    }

    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            active_child_processes += 1;
            if (child_pid == 0) {
                // Дочерний процесс - ищем min/max в своей части массива
                
                // Вычисляем границы для этого процесса
                int part_size = array_size / pnum;
                int start = i * part_size;
                int end = (i == pnum - 1) ? array_size - 1 : (i + 1) * part_size - 1;
                
                // Находим min/max в своей части
                struct MinMax result = GetMinMax(array, start, end);
                
                if (with_files) {
                    // Сохраняем результат в файл
                    char filename[256];
                    sprintf(filename, "temp_%d.txt", i);
                    FILE *f = fopen(filename, "w");
                    if (f != NULL) {
                        fprintf(f, "%d %d", result.min, result.max);
                        fclose(f);
                    }
                } else {
                    // Отправляем результат в pipe
                    close(pipes[i][0]);
                    write(pipes[i][1], &result, sizeof(result));
                    close(pipes[i][1]);
                }
                free(array);
                exit(0);
            } else {
                child_pids[i] = child_pid;  // сохраняем PID дочернего процесса
            }
        } else {
            printf("Fork failed!\n");
            return 1;
        }
    }

    // Ждем завершения дочерних процессов (с возможным таймаутом)
    int finished_count = 0;
    while (finished_count < pnum) {
        int status;
        pid_t result = waitpid(-1, &status, WNOHANG);  // неблокирующий wait
        
        if (result > 0) {
            // Процесс завершился
            finished_count++;
        } else if (result == 0) {
            // Нет завершившихся процессов
            if (timeout_occurred) {
                // Таймаут уже сработал, выходим
                printf("Timeout occurred, stopping waiting for children\n");
                break;
            }
            usleep(10000);  // ждем 10 мс перед следующей проверкой
        } else {
            // Ошибка
            if (errno != ECHILD) {
                perror("waitpid error");
            }
            break;
        }
    }

    // Отключаем таймер, если он был установлен
    if (timeout > 0) {
        alarm(0);
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    // Если был таймаут, выводим сообщение
    if (timeout_occurred) {
        printf("\n=== TIMEOUT OCCURRED ===\n");
        printf("Not all children completed successfully\n");
        printf("Results are partial!\n\n");
    }

    // Собираем результаты от завершившихся процессов
    for (int i = 0; i < pnum; i++) {
        int min = INT_MAX;
        int max = INT_MIN;
        
        // Проверяем, жив ли еще процесс
        if (!timeout_occurred || kill(child_pids[i], 0) != 0) {
            // Процесс мертв или таймаута не было, пытаемся прочитать результат
            if (with_files) {
                // Читаем из файлов
                char filename[256];
                sprintf(filename, "temp_%d.txt", i);
                FILE *f = fopen(filename, "r");
                if (f != NULL) {
                    fscanf(f, "%d %d", &min, &max);
                    fclose(f);
                    remove(filename);
                } else if (timeout_occurred) {
                    printf("Child %d: no result (likely killed by timeout)\n", i);
                    continue;
                }
            } else {
                // Читаем из pipes
                struct MinMax result;
                if (read(pipes[i][0], &result, sizeof(result)) > 0) {
                    min = result.min;
                    max = result.max;
                } else if (timeout_occurred) {
                    printf("Child %d: no result (likely killed by timeout)\n", i);
                    continue;
                }
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
        } else {
            printf("Child %d: still alive (killed?)\n", i);
            continue;
        }

        if (min < min_max.min) min_max.min = min;
        if (max > min_max.max) min_max.max = max;
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    // Освобождаем память
    free(array);
    free(child_pids);
    if (!with_files && pipes != NULL) {
        free(pipes);
    }

    printf("\n=== FINAL RESULT ===\n");
    if (min_max.min != INT_MAX) {
        printf("Min: %d\n", min_max.min);
        printf("Max: %d\n", min_max.max);
    } else {
        printf("No valid results (all children killed?)\n");
    }
    printf("Elapsed time: %fms\n", elapsed_time);
    if (timeout_occurred) {
        printf("Note: Program stopped due to timeout (%d seconds)\n", timeout);
    }
    fflush(NULL);
    return timeout_occurred ? 2 : 0;
}
