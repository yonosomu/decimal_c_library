#include <limits.h>
#include <stdint.h>

#include "../s21_big_decimal.h"
#include "../s21_decimal.h"

int s21_from_decimal_to_int(s21_decimal src, int* dst) {
  int result = 0;

  if (dst == ((void*)0)) {
    result = 1;
  } else {
    s21_big_decimal big = {0};
    if (s21_unpack(src, &big) != 0) {
      return 1;
    }

    for (int i = 0; i < big.scale; i++) {
      s21_words_div_u32(big.mantissa, S21_BIG_WORDS, 10);
    }

    int overflow = 0;
    for (int i = 1; i < S21_BIG_WORDS; i++) {
      if (big.mantissa[i] != 0) {
        overflow = 1;
      }
    }

    if (overflow) {
      result = 1;
    } else {
      uint32_t val = big.mantissa[0];

      if (big.sign) {
        uint32_t limit = (uint32_t)INT_MAX + 1u;
        if (val > limit) {
          result = 1;
        } else if (val == limit) {
          *dst = INT_MIN;
        } else {
          *dst = -(int)val;
        }
      } else {
        if (val > (uint32_t)INT_MAX) {
          result = 1;
        } else {
          *dst = (int)val;
        }
      }
    }
  }

  return result;
}
