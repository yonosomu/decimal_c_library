#include <math.h>
#include <stdint.h>

#include "../s21_big_decimal.h"
#include "../s21_decimal.h"

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
      long double max = 79228162514264337593543950335.0L;

      if (val < 1e-28L || val > max) {
        result = 1;
      } else {
        s21_big_decimal big = {0};
        big.sign = 0;
        if (src < 0.0f) {
          big.sign = 1;
        }

        int exponent = (int)floorl(log10l(val));
        long double factor = powl(10.0L, 6 - exponent);
        uint32_t number = (uint32_t)floorl(val * factor + 0.5L);
        if (number == 10000000u) {
          number = 1000000u;
          exponent++;
        }

        int scale = 6 - exponent;
        if (scale > S21_MAX_SCALE) {
          long double divisor = powl(10.0L, scale - S21_MAX_SCALE);
          number = (uint32_t)floorl((long double)number / divisor + 0.5L);
          scale = S21_MAX_SCALE;
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
          result = s21_big_to_dec(big, 0, dst);
        }
      }
    }
  }

  return result;
}
