#!/bin/bash
gcc tests.c -I../revert_string -L../revert_string -lrevert_string -lcunit -o test_program
echo "Готово! Запусти: LD_LIBRARY_PATH=../revert_string ./test_program"
