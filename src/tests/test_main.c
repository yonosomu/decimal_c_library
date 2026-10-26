#include "check_compat.h"

Suite *s21_suite_big_words(void);
Suite *s21_suite_add_sub(void);
Suite *s21_suite_mul(void);
Suite *s21_suite_div(void);
Suite *s21_suite_big_convert(void);

int main(void) {
  SRunner *runner = srunner_create(s21_suite_big_words());
  srunner_add_suite(runner, s21_suite_add_sub());
  srunner_add_suite(runner, s21_suite_mul());
  srunner_add_suite(runner, s21_suite_div());
  srunner_add_suite(runner, s21_suite_big_convert());
  srunner_run_all(runner, CK_NORMAL);
  int failures = srunner_ntests_failed(runner);
  srunner_free(runner);
  return failures == 0 ? 0 : 1;
}
