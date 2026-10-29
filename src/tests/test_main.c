#include "test_suites.h"

int main(void) {
  SRunner *runner = srunner_create(s21_suite_big_words());
  srunner_add_suite(runner, s21_suite_add_sub());
  srunner_add_suite(runner, s21_suite_mul());
  srunner_add_suite(runner, s21_suite_div());
  srunner_add_suite(runner, s21_suite_big_convert());
  srunner_add_suite(runner, s21_suite_negate());
  srunner_add_suite(runner, s21_suite_truncate());
  srunner_add_suite(runner, s21_suite_floor());
  srunner_add_suite(runner, s21_suite_round());
  srunner_add_suite(runner, comparison_suite());
  srunner_add_suite(runner, conversion_int_suite());
  srunner_add_suite(runner, conversion_float_suite());
  srunner_add_suite(runner, s21_suite_integration());
  srunner_run_all(runner, CK_NORMAL);
  int failures = srunner_ntests_failed(runner);
  srunner_free(runner);
  return failures == 0 ? 0 : 1;
}
