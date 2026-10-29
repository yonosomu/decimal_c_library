#ifndef S21_TEST_SUITES_H_
#define S21_TEST_SUITES_H_

#include <check.h>

Suite *s21_suite_big_words(void);
Suite *s21_suite_add_sub(void);
Suite *s21_suite_mul(void);
Suite *s21_suite_div(void);
Suite *s21_suite_big_convert(void);
Suite *s21_suite_negate(void);
Suite *s21_suite_truncate(void);
Suite *s21_suite_floor(void);
Suite *s21_suite_round(void);
Suite *comparison_suite(void);
Suite *conversion_int_suite(void);
Suite *conversion_float_suite(void);
Suite *s21_suite_integration(void);

#endif
