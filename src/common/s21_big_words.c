#include <string.h>

#include "../s21_big_decimal.h"

void s21_words_zero(uint32_t *first, size_t count) {
  uint32_t *cursor = first;
  const uint32_t *end = first + count;
  while (cursor != end) {
    *cursor = 0;
    ++cursor;
  }
}

int s21_words_is_zero(const uint32_t *first, size_t count) {
  int zero = 1;
  const uint32_t *cursor = first;
  const uint32_t *end = first + count;
  while (cursor != end && zero != 0) {
    zero = (*cursor == 0);
    ++cursor;
  }
  return zero;
}

int s21_words_fits(const uint32_t *first, size_t count, size_t kept) {
  int fits = kept <= count;
  if (fits != 0) {
    const uint32_t *cursor = first + kept;
    const uint32_t *end = first + count;
    while (cursor != end && fits != 0) {
      fits = (*cursor == 0);
      ++cursor;
    }
  }
  return fits;
}

int s21_words_cmp(const uint32_t *a, const uint32_t *b, size_t count) {
  int comparison = 0;
  const uint32_t *pa = a + count;
  const uint32_t *pb = b + count;
  while (pa != a && comparison == 0) {
    --pa;
    --pb;
    if (*pa > *pb) {
      comparison = 1;
    } else if (*pa < *pb) {
      comparison = -1;
    }
  }
  return comparison;
}

int s21_words_get_bit(const uint32_t *first, size_t count, size_t index) {
  int bit = 0;
  if (index < count * S21_WORD_BITS) {
    const uint32_t *word = first + index / S21_WORD_BITS;
    bit = (int)((*word >> (index % S21_WORD_BITS)) & UINT32_C(1));
  }
  return bit;
}

void s21_words_set_bit(uint32_t *first, size_t count, size_t index, int bit) {
  if (index < count * S21_WORD_BITS) {
    uint32_t *word = first + index / S21_WORD_BITS;
    uint32_t mask = UINT32_C(1) << (index % S21_WORD_BITS);
    if (bit != 0) {
      *word |= mask;
    } else {
      *word &= ~mask;
    }
  }
}

uint32_t s21_words_add(const uint32_t *a, const uint32_t *b, uint32_t *out,
                       size_t count) {
  uint64_t carry = 0;
  const uint32_t *end = a + count;
  while (a != end) {
    uint64_t part = (uint64_t)*a + *b + carry;
    *out = (uint32_t)part;
    carry = part >> S21_WORD_BITS;
    ++a;
    ++b;
    ++out;
  }
  return (uint32_t)carry;
}

uint32_t s21_words_sub(const uint32_t *a, const uint32_t *b, uint32_t *out,
                       size_t count) {
  uint64_t borrow = 0;
  const uint32_t *end = a + count;
  while (a != end) {
    uint64_t subtrahend = (uint64_t)*b + borrow;
    uint64_t difference = (uint64_t)*a - subtrahend;
    borrow = ((uint64_t)*a < subtrahend);
    *out = (uint32_t)difference;
    ++a;
    ++b;
    ++out;
  }
  return (uint32_t)borrow;
}

uint32_t s21_words_shl(uint32_t *first, size_t count) {
  uint32_t carry = 0;
  uint32_t *cursor = first;
  const uint32_t *end = first + count;
  while (cursor != end) {
    uint32_t next = *cursor >> (S21_WORD_BITS - 1);
    *cursor = (*cursor << 1) | carry;
    carry = next;
    ++cursor;
  }
  return carry;
}

uint32_t s21_words_mul_u32(uint32_t *first, size_t count, uint32_t factor) {
  uint64_t carry = 0;
  uint32_t *cursor = first;
  const uint32_t *end = first + count;
  while (cursor != end) {
    uint64_t part = (uint64_t)*cursor * factor + carry;
    *cursor = (uint32_t)part;
    carry = part >> S21_WORD_BITS;
    ++cursor;
  }
  return (uint32_t)carry;
}

uint32_t s21_words_div_u32(uint32_t *first, size_t count, uint32_t divisor) {
  if (divisor == 0) {
    return 0;
  }
  uint64_t remainder = 0;
  uint32_t *cursor = first + count;
  while (cursor != first) {
    --cursor;
    uint64_t part = (remainder << S21_WORD_BITS) | *cursor;
    *cursor = (uint32_t)(part / divisor);
    remainder = part % divisor;
  }
  return (uint32_t)remainder;
}

static int s21_mul_row(uint32_t ai, const uint32_t *b, uint32_t *out,
                       size_t row) {
  uint64_t carry = 0;
  const uint32_t *pb = b;
  uint32_t *pr = out + row;
  const uint32_t *limit = b + S21_BIG_WORDS - row;
  int overflow = 0;
  while (pb != limit) {
    uint64_t part = (uint64_t)ai * *pb + *pr + carry;
    *pr = (uint32_t)part;
    carry = part >> S21_WORD_BITS;
    ++pb;
    ++pr;
  }
  overflow = carry != 0;
  while (pb != b + S21_BIG_WORDS) {
    if (ai != 0 && *pb != 0) {
      overflow = 1;
    }
    ++pb;
  }
  return overflow;
}

int s21_words_mul(const uint32_t *a, const uint32_t *b, uint32_t *out) {
  uint32_t product[S21_BIG_WORDS] = {0};
  const uint32_t *pa = a;
  int overflow = 0;
  while (pa != a + S21_BIG_WORDS) {
    overflow |= s21_mul_row(*pa, b, product, (size_t)(pa - a));
    ++pa;
  }
  memcpy(out, product, sizeof(product));
  return overflow;
}

static void s21_div_bit(const uint32_t *num, const uint32_t *den, uint32_t *q,
                        uint32_t *r, size_t bit) {
  uint32_t high = s21_words_shl(r, S21_BIG_WORDS);
  int new_bit = s21_words_get_bit(num, S21_BIG_WORDS, bit);
  s21_words_set_bit(r, S21_BIG_WORDS, 0, new_bit);
  if (high != 0 || s21_words_cmp(r, den, S21_BIG_WORDS) >= 0) {
    s21_words_sub(r, den, r, S21_BIG_WORDS);
    s21_words_set_bit(q, S21_BIG_WORDS, bit, 1);
  }
}

int s21_words_divmod(const uint32_t *num, const uint32_t *den, uint32_t *quot,
                     uint32_t *rem) {
  if (s21_words_is_zero(den, S21_BIG_WORDS) != 0) {
    return 1;
  }
  uint32_t q[S21_BIG_WORDS] = {0};
  uint32_t r[S21_BIG_WORDS] = {0};
  size_t bit = S21_BIG_WORDS * S21_WORD_BITS;
  while (bit != 0) {
    --bit;
    s21_div_bit(num, den, q, r, bit);
  }
  memcpy(quot, q, sizeof(q));
  memcpy(rem, r, sizeof(r));
  return 0;
}
