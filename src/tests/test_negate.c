#include <string.h>

#include "test_other_utils.h"
#include "test_suites.h"

START_TEST(s21_test_negate_preserves_representation) {
  for (int sign = 0; sign <= 1; ++sign) {
    for (int scale = 0; scale <= 28; ++scale) {
      s21_decimal value =
          s21_test_make(UINT32_MAX, 0x87654321u, UINT32_MAX, scale, sign);
      s21_decimal separate = {{1, 2, 3, 4}};
      ck_assert_int_eq(s21_negate(value, &separate), 0);
      ck_assert_int_eq(s21_negate(value, &value), 0);
      ck_assert_mem_eq(&value, &separate, sizeof(value));
      s21_test_assert_decimal(value, UINT32_MAX, 0x87654321u, UINT32_MAX, scale,
                              !sign);
      value = s21_test_make(0, 0, 0, scale, sign);
      ck_assert_int_eq(s21_negate(value, &value), 0);
      s21_test_assert_decimal(value, 0, 0, 0, scale, !sign);
    }
  }
}
END_TEST

START_TEST(s21_test_negate_rejects_invalid) {
  s21_test_unary_invalid(s21_negate);
}
END_TEST

Suite *s21_suite_negate(void) {
  Suite *suite = suite_create("negate");
  TCase *tests = tcase_create("representation");
  tcase_add_test(tests, s21_test_negate_preserves_representation);
  tcase_add_test(tests, s21_test_negate_rejects_invalid);
  suite_add_tcase(suite, tests);
  return suite;
}
