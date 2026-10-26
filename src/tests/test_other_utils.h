#ifndef S21_TEST_OTHER_UTILS_H_
#define S21_TEST_OTHER_UTILS_H_

#include "test_utils.h"

typedef int (*s21_test_unary)(s21_decimal, s21_decimal *);
void s21_test_unary_invalid(s21_test_unary function);

#endif
