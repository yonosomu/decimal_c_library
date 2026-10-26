#include "test_other_utils.h"

#include <string.h>

void s21_test_unary_invalid(s21_test_unary function) {
  s21_decimal value = s21_test_make(1, 2, 3, 0, 0);
  ck_assert_int_eq(function(value, NULL), 1);
  for (int sign = 0; sign <= 1; ++sign) {
    for (int bit = 0; bit < 31; ++bit) {
      if (bit < 16 || bit >= 24) {
        uint32_t flags = (UINT32_C(1) << bit) | ((uint32_t)sign << 31);
        memcpy(&value.bits[3], &flags, sizeof(flags));
        s21_decimal result = {{1, 2, 3, 4}};
        ck_assert_int_eq(function(value, &result), 1);
        ck_assert_mem_eq(&result, &(s21_decimal){{0}}, sizeof(result));
      }
    }
    for (int scale = 29; scale <= 255; ++scale) {
      value = s21_test_make(1, 2, 3, scale, sign);
      ck_assert_int_eq(function(value, &value), 1);
      ck_assert_mem_eq(&value, &(s21_decimal){{0}}, sizeof(value));
    }
  }
}
