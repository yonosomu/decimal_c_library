#include <check.h>

#include "../s21_decimal.h"

START_TEST(cmp_equal_zeros) {
  s21_decimal a = {0}, b = {0};
  b.bits[3] = (1u << 31);
  ck_assert_int_eq(s21_is_equal(a, b), 1);
}
END_TEST

START_TEST(cmp_diff_scales) {
  s21_decimal a, b;
  s21_from_float_to_decimal(1.50f, &a);
  s21_from_float_to_decimal(1.5f, &b);
  ck_assert_int_eq(s21_is_equal(a, b), 1);
}
END_TEST

START_TEST(cmp_less_negative) {
  s21_decimal a, b;
  s21_from_int_to_decimal(-10, &a);
  s21_from_int_to_decimal(-5, &b);
  ck_assert_int_eq(s21_is_less(a, b), 1);
  ck_assert_int_eq(s21_is_greater(a, b), 0);
}
END_TEST

Suite* comparison_suite(void) {
  Suite* s = suite_create("comparison");
  TCase* tc = tcase_create("Core");
  tcase_add_test(tc, cmp_equal_zeros);
  tcase_add_test(tc, cmp_diff_scales);
  tcase_add_test(tc, cmp_less_negative);
  suite_add_tcase(s, tc);
  return s;
}