CC = gcc
AR = ar
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic
CPPFLAGS =
LDFLAGS =
LDLIBS =
BUILD_DIR ?= .build/normal
LIBRARY ?= s21_decimal.a
SANITIZERS ?= undefined
SANITIZER_OPTIONS ?= -fno-omit-frame-pointer -fno-sanitize-recover=all
ASAN_OPTIONS ?= detect_leaks=$(if $(filter Darwin,$(shell uname -s)),0,1)
CHECK_CFLAGS = $(shell pkg-config --cflags check | tr ' ' '\n' | sed 's/^-I/-isystem /' | tr '\n' ' ')
CHECK_LIBS = $(shell pkg-config --libs check)
GCOVR ?= gcovr
GCOV ?= $(if $(findstring clang,$(shell $(CC) --version)),$(if $(filter Darwin,$(shell uname -s)),xcrun llvm-cov gcov,llvm-cov gcov),gcov)

SRC = common/s21_big_words.c common/s21_big_convert.c \
      arithmetic/s21_add.c arithmetic/s21_sub.c \
      arithmetic/s21_mul.c arithmetic/s21_div.c $(wildcard other/*.c)
OBJ = $(addprefix $(BUILD_DIR)/,$(SRC:.c=.o))
TEST_SRC = $(wildcard tests/test_*.c)
TEST_OBJ = $(addprefix $(BUILD_DIR)/,$(TEST_SRC:.c=.o))
DEPS = $(OBJ:.o=.d) $(TEST_OBJ:.o=.d)
TEST_BIN = $(BUILD_DIR)/s21_test

.PHONY: all test test-sanitize gcov_report clean

all: $(LIBRARY)

$(LIBRARY): $(OBJ)
	@mkdir -p $(@D)
	$(AR) rcs $@ $^

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(TEST_OBJ): $(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CHECK_CFLAGS) -MMD -MP -c $< -o $@

$(TEST_BIN): $(TEST_OBJ) $(LIBRARY)
	$(CC) $(CFLAGS) $(LDFLAGS) $(TEST_OBJ) $(LIBRARY) $(CHECK_LIBS) $(LDLIBS) -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

test-sanitize:
	rm -rf .build/sanitize
	ASAN_OPTIONS='$(ASAN_OPTIONS)' CK_FORK=no $(MAKE) test \
	  BUILD_DIR=.build/sanitize LIBRARY=.build/sanitize/s21_decimal.a \
	  CFLAGS='$(CFLAGS) -fsanitize=$(SANITIZERS) $(SANITIZER_OPTIONS)' \
	  LDFLAGS='$(LDFLAGS) -fsanitize=$(SANITIZERS)'

gcov_report:
	rm -rf .build/coverage
	CK_FORK=no $(MAKE) test BUILD_DIR=.build/coverage \
	  LIBRARY=.build/coverage/s21_decimal.a \
	  CFLAGS='$(CFLAGS) -O0 -g --coverage' LDFLAGS='$(LDFLAGS) --coverage'
	$(GCOVR) --root . --filter 'common/|arithmetic/|other/' \
	  --gcov-executable '$(GCOV)' --html-details .build/coverage/index.html \
	  --json .build/coverage/coverage.json --print-summary .build/coverage

clean:
	rm -rf .build
	rm -f $(SRC:.c=.o) s21_decimal.a s21_test s21_test_san

-include $(DEPS)
