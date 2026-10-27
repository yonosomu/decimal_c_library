#ifndef S21_BIG_DECIMAL_H_
#define S21_BIG_DECIMAL_H_

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#include "s21_decimal.h"

_Static_assert(sizeof(int) * CHAR_BIT == 32,
               "s21_decimal needs 32-bit int words");

#define S21_BIG_WORDS 8
#define S21_WORD_BITS 32

typedef struct {
  uint32_t mantissa[S21_BIG_WORDS];
  int scale;
  char sign;
} s21_big_decimal;

void s21_words_zero(uint32_t *first, size_t count);
int s21_words_is_zero(const uint32_t *first, size_t count);
int s21_words_fits(const uint32_t *first, size_t count, size_t kept);
int s21_words_cmp(const uint32_t *a, const uint32_t *b, size_t count);
int s21_words_get_bit(const uint32_t *first, size_t count, size_t index);
void s21_words_set_bit(uint32_t *first, size_t count, size_t index, int bit);
uint32_t s21_words_add(const uint32_t *a, const uint32_t *b, uint32_t *out,
                       size_t count);
uint32_t s21_words_sub(const uint32_t *a, const uint32_t *b, uint32_t *out,
                       size_t count);
uint32_t s21_words_shl(uint32_t *first, size_t count);
uint32_t s21_words_mul_u32(uint32_t *first, size_t count, uint32_t factor);
uint32_t s21_words_div_u32(uint32_t *first, size_t count, uint32_t divisor);
int s21_words_mul(const uint32_t *a, const uint32_t *b, uint32_t *out);
int s21_words_divmod(const uint32_t *num, const uint32_t *den, uint32_t *quot,
                     uint32_t *rem);

int s21_unpack(s21_decimal source, s21_big_decimal *out);
int s21_unpack_pair(s21_decimal a, s21_decimal b, s21_big_decimal *left,
                    s21_big_decimal *right);
int s21_align_scales(s21_big_decimal *a, s21_big_decimal *b);
int s21_big_to_dec(s21_big_decimal value, int sticky_tail, s21_decimal *out);
int s21_add_core(s21_decimal first, s21_decimal second, int subtract_b,
                 s21_decimal *result);

#endif
