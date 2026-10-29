#include <check.h>
#include <stdint.h>
#include <string.h>

#include "../s21_big_decimal.h"

START_TEST(test_pointer_contiguous_memory) {
  s21_big_decimal x = {.mantissa = {1, 2, 3}, .scale = 7, .sign = 1};
  uint32_t *first = x.mantissa;
  ck_assert_ptr_eq(first + 1, &x.mantissa[1]);
  ck_assert_int_eq((int)((char *)(first + S21_BIG_WORDS) - (char *)first),
                   S21_BIG_WORDS * (int)sizeof(uint32_t));
  s21_words_zero(first + 1, 2);
  ck_assert_uint_eq(x.mantissa[0], 1);
  ck_assert_uint_eq(x.mantissa[1], 0);
  ck_assert_uint_eq(x.mantissa[2], 0);
  ck_assert_int_eq(x.scale, 7);
  ck_assert_int_eq(x.sign, 1);
}
END_TEST

START_TEST(test_pointer_bits_and_shift) {
  s21_big_decimal x = {.scale = 4, .sign = 1};
  s21_words_set_bit(x.mantissa, S21_BIG_WORDS, 31, 1);
  ck_assert_int_eq(s21_words_get_bit(x.mantissa, S21_BIG_WORDS, 31), 1);
  ck_assert_uint_eq(s21_words_shl(x.mantissa, S21_BIG_WORDS), 0);
  ck_assert_int_eq(s21_words_get_bit(x.mantissa, S21_BIG_WORDS, 32), 1);
  s21_words_set_bit(x.mantissa, S21_BIG_WORDS, 32, 0);
  ck_assert_int_eq(s21_words_is_zero(x.mantissa, S21_BIG_WORDS), 1);
  ck_assert_int_eq(s21_words_get_bit(x.mantissa, S21_BIG_WORDS, 256), 0);
  ck_assert_int_eq(x.scale, 4);
  ck_assert_int_eq(x.sign, 1);
}
END_TEST

START_TEST(test_pointer_add_carry_and_alias) {
  uint32_t a[8] = {UINT32_MAX, UINT32_MAX};
  uint32_t const b[8] = {1};
  ck_assert_uint_eq(s21_words_add(a, b, a, S21_BIG_WORDS), 0);
  ck_assert_uint_eq(a[0], 0);
  ck_assert_uint_eq(a[1], 0);
  ck_assert_uint_eq(a[2], 1);
}
END_TEST

START_TEST(test_pointer_sub_borrow_and_alias) {
  uint32_t a[8] = {0, 0, 1};
  uint32_t b[8] = {1};
  ck_assert_uint_eq(s21_words_sub(a, b, a, S21_BIG_WORDS), 0);
  ck_assert_uint_eq(a[0], UINT32_MAX);
  ck_assert_uint_eq(a[1], UINT32_MAX);
  ck_assert_uint_eq(a[2], 0);
  ck_assert_uint_eq(s21_words_sub(b, a, b, S21_BIG_WORDS), 1);
}
END_TEST

START_TEST(test_pointer_compare_fit_overflow) {
  uint32_t a[8] = {1};
  uint32_t const b[8] = {2};
  ck_assert_int_eq(s21_words_cmp(a, b, S21_BIG_WORDS), -1);
  ck_assert_int_eq(s21_words_cmp(b, a, S21_BIG_WORDS), 1);
  ck_assert_int_eq(s21_words_cmp(a, a, S21_BIG_WORDS), 0);
  ck_assert_int_eq(s21_words_fits(a, S21_BIG_WORDS, 3), 1);
  s21_words_set_bit(a, S21_BIG_WORDS, 96, 1);
  ck_assert_int_eq(s21_words_fits(a, S21_BIG_WORDS, 3), 0);
  ck_assert_int_eq(s21_words_fits(a, S21_BIG_WORDS, 9), 0);
  ck_assert_int_eq(s21_words_fits(a, S21_BIG_WORDS, 8), 1);
}
END_TEST

START_TEST(test_pointer_mul_small_and_div_small) {
  uint32_t a[8] = {123456789};
  ck_assert_uint_eq(s21_words_mul_u32(a, S21_BIG_WORDS, 100), 0);
  ck_assert_uint_eq(a[0], (uint32_t)12345678900ULL);
  ck_assert_uint_eq(a[1], (uint32_t)(12345678900ULL >> 32));
  ck_assert_uint_eq(s21_words_div_u32(a, S21_BIG_WORDS, 10), 0);
  ck_assert_uint_eq(a[0], 1234567890u);
  a[0] = 1234;
  s21_words_zero(a + 1, 7);
  ck_assert_uint_eq(s21_words_div_u32(a, S21_BIG_WORDS, 10), 4);
  ck_assert_uint_eq(a[0], 123u);
}
END_TEST

START_TEST(test_pointer_long_multiplication) {
  uint32_t a[8] = {0, UINT32_MAX};
  uint32_t b[8] = {0, 2};
  uint32_t product[8] = {0};
  ck_assert_int_eq(s21_words_mul(a, b, product), 0);
  ck_assert_uint_eq(product[2], UINT32_MAX - 1u);
  ck_assert_uint_eq(product[3], 1u);
  a[7] = UINT32_C(0x80000000);
  b[0] = 2;
  ck_assert_int_eq(s21_words_mul(a, b, product), 1);
}
END_TEST

START_TEST(test_pointer_divmod_and_high_overflow) {
  uint32_t num[8] = {0};
  uint32_t den[8] = {3};
  uint32_t q[8] = {0};
  uint32_t r[8] = {0};
  num[2] = 1;
  ck_assert_int_eq(s21_words_divmod(num, den, q, r), 0);
  ck_assert_uint_eq(q[0], UINT32_C(0x55555555));
  ck_assert_uint_eq(q[1], UINT32_C(0x55555555));
  ck_assert_uint_eq(r[0], 1);
  ck_assert_int_eq(s21_words_divmod(num, (uint32_t[8]){0}, q, r), 1);
  num[7] = UINT32_MAX;
  den[7] = UINT32_C(0x80000000);
  ck_assert_int_eq(s21_words_divmod(num, den, q, r), 0);
}
END_TEST

START_TEST(test_pointer_all_ones) {
  uint32_t full[8] = {0};
  uint32_t const one[8] = {1};
  uint32_t out[8] = {0};
  uint32_t *cursor = full;
  while (cursor != full + S21_BIG_WORDS) {
    *cursor = UINT32_MAX;
    ++cursor;
  }
  ck_assert_uint_eq(s21_words_add(full, one, out, 8), 1);
  ck_assert_int_eq(s21_words_is_zero(out, 8), 1);
  ck_assert_uint_eq(s21_words_mul_u32(full, 8, 10), 9u);
}
END_TEST

START_TEST(test_big_metadata) {
  s21_decimal x = {{12345, 0, 0, 0}};
  uint32_t flags = (2u << 16) | UINT32_C(0x80000000);
  memcpy(&x.bits[3], &flags, sizeof(flags));
  s21_big_decimal out = {0};
  ck_assert_int_eq(s21_unpack(x, &out), 0);
  ck_assert_uint_eq(out.mantissa[0], 12345);
  ck_assert_int_eq(out.scale, 2);
  ck_assert_int_eq(out.sign, 1);
}
END_TEST

Suite *s21_suite_big_words(void) {
  Suite *suite = suite_create("big_words_pointers");
  TCase *tests = tcase_create("words");
  tcase_add_test(tests, test_pointer_contiguous_memory);
  tcase_add_test(tests, test_pointer_bits_and_shift);
  tcase_add_test(tests, test_pointer_add_carry_and_alias);
  tcase_add_test(tests, test_pointer_sub_borrow_and_alias);
  tcase_add_test(tests, test_pointer_compare_fit_overflow);
  tcase_add_test(tests, test_pointer_mul_small_and_div_small);
  tcase_add_test(tests, test_pointer_long_multiplication);
  tcase_add_test(tests, test_pointer_divmod_and_high_overflow);
  tcase_add_test(tests, test_pointer_all_ones);
  tcase_add_test(tests, test_big_metadata);
  suite_add_tcase(suite, tests);
  return suite;
}
