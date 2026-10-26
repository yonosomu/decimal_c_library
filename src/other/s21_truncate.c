#include <string.h>

#include "../s21_big_decimal.h"

int s21_truncate(s21_decimal value, s21_decimal *result) {
  if (result == NULL) {
    return 1;
  }
  memset(result, 0, sizeof(*result));
  s21_big_decimal unpacked = {0};
  int status = s21_unpack(value, &unpacked);
  if (status == 0) {
    while (unpacked.scale > 0) {
      s21_words_div_u32(unpacked.mantissa, S21_BIG_WORDS, 10);
      --unpacked.scale;
    }
    status = s21_big_to_dec(unpacked, 0, result);
  }
  return status;
}
