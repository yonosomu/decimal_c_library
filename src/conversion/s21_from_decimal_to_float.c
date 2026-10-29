#include <math.h>
#include <stdint.h>

#include "../s21_big_decimal.h"
#include "../s21_decimal.h"

int s21_from_decimal_to_float(s21_decimal src, float* dst) {
  int result = 0;

  if (dst == ((void*)0)) {
    result = 1;
  } else {
    s21_big_decimal unpacked = {0};
    if (s21_unpack(src, &unpacked) != 0) {
      return 1;
    }
    double val = 0.0;
    for (int i = 2; i >= 0; i--) {
      val = val * 4294967296.0 + (uint32_t)src.bits[i];
    }

    int scale = ((uint32_t)src.bits[3] >> 16) & 0xFF;
    for (int i = 0; i < scale; i++) {
      val /= 10.0;
    }

    if ((uint32_t)src.bits[3] >> 31) {
      val = -val;
    }
    *dst = (float)val;
  }

  return result;
}
