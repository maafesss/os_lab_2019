# Лабораторная работа №5

## Задание 1

### Необходимые знания

cd /workspaces/os_lab_2019/lab4/src
nano mutex.c

### Ресурсы

1. [Туториал по POSIX threads от университета Карнеги-Меллона](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html#SCHEDULING)
2. [Статья о Race condition [wikipedia]](https://en.wikipedia.org/wiki/Race_condition)
3. [Статья о Critical Section [wikipedia]](https://en.wikipedia.org/wiki/Critical_section)

## Задание 2

### Необходимые знания

1. POSIX threads: как создавать, как дожидаться завершения.
2. Как линковаться на бибилотеку `pthread`
3. Как использовать мьютексы.

Написать программу для паралелльного вычисления факториала по модулю `mod` (`k!`), которая будет принимать на вход следующие параметры (пример: `-k 10 --pnum=4 --mod=10`):

1. `k` - число, факториал которого необходимо вычислить.
2. `pnum` - количество потоков.
3. `mod` - модуль факториала

Для синхронизации результатов необходимо использовать мьютексы.

### Ресурсы

1. [Туториал по POSIX threads от университета Карнеги-Меллона](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html#SCHEDULING)

## Задание 3

### Необходимые знания

1. Состояние deadlock

Напишите программу для демонстрации состояния deadlock.

### Ресурсы

1. [Статья о deadlock [wikipedia]](https://en.wikipedia.org/wiki/Deadlock)

## Перед тем, как сдавать

Залейте ваш код в ваш репозиторий на GitHub. Убедитесь, что вы не добавляете в репозиторий бинарные файлы (программы, утилиты, библиотеки и т.д.).




