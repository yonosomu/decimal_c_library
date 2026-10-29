#include <math.h>
#include <stdint.h>

#include "../s21_big_decimal.h"
#include "../s21_decimal.h"

static uint32_t s21_float_round_even(long double value) {
  long double integral = floorl(value);
  uint32_t result = (uint32_t)integral;
  long double fraction = value - integral;
  if (fraction > 0.5L || (fraction == 0.5L && (result & 1u) != 0)) {
    ++result;
  }
  return result;
}

int s21_from_float_to_decimal(float src, s21_decimal* dst) {
  int result = 0;

  if (dst == ((void*)0)) {
    result = 1;
  } else {
    dst->bits[0] = 0;
    dst->bits[1] = 0;
    dst->bits[2] = 0;
    dst->bits[3] = 0;

    if (isnan(src) || isinf(src)) {
      result = 1;
    } else if (src == 0.0f) {
      if (signbit(src)) {
        dst->bits[3] |= (1u << 31);
      }
    } else {
      long double val = fabsl((long double)src);
      long double limit = ldexpl(1.0L, 96);

      if (val < 1e-28L || val >= limit) {
        result = 1;
      } else {
        s21_big_decimal big = {0};
        big.sign = 0;
        if (src < 0.0f) {
          big.sign = 1;
        }

        int exponent = (int)floorl(log10l(val));
        int scale = 6 - exponent;
        if (scale > S21_MAX_SCALE) {
          scale = S21_MAX_SCALE;
        }
        long double factor = powl(10.0L, scale);
        uint32_t number = s21_float_round_even(val * factor);
        if (number == 10000000u) {
          number = 1000000u;
          --scale;
        }

        big.mantissa[0] = number;
        if (scale < 0) {
          for (int i = 0; i < -scale; i++) {
            if (s21_words_mul_u32(big.mantissa, S21_BIG_WORDS, 10)) {
              result = 1;
            }
          }
          scale = 0;
        }
        big.scale = scale;

        if (result == 0) {
          result = s21_big_to_dec(big, 0, dst) != 0;
        }
      }
    }
  }

  return result;
}
