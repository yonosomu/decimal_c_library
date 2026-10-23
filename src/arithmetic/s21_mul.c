#include <string.h>

#include "../s21_big_decimal.h"

int s21_mul(s21_decimal first, s21_decimal second, s21_decimal *result) {
  if (result == NULL) {
    return 1;
  }
  memset(result, 0, sizeof(*result));
  s21_big_decimal a = {0};
  s21_big_decimal b = {0};
  s21_big_decimal product = {0};
  int status = s21_unpack_pair(first, second, &a, &b);
  if (status == 0) {
    status = s21_words_mul(a.mantissa, b.mantissa, product.mantissa);
    product.scale = a.scale + b.scale;
    product.sign = a.sign ^ b.sign;
    if (status != 0) {
      status = product.sign != 0 ? 2 : 1;
    }
  }
  if (status == 0) {
    status = s21_big_to_dec(product, 0, result);
  }
  return status;
}
