#ifndef S21_DECIMAL_H_
#define S21_DECIMAL_H_

#define S21_DECIMAL_WORDS 3
#define S21_MAX_SCALE 28

typedef struct {
  int bits[4];
} s21_decimal;

int s21_add(s21_decimal a, s21_decimal b, s21_decimal *result);
int s21_sub(s21_decimal a, s21_decimal b, s21_decimal *result);
int s21_mul(s21_decimal a, s21_decimal b, s21_decimal *result);
int s21_div(s21_decimal a, s21_decimal b, s21_decimal *result);

#endif
