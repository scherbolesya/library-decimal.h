CC = gcc
PLATFORM = $(shell uname)
CFLAGS = -Wall -Werror -Wextra -std=c11 
TFLAGS = `pkg-config --cflags --libs check` --coverage
SRC = arithmetic.c comparison.c converters.c other_functions.c
OBJ = *.o
TESTS_SRC = s21_test.c

ifeq ($(PLATFORM), Linux)
OPEN = xdg-open 
else
OPEN = open
endif

all: clean s21_decimal.a test gcov_report

s21_decimal.a: $(OBJ) s21_decimal.h
	$(CC) $(CFLAGS) -c $(SRC)
	ar rcs $@ $(OBJ)
	ranlib $@

test: s21_decimal.a
	$(CC) *.c -o test_check $(CFLAGS) $(TFLAGS) s21_decimal.a
	./test_check

gcov_report: test
	lcov -t "gcov_report" -o test.info -c -d .
	genhtml -o gcov_report test.info
	$(OPEN) ./gcov_report/index.html

clean:
	rm -f test_check
	rm -f *.info
	rm -f *.o
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.a
	rm -f *.out
	rm -f *.css
	rm -fr gcov_report

check: clean
	clang-format -n *.c *.h
