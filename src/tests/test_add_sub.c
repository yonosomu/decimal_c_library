#include <stdint.h>

#include "../s21_big_decimal.h"
#include "check_compat.h"
#include "test_utils.h"

START_TEST(s21_test_add_different_scales) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_add(s21_test_make(123, 0, 0, 2, 0),
                           s21_test_make(45, 0, 0, 1, 0), &result),
                   0);
  s21_test_assert_decimal(result, 573, 0, 0, 2, 0);
}
END_TEST

START_TEST(s21_test_add_mixed_signs) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_add(s21_test_make(12, 0, 0, 0, 1),
                           s21_test_make(5, 0, 0, 0, 0), &result),
                   0);
  s21_test_assert_decimal(result, 7, 0, 0, 0, 1);
  ck_assert_int_eq(s21_add(s21_test_make(5, 0, 0, 0, 0),
                           s21_test_make(12, 0, 0, 0, 1), &result),
                   0);
  s21_test_assert_decimal(result, 7, 0, 0, 0, 1);
}
END_TEST

START_TEST(s21_test_sub_and_zero) {
  s21_decimal result = {{0}};
  s21_decimal five = s21_test_make(5, 0, 0, 0, 0);
  ck_assert_int_eq(s21_sub(five, five, &result), 0);
  s21_test_assert_decimal(result, 0, 0, 0, 0, 0);
  ck_assert_int_eq(s21_sub(five, s21_test_make(7, 0, 0, 0, 0), &result), 0);
  s21_test_assert_decimal(result, 2, 0, 0, 0, 1);
}
END_TEST

START_TEST(s21_test_add_across_32bit_boundary) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_add(s21_test_make(UINT32_MAX, 0, 0, 0, 0),
                           s21_test_make(1, 0, 0, 0, 0), &result),
                   0);
  s21_test_assert_decimal(result, 0, 1, 0, 0, 0);
}
END_TEST

START_TEST(s21_test_add_overflow_signs) {
  s21_decimal result = {{0}};
  s21_decimal max = s21_test_make(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0);
  ck_assert_int_eq(s21_add(max, s21_test_make(1, 0, 0, 0, 0), &result), 1);
  max.bits[3] = (int)UINT32_C(0x80000000);
  ck_assert_int_eq(s21_add(max, s21_test_make(1, 0, 0, 0, 1), &result), 2);
}
END_TEST

START_TEST(s21_test_sub_max_minus_six_tenths) {
  s21_decimal result = {{0}};
  s21_decimal max = s21_test_make(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0);
  ck_assert_int_eq(s21_sub(max, s21_test_make(6, 0, 0, 1, 0), &result), 0);
  s21_test_assert_decimal(result, UINT32_MAX - 1u, UINT32_MAX, UINT32_MAX, 0,
                          0);
}
END_TEST

START_TEST(s21_test_add_valid_scale_mixed_zero) {
  s21_decimal output = {{0}};
  ck_assert_int_eq(s21_add(s21_test_make(0, 0, 0, 6, 1),
                           s21_test_make(2, 0, 0, 1, 0), &output),
                   0);
  s21_test_assert_decimal(output, 200000, 0, 0, 6, 0);
  ck_assert_int_eq(s21_sub(s21_test_make(1, 0, 0, 1, 0),
                           s21_test_make(2, 0, 0, 1, 0), &output),
                   0);
  s21_test_assert_decimal(output, 1, 0, 0, 1, 1);
}
END_TEST

START_TEST(s21_test_invalid_metadata_and_null_output) {
  s21_decimal result = {{0}};
  s21_decimal a = s21_test_make(1, 0, 0, 0, 0);
  s21_decimal bad_scale = s21_test_make(1, 0, 0, 29, 0);
  s21_decimal bad_flags = s21_test_make(1, 0, 0, 0, 0);
  bad_flags.bits[3] = (int)(1u << 24);
  ck_assert_int_eq(s21_add(a, bad_scale, &result), 1);
  ck_assert_int_eq(s21_sub(bad_flags, a, &result), 1);
  ck_assert_int_eq(s21_add(a, a, NULL), 1);
}
END_TEST

START_TEST(s21_test_round_half_even_on_add) {
  s21_decimal result = {{0}};
  s21_decimal max = s21_test_make(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0);
  ck_assert_int_eq(s21_sub(max, s21_test_make(5, 0, 0, 1, 0), &result), 0);
  s21_test_assert_decimal(result, UINT32_MAX - 1u, UINT32_MAX, UINT32_MAX, 0,
                          0);
  ck_assert_int_eq(s21_add(s21_test_make(15, 0, 0, 1, 1),
                           s21_test_make(15, 0, 0, 1, 0), &result),
                   0);
  s21_test_assert_decimal(result, 0, 0, 0, 1, 0);
}
END_TEST

Suite *s21_suite_add_sub(void) {
  Suite *suite = suite_create("add_sub");
  TCase *tests = tcase_create("arithmetics");
  tcase_add_test(tests, s21_test_add_different_scales);
  tcase_add_test(tests, s21_test_add_mixed_signs);
  tcase_add_test(tests, s21_test_sub_and_zero);
  tcase_add_test(tests, s21_test_add_across_32bit_boundary);
  tcase_add_test(tests, s21_test_add_overflow_signs);
  tcase_add_test(tests, s21_test_sub_max_minus_six_tenths);
  tcase_add_test(tests, s21_test_add_valid_scale_mixed_zero);
  tcase_add_test(tests, s21_test_invalid_metadata_and_null_output);
  tcase_add_test(tests, s21_test_round_half_even_on_add);
  suite_add_tcase(suite, tests);
  return suite;
}
