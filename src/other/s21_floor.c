#include <string.h>

#include "../s21_big_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  if (result == NULL) {
    return 1;
  }
  memset(result, 0, sizeof(*result));
  s21_big_decimal unpacked = {0};
  int status = s21_unpack(value, &unpacked);
  if (status == 0) {
    int fractional = 0;
    while (unpacked.scale > 0) {
      fractional |=
          s21_words_div_u32(unpacked.mantissa, S21_BIG_WORDS, 10) != 0;
      --unpacked.scale;
    }
    if (unpacked.sign != 0 && fractional != 0) {
      const uint32_t one[S21_BIG_WORDS] = {1};
      s21_words_add(unpacked.mantissa, one, unpacked.mantissa, S21_BIG_WORDS);
    }
    status = s21_big_to_dec(unpacked, 0, result);
  }
  return status;
}
