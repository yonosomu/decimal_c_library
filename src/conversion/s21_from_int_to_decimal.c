#include <stdint.h>

#include "../s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal* dst) {
  int result = 0;

  if (dst == ((void*)0)) {
    result = 1;
  } else {
    dst->bits[0] = 0;
    dst->bits[1] = 0;
    dst->bits[2] = 0;
    dst->bits[3] = 0;

    uint32_t abs_val;
    if (src < 0) {
      abs_val = (uint32_t)(-(src + 1)) + 1u;
    } else {
      abs_val = (uint32_t)src;
    }

    dst->bits[0] = (int)abs_val;

    if (src < 0) {
      dst->bits[3] |= (1u << 31);
    }
  }

  return result;
}
