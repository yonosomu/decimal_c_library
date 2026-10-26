#include "../s21_big_decimal.h"

int s21_sub(s21_decimal a, s21_decimal b, s21_decimal *result) {
  return s21_add_core(a, b, 1, result);
}
