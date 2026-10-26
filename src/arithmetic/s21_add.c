#include <string.h>

#include "../s21_big_decimal.h"

static int s21_combine(const s21_big_decimal *a, const s21_big_decimal *b,
                       s21_big_decimal *out) {
  int overflow = 0;
  char sign = 0;
  int cmp = s21_words_cmp(a->mantissa, b->mantissa, S21_BIG_WORDS);
  if (a->sign == b->sign) {
    overflow = (int)s21_words_add(a->mantissa, b->mantissa, out->mantissa,
                                  S21_BIG_WORDS);
    sign = a->sign;
  } else if (cmp >= 0) {
    s21_words_sub(a->mantissa, b->mantissa, out->mantissa, S21_BIG_WORDS);
    sign = a->sign;
  } else {
    s21_words_sub(b->mantissa, a->mantissa, out->mantissa, S21_BIG_WORDS);
    sign = b->sign;
  }
  out->scale = a->scale;
  out->sign = s21_words_is_zero(out->mantissa, S21_BIG_WORDS) ? 0 : sign;
  return overflow;
}

int s21_add_core(s21_decimal first, s21_decimal second, int subtract_b,
                 s21_decimal *result) {
  if (result == NULL) {
    return 1;
  }
  memset(result, 0, sizeof(*result));
  s21_big_decimal a = {0};
  s21_big_decimal b = {0};
  s21_big_decimal total = {0};
  int status = s21_unpack_pair(first, second, &a, &b);
  if (status == 0) {
    b.sign ^= (char)(subtract_b != 0);
    status = s21_align_scales(&a, &b);
  }
  if (status == 0) {
    status = s21_combine(&a, &b, &total);
    if (status != 0) {
      status = total.sign != 0 ? 2 : 1;
    }
  }
  if (status == 0) {
    status = s21_big_to_dec(total, 0, result);
  }
  return status;
}

int s21_add(s21_decimal a, s21_decimal b, s21_decimal *result) {
  return s21_add_core(a, b, 0, result);
}
