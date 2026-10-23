#include <stdint.h>

#include "../s21_big_decimal.h"
#include "check_compat.h"
#include "test_utils.h"

START_TEST(test_unpack_pair) {
  s21_decimal a = s21_test_make(125, 0, 0, 2, 0);
  s21_decimal b = s21_test_make(5, 0, 0, 1, 1);
  s21_big_decimal x = {0};
  s21_big_decimal y = {0};
  ck_assert_int_eq(s21_unpack_pair(a, b, &x, &y), 0);
  ck_assert_uint_eq(x.mantissa[0], 125);
  ck_assert_int_eq(x.scale, 2);
  ck_assert_int_eq(y.scale, 1);
  ck_assert_int_eq(y.sign, 1);
  b.bits[3] = 1;
  ck_assert_int_eq(s21_unpack_pair(a, b, &x, &y), 1);
}
END_TEST

START_TEST(test_align_scales) {
  s21_big_decimal a = {.mantissa = {125}, .scale = 2};
  s21_big_decimal b = {.mantissa = {5}, .scale = 1};
  ck_assert_int_eq(s21_align_scales(&a, &b), 0);
  ck_assert_uint_eq(a.mantissa[0], 125);
  ck_assert_uint_eq(b.mantissa[0], 50);
  ck_assert_int_eq(b.scale, 2);
  ck_assert_int_eq(s21_align_scales(NULL, &b), 1);
}
END_TEST

START_TEST(test_round_half_even) {
  s21_big_decimal value = {.mantissa = {1255}, .scale = 3};
  s21_decimal out = {{0}};
  ck_assert_int_eq(s21_big_to_dec(value, 0, &out), 0);
  s21_test_assert_decimal(out, 1255, 0, 0, 3, 0);
  value.mantissa[3] = 1;
  ck_assert_int_eq(s21_big_to_dec(value, 0, &out), 0);
  ck_assert_int_eq(value.scale, 3);
}
END_TEST

START_TEST(test_big_to_dec_overflow) {
  s21_big_decimal value = {.mantissa = {0, 0, 0, 1}, .scale = 0};
  s21_decimal out = {{0}};
  ck_assert_int_eq(s21_big_to_dec(value, 0, &out), 1);
  value.sign = 1;
  ck_assert_int_eq(s21_big_to_dec(value, 0, &out), 2);
}
END_TEST

START_TEST(test_max_scale_rounding) {
  s21_big_decimal value = {.mantissa = {155}, .scale = S21_MAX_SCALE + 1};
  s21_decimal out = {{0}};
  ck_assert_int_eq(s21_big_to_dec(value, 0, &out), 0);
  s21_test_assert_decimal(out, 16, 0, 0, S21_MAX_SCALE, 0);
  value.scale = 2 * S21_MAX_SCALE + 1;
  ck_assert_int_eq(s21_big_to_dec(value, 0, &out), 1);
}
END_TEST

Suite *s21_suite_big_convert(void) {
  Suite *suite = suite_create("big_convert");
  TCase *tests = tcase_create("big_convert");
  tcase_add_test(tests, test_unpack_pair);
  tcase_add_test(tests, test_align_scales);
  tcase_add_test(tests, test_round_half_even);
  tcase_add_test(tests, test_big_to_dec_overflow);
  tcase_add_test(tests, test_max_scale_rounding);
  suite_add_tcase(suite, tests);
  return suite;
}
