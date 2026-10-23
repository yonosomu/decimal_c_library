#include <stdint.h>

#include "../s21_big_decimal.h"
#include "check_compat.h"
#include "test_utils.h"

START_TEST(s21_test_mul_fraction) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_mul(s21_test_make(12, 0, 0, 1, 0),
                           s21_test_make(34, 0, 0, 1, 0), &result),
                   0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 408u);
  ck_assert_uint_eq(((uint32_t)result.bits[3] >> 16) & 0xffu, 2u);
}
END_TEST

START_TEST(s21_test_mul_negative) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_mul(s21_test_make(2, 0, 0, 0, 1),
                           s21_test_make(3, 0, 0, 0, 0), &result),
                   0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 6u);
  ck_assert_uint_eq((uint32_t)result.bits[3] >> 31, 1u);
}
END_TEST

START_TEST(s21_test_mul_max_by_half_bankers) {
  s21_decimal result = {{0}};
  s21_decimal max = s21_test_make(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0);
  ck_assert_int_eq(s21_mul(max, s21_test_make(5, 0, 0, 1, 0), &result), 0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 0u);
  ck_assert_uint_eq((uint32_t)result.bits[1], 0u);
  ck_assert_uint_eq((uint32_t)result.bits[2], UINT32_C(0x80000000));
  ck_assert_uint_eq((uint32_t)result.bits[3], 0u);
}
END_TEST

START_TEST(s21_test_mul_overflow) {
  s21_decimal result = {{0}};
  s21_decimal max = s21_test_make(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0);
  ck_assert_int_eq(s21_mul(max, s21_test_make(2, 0, 0, 0, 0), &result), 1);
  ck_assert_int_eq(s21_mul(max, s21_test_make(2, 0, 0, 0, 1), &result), 2);
}
END_TEST

START_TEST(s21_test_mul_smallest_scale) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_mul(s21_test_make(1, 0, 0, 28, 0),
                           s21_test_make(1, 0, 0, 28, 0), &result),
                   0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 0u);
}
END_TEST

START_TEST(s21_test_mul_zero_and_null) {
  s21_decimal result = {{0}};
  s21_decimal zero = s21_test_make(0, 0, 0, 28, 1);
  s21_decimal max = s21_test_make(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0);
  ck_assert_int_eq(s21_mul(zero, max, &result), 0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 0u);
  ck_assert_uint_eq((uint32_t)result.bits[3] >> 31, 0u);
  ck_assert_int_eq(s21_mul(max, max, NULL), 1);
}
END_TEST

START_TEST(s21_test_mul_invalid_metadata) {
  s21_decimal result = {{0}};
  s21_decimal valid = s21_test_make(2, 0, 0, 0, 0);
  s21_decimal invalid = s21_test_make(2, 0, 0, 0, 0);
  invalid.bits[3] = 1;
  ck_assert_int_eq(s21_mul(valid, invalid, &result), 1);
  ck_assert_int_eq(s21_mul(invalid, valid, &result), 1);
}
END_TEST

Suite *s21_suite_mul(void) {
  Suite *suite = suite_create("mul");
  TCase *tests = tcase_create("arithmetics");
  tcase_add_test(tests, s21_test_mul_fraction);
  tcase_add_test(tests, s21_test_mul_negative);
  tcase_add_test(tests, s21_test_mul_max_by_half_bankers);
  tcase_add_test(tests, s21_test_mul_overflow);
  tcase_add_test(tests, s21_test_mul_smallest_scale);
  tcase_add_test(tests, s21_test_mul_zero_and_null);
  tcase_add_test(tests, s21_test_mul_invalid_metadata);
  suite_add_tcase(suite, tests);
  return suite;
}
