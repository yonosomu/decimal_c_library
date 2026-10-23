CC = gcc
AR = ar
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic

SRC = common/s21_big_words.c common/s21_big_convert.c \
      arithmetic/s21_add.c arithmetic/s21_sub.c \
      arithmetic/s21_mul.c arithmetic/s21_div.c
OBJ = $(SRC:.c=.o)
TEST_SRC = $(wildcard tests/test_*.c) tests/check_compat.c

.PHONY: all test test-sanitize clean

all: s21_decimal.a

s21_decimal.a: $(OBJ)
	$(AR) rcs $@ $^

%.o: %.c s21_decimal.h s21_big_decimal.h
	$(CC) $(CFLAGS) -c $< -o $@

test:
	$(CC) $(CFLAGS) $(SRC) $(TEST_SRC) -o s21_test
	./s21_test

test-sanitize:
	$(CC) $(CFLAGS) -fsanitize=address,undefined -fno-omit-frame-pointer \
	  $(SRC) $(TEST_SRC) -o s21_test_san
	ASAN_OPTIONS=detect_leaks=1 ./s21_test_san

clean:
	rm -f $(OBJ) s21_decimal.a s21_test s21_test_san
