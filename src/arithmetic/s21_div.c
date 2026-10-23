#include <string.h>

#include "../s21_big_decimal.h"

static int s21_mul_pow10(uint32_t *mantissa, int power) {
  int overflow = 0;
  for (int i = 0; i < power && overflow == 0; ++i) {
    overflow = s21_words_mul_u32(mantissa, S21_BIG_WORDS, 10) != 0;
  }
  return overflow;
}

static int s21_div_prepare(s21_big_decimal *num, s21_big_decimal *den) {
  int scale_num = num->scale;
  int scale_den = den->scale;
  int overflow = s21_mul_pow10(num->mantissa, scale_den);
  if (overflow == 0) {
    overflow = s21_mul_pow10(den->mantissa, scale_num);
  }
  if (overflow == 0) {
    num->scale = 0;
    den->scale = 0;
  }
  return overflow;
}

static int s21_div_digit(s21_big_decimal *q, uint32_t *rem,
                         const uint32_t *den) {
  uint32_t digit[S21_BIG_WORDS] = {0};
  uint32_t next_rem[S21_BIG_WORDS] = {0};
  int overflow = s21_words_mul_u32(rem, S21_BIG_WORDS, 10) != 0;
  if (overflow == 0) {
    overflow = s21_words_divmod(rem, den, digit, next_rem);
  }
  if (overflow == 0) {
    overflow = s21_words_mul_u32(q->mantissa, S21_BIG_WORDS, 10) != 0;
  }
  if (overflow == 0) {
    overflow =
        (int)s21_words_add(q->mantissa, digit, q->mantissa, S21_BIG_WORDS);
  }
  if (overflow == 0) {
    memcpy(rem, next_rem, sizeof(next_rem));
    ++q->scale;
  }
  return overflow;
}

static int s21_div_frac(s21_big_decimal *q, uint32_t *rem, const uint32_t *den,
                        int *sticky) {
  int overflow = 0;
  while (s21_words_is_zero(rem, S21_BIG_WORDS) == 0 &&
         q->scale < (S21_MAX_SCALE + 1) && overflow == 0) {
    overflow = s21_div_digit(q, rem, den);
  }
  *sticky = s21_words_is_zero(rem, S21_BIG_WORDS) == 0;
  return overflow;
}

static int s21_div_core(s21_big_decimal num, s21_big_decimal den,
                        s21_decimal *result) {
  char sign = num.sign ^ den.sign;
  int status = s21_div_prepare(&num, &den);
  s21_big_decimal quotient = {0};
  uint32_t rem[S21_BIG_WORDS] = {0};
  int sticky = 0;
  if (status == 0) {
    status =
        s21_words_divmod(num.mantissa, den.mantissa, quotient.mantissa, rem);
  }
  if (status == 0 && s21_words_fits(quotient.mantissa, S21_BIG_WORDS,
                                    S21_DECIMAL_WORDS) == 0) {
    status = 1;
  }
  if (status == 0) {
    status = s21_div_frac(&quotient, rem, den.mantissa, &sticky);
  }
  if (status == 0) {
    quotient.sign = sign;
    status = s21_big_to_dec(quotient, sticky, result);
  } else {
    status = sign != 0 ? 2 : 1;
  }
  return status;
}

int s21_div(s21_decimal first, s21_decimal second, s21_decimal *result) {
  if (result == NULL) {
    return 1;
  }
  memset(result, 0, sizeof(*result));
  s21_big_decimal num = {0};
  s21_big_decimal den = {0};
  int status = s21_unpack_pair(first, second, &num, &den);
  if (status == 0 && s21_words_is_zero(den.mantissa, S21_BIG_WORDS)) {
    status = 3;
  }
  if (status == 0 && !s21_words_is_zero(num.mantissa, S21_BIG_WORDS)) {
    status = s21_div_core(num, den, result);
  }
  return status;
}
