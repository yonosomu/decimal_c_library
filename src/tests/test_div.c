#include <check.h>
#include <stdint.h>

#include "../s21_big_decimal.h"
#include "test_utils.h"

START_TEST(s21_test_div_quarter) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_div(s21_test_make(1, 0, 0, 0, 0),
                           s21_test_make(4, 0, 0, 0, 0), &result),
                   0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 25u);
  ck_assert_uint_eq(((uint32_t)result.bits[3] >> 16) & 0xffu, 2u);
}
END_TEST

START_TEST(s21_test_div_fractional_input) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_div(s21_test_make(125, 0, 0, 2, 0),
                           s21_test_make(5, 0, 0, 1, 0), &result),
                   0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 25u);
  ck_assert_uint_eq(((uint32_t)result.bits[3] >> 16) & 0xffu, 1u);
}
END_TEST

START_TEST(s21_test_div_negative) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_div(s21_test_make(1, 0, 0, 0, 1),
                           s21_test_make(2, 0, 0, 0, 0), &result),
                   0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 5u);
  ck_assert_uint_eq((uint32_t)result.bits[3] >> 31, 1u);
}
END_TEST

START_TEST(s21_test_div_zero_error) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_div(s21_test_make(1, 0, 0, 0, 0),
                           s21_test_make(0, 0, 0, 0, 0), &result),
                   3);
  ck_assert_int_eq(s21_div(s21_test_make(0, 0, 0, 0, 0),
                           s21_test_make(7, 0, 0, 0, 0), &result),
                   0);
}
END_TEST

START_TEST(s21_test_div_half_even_ties) {
  s21_decimal result = {{0}};

  ck_assert_int_eq(s21_div(s21_test_make(3, 0, 0, 28, 0),
                           s21_test_make(2, 0, 0, 0, 0), &result),
                   0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 2u);
  ck_assert_uint_eq(((uint32_t)result.bits[3] >> 16) & 0xffu, 28u);
  ck_assert_int_eq(s21_div(s21_test_make(1, 0, 0, 28, 0),
                           s21_test_make(2, 0, 0, 0, 0), &result),
                   0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 0u);
}
END_TEST

START_TEST(s21_test_div_repeating_one_third) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_div(s21_test_make(1, 0, 0, 0, 0),
                           s21_test_make(3, 0, 0, 0, 0), &result),
                   0);

  s21_big_decimal digits = {0};
  for (int i = 0; i < 28; ++i) {
    ck_assert_int_eq(s21_words_mul_u32(digits.mantissa, S21_BIG_WORDS, 10), 0);
    s21_big_decimal three = {.mantissa = {3}}, next = {0};
    ck_assert_int_eq(s21_words_add(digits.mantissa, three.mantissa,
                                   next.mantissa, S21_BIG_WORDS),
                     0);
    digits = next;
  }
  ck_assert_uint_eq((uint32_t)result.bits[0], digits.mantissa[0]);
  ck_assert_uint_eq((uint32_t)result.bits[1], digits.mantissa[1]);
  ck_assert_uint_eq((uint32_t)result.bits[2], digits.mantissa[2]);
  ck_assert_uint_eq(((uint32_t)result.bits[3] >> 16) & 0xffu, 28u);
}
END_TEST

START_TEST(s21_test_div_large_and_small) {
  s21_decimal result = {{0}};
  s21_decimal max = s21_test_make(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0);
  ck_assert_int_eq(s21_div(max, s21_test_make(1, 0, 0, 28, 0), &result), 1);
  ck_assert_int_eq(s21_div(s21_test_make(1, 0, 0, 28, 0), max, &result), 0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 0u);
  ck_assert_int_eq(s21_div(s21_test_make(1, 0, 0, 28, 1),
                           s21_test_make(1, 0, 0, 28, 0), &result),
                   0);
  ck_assert_uint_eq((uint32_t)result.bits[3] >> 31, 1u);
}
END_TEST

START_TEST(s21_test_div_invalid_input_and_null) {
  s21_decimal result = {{0}};
  s21_decimal one = s21_test_make(1, 0, 0, 0, 0);
  s21_decimal bad = one;
  bad.bits[3] = 1;
  ck_assert_int_eq(s21_div(bad, one, &result), 1);
  ck_assert_int_eq(s21_div(one, bad, &result), 1);
  ck_assert_int_eq(s21_div(one, one, NULL), 1);
}
END_TEST

START_TEST(s21_test_div_rounding_sticky) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_div(s21_test_make(1, 0, 0, 28, 0),
                           s21_test_make(3, 0, 0, 0, 0), &result),
                   0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 0u);
  ck_assert_int_eq(s21_div(s21_test_make(2, 0, 0, 28, 0),
                           s21_test_make(3, 0, 0, 0, 0), &result),
                   0);
  ck_assert_uint_eq((uint32_t)result.bits[0], 1u);
}
END_TEST

START_TEST(s21_test_div_guard_digit) {
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_div(s21_test_make(1, 0, 0, 0, 0),
                           s21_test_make(6, 0, 0, 0, 0), &result),
                   0);
  s21_test_assert_decimal(result, UINT32_C(2192222891), UINT32_C(173768805),
                          UINT32_C(90350181), S21_MAX_SCALE, 0);
}
END_TEST

Suite *s21_suite_div(void) {
  Suite *suite = suite_create("div");
  TCase *tests = tcase_create("arithmetics");
  tcase_add_test(tests, s21_test_div_quarter);
  tcase_add_test(tests, s21_test_div_fractional_input);
  tcase_add_test(tests, s21_test_div_negative);
  tcase_add_test(tests, s21_test_div_zero_error);
  tcase_add_test(tests, s21_test_div_half_even_ties);
  tcase_add_test(tests, s21_test_div_repeating_one_third);
  tcase_add_test(tests, s21_test_div_large_and_small);
  tcase_add_test(tests, s21_test_div_invalid_input_and_null);
  tcase_add_test(tests, s21_test_div_rounding_sticky);
  tcase_add_test(tests, s21_test_div_guard_digit);
  suite_add_tcase(suite, tests);
  return suite;
}
