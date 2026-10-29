#include <limits.h>
#include <math.h>

#include "test_suites.h"
#include "test_utils.h"

START_TEST(s21_test_comparison_matrix) {
  const s21_decimal values[] = {
      s21_test_make(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 1),
      s21_test_make(25, 0, 0, 1, 1),
      s21_test_make(1, 0, 0, 28, 1),
      s21_test_make(0, 0, 0, 28, 1),
      s21_test_make(0, 0, 0, 0, 0),
      s21_test_make(1, 0, 0, 28, 0),
      s21_test_make(25, 0, 0, 1, 0),
      s21_test_make(2500, 0, 0, 3, 0),
      s21_test_make(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, 0)};
  const int ranks[] = {0, 1, 2, 3, 3, 4, 5, 5, 6};
  const size_t count = sizeof(values) / sizeof(*values);
  for (size_t i = 0; i < count; ++i) {
    for (size_t j = 0; j < count; ++j) {
      ck_assert_int_eq(s21_is_less(values[i], values[j]), ranks[i] < ranks[j]);
      ck_assert_int_eq(s21_is_less_or_equal(values[i], values[j]),
                       ranks[i] <= ranks[j]);
      ck_assert_int_eq(s21_is_greater(values[i], values[j]),
                       ranks[i] > ranks[j]);
      ck_assert_int_eq(s21_is_greater_or_equal(values[i], values[j]),
                       ranks[i] >= ranks[j]);
      ck_assert_int_eq(s21_is_equal(values[i], values[j]),
                       ranks[i] == ranks[j]);
      ck_assert_int_eq(s21_is_not_equal(values[i], values[j]),
                       ranks[i] != ranks[j]);
    }
  }
}
END_TEST

START_TEST(s21_test_int_boundaries) {
  const int values[] = {0, 1, -1, INT_MAX, INT_MIN, 123456789, -123456789};
  for (size_t i = 0; i < sizeof(values) / sizeof(*values); ++i) {
    s21_decimal decimal;
    int result = 0;
    ck_assert_int_eq(s21_from_int_to_decimal(values[i], &decimal), 0);
    ck_assert_int_eq(s21_from_decimal_to_int(decimal, &result), 0);
    ck_assert_int_eq(result, values[i]);
  }
  int result = 0;
  ck_assert_int_eq(
      s21_from_decimal_to_int(s21_test_make(1299, 0, 0, 2, 1), &result), 0);
  ck_assert_int_eq(result, -12);
  ck_assert_int_eq(
      s21_from_decimal_to_int(s21_test_make(99, 0, 0, 2, 1), &result), 0);
  ck_assert_int_eq(result, 0);
  ck_assert_int_eq(
      s21_from_decimal_to_int(s21_test_make(UINT32_C(0x80000000), 0, 0, 0, 0),
                              &result),
      1);
  ck_assert_int_eq(
      s21_from_decimal_to_int(s21_test_make(UINT32_C(0x80000001), 0, 0, 0, 1),
                              &result),
      1);
  ck_assert_int_eq(s21_from_int_to_decimal(1, NULL), 1);
  ck_assert_int_eq(s21_from_decimal_to_int(s21_test_make(1, 0, 0, 0, 0), NULL),
                   1);
}
END_TEST

START_TEST(s21_test_float_bankers_ties) {
  const float values[] = {12345665.0f, -12345665.0f, 12345675.0f, -12345675.0f};
  const uint32_t expected[] = {12345660, 12345660, 12345680, 12345680};
  for (size_t i = 0; i < sizeof(values) / sizeof(*values); ++i) {
    s21_decimal decimal;
    ck_assert_int_eq(s21_from_float_to_decimal(values[i], &decimal), 0);
    s21_test_assert_decimal(decimal, expected[i], 0, 0, 0, values[i] < 0);
  }
}
END_TEST

START_TEST(s21_test_float_ranges) {
  const float values[] = {0.0f,
                          -0.0f,
                          1.0f,
                          -1.0f,
                          1.25f,
                          -1.25f,
                          1e-28f,
                          1.45e-28f,
                          1e-20f,
                          1e20f,
                          9999999.0f,
                          9999999.5f,
                          nextafterf(ldexpf(1.0f, 96), 0.0f)};
  for (size_t i = 0; i < sizeof(values) / sizeof(*values); ++i) {
    s21_decimal decimal;
    float result = 0;
    ck_assert_int_eq(s21_from_float_to_decimal(values[i], &decimal), 0);
    ck_assert_int_eq(s21_from_decimal_to_float(decimal, &result), 0);
    if (fabsf(values[i]) >= 1e-27f) {
      ck_assert_float_eq_tol(result, values[i], fabsf(values[i]) * 1e-6f);
    }
    ck_assert_uint_le(((uint32_t)decimal.bits[3] >> 16) & 255u, S21_MAX_SCALE);
    if (values[i] != 0.0f && fabsf(values[i]) < 1e-27f) {
      s21_test_assert_decimal(decimal, 1, 0, 0, 28, 0);
    }
  }
  s21_decimal decimal;
  const float invalid[] = {NAN,     INFINITY,         -INFINITY,         1e-29f,
                           -1e-29f, ldexpf(1.0f, 96), -ldexpf(1.0f, 96), 1e30f};
  for (size_t i = 0; i < sizeof(invalid) / sizeof(*invalid); ++i) {
    ck_assert_int_eq(s21_from_float_to_decimal(invalid[i], &decimal), 1);
    s21_test_assert_decimal(decimal, 0, 0, 0, 0, 0);
  }
  ck_assert_int_eq(s21_from_float_to_decimal(1.0f, NULL), 1);
  ck_assert_int_eq(
      s21_from_decimal_to_float(s21_test_make(1, 0, 0, 0, 0), NULL), 1);
}
END_TEST

START_TEST(s21_test_conversion_invalid_metadata) {
  for (int scale = 29; scale <= 255; ++scale) {
    s21_decimal invalid = s21_test_make(1, 0, 0, scale, 0);
    int integer = 0;
    float floating = 0;
    ck_assert_int_eq(s21_from_decimal_to_int(invalid, &integer), 1);
    ck_assert_int_eq(s21_from_decimal_to_float(invalid, &floating), 1);
  }
  s21_decimal invalid = {{1, 0, 0, 1}};
  int integer = 0;
  float floating = 0;
  ck_assert_int_eq(s21_from_decimal_to_int(invalid, &integer), 1);
  ck_assert_int_eq(s21_from_decimal_to_float(invalid, &floating), 1);
}
END_TEST

Suite *s21_suite_integration(void) {
  Suite *suite = suite_create("integration");
  TCase *tests = tcase_create("cross_module");
  tcase_add_test(tests, s21_test_comparison_matrix);
  tcase_add_test(tests, s21_test_int_boundaries);
  tcase_add_test(tests, s21_test_float_bankers_ties);
  tcase_add_test(tests, s21_test_float_ranges);
  tcase_add_test(tests, s21_test_conversion_invalid_metadata);
  suite_add_tcase(suite, tests);
  return suite;
}
