#include <check.h>
#include <limits.h>
#include <stdint.h>

#include "../s21_decimal.h"

START_TEST(int_to_dec_zero) {
  s21_decimal dst;
  ck_assert_int_eq(s21_from_int_to_decimal(0, &dst), 0);
  ck_assert_int_eq(dst.bits[0], 0);
  ck_assert_int_eq(dst.bits[3], 0);
}
END_TEST

START_TEST(int_to_dec_min_max) {
  s21_decimal dst;
  ck_assert_int_eq(s21_from_int_to_decimal(INT_MAX, &dst), 0);
  ck_assert_uint_eq((uint32_t)dst.bits[0], (uint32_t)INT_MAX);

  ck_assert_int_eq(s21_from_int_to_decimal(INT_MIN, &dst), 0);
  ck_assert_uint_eq((uint32_t)dst.bits[0], (uint32_t)INT_MAX + 1u);
  ck_assert_uint_eq((uint32_t)dst.bits[3] >> 31, 1u);
}
END_TEST

START_TEST(dec_to_int_overflow) {
  s21_decimal src;
  s21_from_int_to_decimal(INT_MAX, &src);
  src.bits[1] = 1;
  int dst;
  ck_assert_int_eq(s21_from_decimal_to_int(src, &dst), 1);
}
END_TEST

Suite* conversion_int_suite(void) {
  Suite* s = suite_create("conversion_int");
  TCase* tc = tcase_create("Core");
  tcase_add_test(tc, int_to_dec_zero);
  tcase_add_test(tc, int_to_dec_min_max);
  tcase_add_test(tc, dec_to_int_overflow);
  suite_add_tcase(s, tc);
  return s;
}
