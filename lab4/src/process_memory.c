#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Глобальные переменные
int global_initialized = 10;      // инициализированная глобальная (data segment)
int global_uninitialized;          // неинициализированная глобальная (bss segment)
const int const_global = 100;      // константная глобальная (rodata)

// Функция для демонстрации
void test_function() {
    int local = 42;                 // локальная переменная (stack)
    static int static_var = 5;      // статическая переменная (data segment)
    static int static_uninit;        // статическая неинициализированная (bss segment)
    
    printf("  Inside function:\n");
    printf("    local (stack): %p\n", (void*)&local);
    printf("    static_var (data): %p\n", (void*)&static_var);
    printf("    static_uninit (bss): %p\n", (void*)&static_uninit);
}

// Внешние переменные, определенные линковщиком
extern char etext, edata, end;

int main(int argc, char *argv[]) {
    int local_stack = 123;          // локальная переменная (stack)
    int *heap_var = malloc(sizeof(int));  // динамическая память (heap)
    
    printf("========================================\n");
    printf("Process Memory Layout Demonstration\n");
    printf("========================================\n\n");
    
    // Сегмент TEXT (код программы)
    printf("--- TEXT SEGMENT (code) ---\n");
    printf("  main function: %p\n", (void*)main);
    printf("  test_function: %p\n", (void*)test_function);
    printf("  etext (end of text segment): %p\n\n", (void*)&etext);
    
    // Сегмент DATA (инициализированные данные)
    printf("--- DATA SEGMENT (initialized data) ---\n");
    printf("  global_initialized: %p\n", (void*)&global_initialized);
    printf("  edata (end of data segment): %p\n\n", (void*)&edata);
    
    // Сегмент BSS (неинициализированные данные)
    printf("--- BSS SEGMENT (uninitialized data) ---\n");
    printf("  global_uninitialized: %p\n", (void*)&global_uninitialized);
    printf("  end (end of bss segment): %p\n\n", (void*)&end);
    
    // Константные данные
    printf("--- RODATA SEGMENT (read-only data) ---\n");
    printf("  const_global: %p\n\n", (void*)&const_global);
    
    // HEAP (куча)
    printf("--- HEAP SEGMENT (dynamic memory) ---\n");
    printf("  heap_var: %p\n", (void*)heap_var);
    printf("  sbrk(0): %p\n\n", (void*)sbrk(0));
    
    // STACK (стек)
    printf("--- STACK SEGMENT ---\n");
    printf("  local_stack: %p\n", (void*)&local_stack);
    printf("  argc: %p\n", (void*)&argc);
    printf("  argv: %p\n\n", (void*)&argv);
    
    // Вызов функции для показа стековых адресов
    printf("--- FUNCTION CALL STACK ---\n");
    test_function();
    
    printf("\n========================================\n");
    printf("Memory Layout (typical x86_64 Linux):\n");
    printf("========================================\n");
    printf("HIGH ADDRESS\n");
    printf("| Stack     | <- grows downward\n");
    printf("| ...       |\n");
    printf("| Heap      | <- grows upward\n");
    printf("| BSS       | (uninitialized data)\n");
    printf("| Data      | (initialized data)\n");
    printf("| Text      | (code)\n");
    printf("LOW ADDRESS\n");
    
    free(heap_var);
    return 0;
}
