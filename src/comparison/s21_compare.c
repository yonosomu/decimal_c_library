#include "../s21_big_decimal.h"
#include "../s21_decimal.h"

static int decimal_cmp(s21_decimal a, s21_decimal b) {
  s21_big_decimal big_a, big_b;
  s21_unpack_pair(a, b, &big_a, &big_b);

  int is_zero_a = s21_words_is_zero(big_a.mantissa, S21_BIG_WORDS);
  int is_zero_b = s21_words_is_zero(big_b.mantissa, S21_BIG_WORDS);

  if (is_zero_a && is_zero_b) {
    return 0;
  }

  if (is_zero_a) big_a.sign = 0;
  if (is_zero_b) big_b.sign = 0;

  if (big_a.sign != big_b.sign) {
    if (big_a.sign) {
      return -1;
    } else {
      return 1;
    }
  }

  s21_align_scales(&big_a, &big_b);

  int cmp = s21_words_cmp(big_a.mantissa, big_b.mantissa, S21_BIG_WORDS);

  if (big_a.sign) {
    cmp = -cmp;
  }

  return cmp;
}

int s21_is_less(s21_decimal a, s21_decimal b) {
  int result = 0;
  if (decimal_cmp(a, b) < 0) {
    result = 1;
  }
  return result;
}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
  int result = 0;
  if (decimal_cmp(a, b) <= 0) {
    result = 1;
  }
  return result;
}

int s21_is_greater(s21_decimal a, s21_decimal b) {
  int result = 0;
  if (decimal_cmp(a, b) > 0) {
    result = 1;
  }
  return result;
}

int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
  int result = 0;
  if (decimal_cmp(a, b) >= 0) {
    result = 1;
  }
  return result;
}

int s21_is_equal(s21_decimal a, s21_decimal b) {
  int result = 0;
  if (decimal_cmp(a, b) == 0) {
    result = 1;
  }
  return result;
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
  int result = 0;
  if (decimal_cmp(a, b) != 0) {
    result = 1;
  }
  return result;
}