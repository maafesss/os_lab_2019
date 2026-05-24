#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
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
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
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
  
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

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
          close(pipes[i][0]);  // закрываем чтение
          write(pipes[i][1], &result, sizeof(result));
          close(pipes[i][1]);  // закрываем запись
        }
        free(array);
        exit(0);
      }
    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  // Ждем завершения всех дочерних процессов
  while (active_child_processes > 0) {
    wait(NULL);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  // Собираем результаты от всех процессов
  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // Читаем из файлов
      char filename[256];
      sprintf(filename, "temp_%d.txt", i);
      FILE *f = fopen(filename, "r");
      if (f != NULL) {
        fscanf(f, "%d %d", &min, &max);
        fclose(f);
        remove(filename);  // удаляем временный файл
      }
    } else {
      // Читаем из pipes
      struct MinMax result;
      close(pipes[i][1]);  // закрываем запись
      read(pipes[i][0], &result, sizeof(result));
      close(pipes[i][0]);  // закрываем чтение
      min = result.min;
      max = result.max;
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
  if (!with_files && pipes != NULL) {
    free(pipes);
  }

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
