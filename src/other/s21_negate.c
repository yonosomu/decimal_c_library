#include <string.h>

#include "../s21_big_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  if (result == NULL) {
    return 1;
  }
  memset(result, 0, sizeof(*result));
  s21_big_decimal unpacked = {0};
  int status = s21_unpack(value, &unpacked);
  if (status == 0) {
    uint32_t flags = 0;
    *result = value;
    memcpy(&flags, &value.bits[3], sizeof(flags));
    flags ^= UINT32_C(0x80000000);
    memcpy(&result->bits[3], &flags, sizeof(flags));
  }
  return status;
}
