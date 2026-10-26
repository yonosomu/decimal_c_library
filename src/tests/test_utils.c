#include "test_utils.h"

#include <string.h>

s21_decimal s21_test_make(uint32_t lo, uint32_t mid, uint32_t hi, int scale,
                          int negative) {
  s21_decimal value = {{0}};
  uint32_t words[4] = {lo, mid, hi,
                       ((uint32_t)scale << 16) | ((uint32_t)negative << 31)};
  memcpy(value.bits, words, sizeof(words));
  return value;
}

void s21_test_assert_decimal(s21_decimal value, uint32_t lo, uint32_t mid,
                             uint32_t hi, int scale, int negative) {
  uint32_t flags = 0;
  memcpy(&flags, &value.bits[3], sizeof(flags));
  ck_assert_uint_eq((uint32_t)value.bits[0], lo);
  ck_assert_uint_eq((uint32_t)value.bits[1], mid);
  ck_assert_uint_eq((uint32_t)value.bits[2], hi);
  ck_assert_int_eq((int)((flags >> 16) & 0xFFu), scale);
  ck_assert_int_eq((int)(flags >> 31), negative);
}
