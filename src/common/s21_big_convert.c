#include <string.h>

#include "../s21_big_decimal.h"

int s21_unpack(s21_decimal src, s21_big_decimal *dst) {
  if (dst == NULL) {
    return 1;
  }
  uint32_t flags = 0;
  memcpy(&flags, &src.bits[3], sizeof(flags));
  int scale = (int)((flags >> 16) & UINT32_C(0xFF));
  int invalid = (flags & UINT32_C(0x7F00FFFF)) != 0 || scale > S21_MAX_SCALE;
  if (invalid == 0) {
    memset(dst, 0, sizeof(*dst));
    memcpy(dst->mantissa, src.bits, S21_DECIMAL_WORDS * sizeof(uint32_t));
    dst->scale = scale;
    dst->sign = (char)(flags >> 31);
  }
  return invalid;
}

int s21_unpack_pair(s21_decimal a, s21_decimal b, s21_big_decimal *left,
                    s21_big_decimal *right) {
  int status = s21_unpack(a, left);
  if (status == 0) {
    status = s21_unpack(b, right);
  }
  return status;
}

static int s21_raise_scale(s21_big_decimal *value, int target) {
  int overflow = 0;
  while (value->scale < target && overflow == 0) {
    overflow = s21_words_mul_u32(value->mantissa, S21_BIG_WORDS, 10) != 0;
    if (overflow == 0) {
      ++value->scale;
    }
  }
  return overflow;
}

int s21_align_scales(s21_big_decimal *a, s21_big_decimal *b) {
  if (a == NULL || b == NULL) {
    return 1;
  }
  int target = a->scale > b->scale ? a->scale : b->scale;
  int overflow = s21_raise_scale(a, target);
  if (overflow == 0) {
    overflow = s21_raise_scale(b, target);
  }
  return overflow;
}

static void s21_pack(const s21_big_decimal *value, s21_decimal *out) {
  uint32_t flags =
      ((uint32_t)value->scale << 16) | ((uint32_t)(value->sign != 0) << 31);
  memset(out, 0, sizeof(*out));
  memcpy(out->bits, value->mantissa, S21_DECIMAL_WORDS * sizeof(uint32_t));
  memcpy(&out->bits[3], &flags, sizeof(flags));
}

static int s21_round_up(uint32_t discarded, int sticky, uint32_t last) {
  int up = discarded > 5;
  if (discarded == 5) {
    up = sticky != 0 || (last & UINT32_C(1)) != 0;
  }
  return up;
}

static void s21_round_candidate(const s21_big_decimal *original, int drop,
                                int tail, s21_big_decimal *candidate) {
  *candidate = *original;
  int sticky = tail != 0;
  uint32_t discarded = 0;
  for (int i = 0; i < drop; ++i) {
    uint32_t digit = s21_words_div_u32(candidate->mantissa, S21_BIG_WORDS, 10);
    if (i + 1 < drop && digit != 0) {
      sticky = 1;
    }
    discarded = digit;
  }

  if (drop > 0 &&
      s21_round_up(discarded, sticky, candidate->mantissa[0]) != 0) {
    uint32_t const one[S21_BIG_WORDS] = {1};
    s21_words_add(candidate->mantissa, one, candidate->mantissa, S21_BIG_WORDS);
  }
}

int s21_big_to_dec(s21_big_decimal value, int sticky_tail, s21_decimal *out) {
  if (out == NULL || value.scale < 0 || value.scale > 2 * S21_MAX_SCALE) {
    return 1;
  }
  memset(out, 0, sizeof(*out));
  int status = value.sign != 0 ? 2 : 1;
  int drop = value.scale > S21_MAX_SCALE ? value.scale - S21_MAX_SCALE : 0;
  int found = 0;
  while (drop <= value.scale && found == 0) {
    s21_big_decimal candidate = {0};
    s21_round_candidate(&value, drop, sticky_tail, &candidate);
    if (s21_words_fits(candidate.mantissa, S21_BIG_WORDS, S21_DECIMAL_WORDS)) {
      candidate.scale = value.scale - drop;
      candidate.sign =
          s21_words_is_zero(candidate.mantissa, S21_BIG_WORDS) ? 0 : value.sign;
      s21_pack(&candidate, out);
      status = 0;
      found = 1;
    } else {
      ++drop;
    }
  }
  return status;
}
