#include <check.h>
#include <math.h>
#include <stdint.h>

#include "../s21_decimal.h"

START_TEST(float_to_dec_special) {
  s21_decimal dst;
  ck_assert_int_eq(s21_from_float_to_decimal(NAN, &dst), 1);
  ck_assert_int_eq(s21_from_float_to_decimal(INFINITY, &dst), 1);
  ck_assert_int_eq(s21_from_float_to_decimal(-INFINITY, &dst), 1);
}
END_TEST

START_TEST(float_to_dec_roundtrip) {
  s21_decimal dst;
  float src = 123.456f;
  ck_assert_int_eq(s21_from_float_to_decimal(src, &dst), 0);

  float res;
  ck_assert_int_eq(s21_from_decimal_to_float(dst, &res), 0);

  ck_assert_float_eq_tol(src, res, 1e-5);
}
END_TEST

START_TEST(float_to_dec_small) {
  s21_decimal dst;
  ck_assert_int_eq(s21_from_float_to_decimal(1e-29f, &dst), 1);
  ck_assert_int_eq(dst.bits[0], 0);
  ck_assert_int_eq(s21_from_float_to_decimal(-0.0f, &dst), 0);
  ck_assert_uint_eq((uint32_t)dst.bits[3], 1u << 31);
}
END_TEST

START_TEST(float_to_dec_round_away) {
  s21_decimal dst;
  ck_assert_int_eq(s21_from_float_to_decimal(12345675.0f, &dst), 0);
  ck_assert_int_eq(dst.bits[0], 12345680);
  ck_assert_int_eq((dst.bits[3] >> 16) & 0xFF, 0);
  ck_assert_int_eq(s21_from_float_to_decimal(-12345675.0f, &dst), 0);
  ck_assert_int_eq(dst.bits[0], 12345680);
  ck_assert_uint_eq((uint32_t)dst.bits[3] >> 31, 1u);
}
END_TEST

Suite* conversion_float_suite(void) {
  Suite* s = suite_create("conversion_float");
  TCase* tc = tcase_create("Core");
  tcase_add_test(tc, float_to_dec_special);
  tcase_add_test(tc, float_to_dec_roundtrip);
  tcase_add_test(tc, float_to_dec_small);
  tcase_add_test(tc, float_to_dec_round_away);
  suite_add_tcase(s, tc);
  return s;
}