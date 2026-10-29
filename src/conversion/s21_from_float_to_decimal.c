#include <math.h>
#include <stdint.h>
#include <string.h>

#include "../s21_big_decimal.h"

static uint32_t s21_float_coefficient(float src, int scale) {
  int exponent = 0;
  float fraction = frexpf(fabsf(src), &exponent);
  uint32_t numerator[S21_BIG_WORDS] = {(uint32_t)ldexpf(fraction, 24)};
  uint32_t denominator[S21_BIG_WORDS] = {1};
  uint32_t quotient[S21_BIG_WORDS] = {0};
  uint32_t remainder[S21_BIG_WORDS] = {0};
  exponent -= 24;
  uint32_t *binary = exponent < 0 ? denominator : numerator;
  int binary_steps = exponent < 0 ? -exponent : exponent;
  for (int i = 0; i < binary_steps; ++i) {
    s21_words_shl(binary, S21_BIG_WORDS);
  }
  uint32_t *decimal = scale < 0 ? denominator : numerator;
  int decimal_steps = scale < 0 ? -scale : scale;
  for (int i = 0; i < decimal_steps; ++i) {
    s21_words_mul_u32(decimal, S21_BIG_WORDS, 10);
  }
  s21_words_divmod(numerator, denominator, quotient, remainder);
  s21_words_shl(remainder, S21_BIG_WORDS);
  int comparison = s21_words_cmp(remainder, denominator, S21_BIG_WORDS);
  uint32_t result = quotient[0];
  if (comparison > 0 || (comparison == 0 && (result & 1u) != 0)) {
    ++result;
  }
  return result;
}

static int s21_float_build(float src, s21_decimal *dst) {
  long double value = fabsl((long double)src);
  int scale = 6 - (int)floorl(log10l(value));
  if (scale > S21_MAX_SCALE) {
    scale = S21_MAX_SCALE;
  }
  uint32_t number = s21_float_coefficient(src, scale);
  if (number == 10000000u) {
    number = 1000000u;
    --scale;
  }
  s21_big_decimal big = {.mantissa = {number}, .sign = (char)(src < 0.0f)};
  int result = 0;
  for (int i = 0; i < -scale && result == 0; ++i) {
    result = s21_words_mul_u32(big.mantissa, S21_BIG_WORDS, 10) != 0;
  }
  big.scale = scale < 0 ? 0 : scale;
  if (result == 0) {
    result = s21_big_to_dec(big, 0, dst) != 0;
  }
  return result;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int result = 1;
  if (dst != NULL) {
    memset(dst, 0, sizeof(*dst));
    long double value = fabsl((long double)src);
    if (isfinite(src) && src == 0.0f) {
      uint32_t flags = signbit(src) ? UINT32_C(0x80000000) : 0;
      memcpy(&dst->bits[3], &flags, sizeof(flags));
      result = 0;
    } else if (isfinite(src) && value >= 1e-28L && value < ldexpl(1.0L, 96)) {
      result = s21_float_build(src, dst);
    }
  }
  return result;
}
