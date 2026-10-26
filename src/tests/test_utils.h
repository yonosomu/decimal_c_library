#ifndef S21_TEST_UTILS_H_
#define S21_TEST_UTILS_H_

#include <check.h>
#include <stdint.h>

#include "../s21_decimal.h"

s21_decimal s21_test_make(uint32_t lo, uint32_t mid, uint32_t hi,
                          int scale, int negative);
void s21_test_assert_decimal(s21_decimal value, uint32_t lo, uint32_t mid,
                             uint32_t hi, int scale, int negative);

#endif
