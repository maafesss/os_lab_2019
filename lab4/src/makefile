# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

# Список всех программ
PROGRAMS = process_memory zombie_demo zombie_forever

# Цель по умолчанию (собирает все программы)
all: $(PROGRAMS)

# Сборка process_memory (задание 1 - виртуальная память)
process_memory: process_memory.c
	$(CC) $(CFLAGS) process_memory.c -o process_memory $(LDFLAGS)

# Сборка zombie_demo (задание 3 - демонстрация зомби)
zombie_demo: zombie_demo.c
	$(CC) $(CFLAGS) zombie_demo.c -o zombie_demo $(LDFLAGS)

# Сборка zombie_forever (задание 3 - бесконечный зомби)
zombie_forever: zombie_forever.c
	$(CC) $(CFLAGS) zombie_forever.c -o zombie_forever $(LDFLAGS)

# Очистка собранных файлов
clean:
	rm -f $(PROGRAMS) *.o

# Полная очистка и пересборка
rebuild: clean all

# Запуск демонстрации process_memory
run_memory: process_memory
	./process_memory

# Запуск демонстрации зомби (30 секунд)
run_zombie: zombie_demo
	./zombie_demo

# Запуск бесконечного зомби (Ctrl+C для остановки)
run_zombie_forever: zombie_forever
	./zombie_forever

# Показать помощь
help:
	@echo "Доступные команды:"
	@echo "  make all              - собрать все программы"
	@echo "  make process_memory   - собрать программу для виртуальной памяти"
	@echo "  make zombie_demo      - собрать демо зомби (30 сек)"
	@echo "  make zombie_forever   - собрать бесконечного зомби"
	@echo "  make clean            - удалить все собранные файлы"
	@echo "  make rebuild          - пересобрать всё с нуля"
	@echo "  make run_memory       - собрать и запустить process_memory"
	@echo "  make run_zombie       - собрать и запустить zombie_demo"
	@echo "  make run_zombie_forever - собрать и запустить zombie_forever"
	@echo ""
	@echo "Для проверки зомби в другом терминале: ps aux | grep Z"

# Объявляем цели, которые не являются файлами
.PHONY: all clean rebuild run_memory run_zombie run_zombie_forever help
