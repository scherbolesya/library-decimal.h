#include "./s21_decimal.h"

int s21_is_equal(s21_decimal number1, s21_decimal number2) {
  int equal = 1;
  int number1_sign = s21_getsign(number1);
  int number2_sign = s21_getsign(number2);

  if (number1_sign && !number2_sign) {
    equal = 0;
  }
  if (!number1_sign && number2_sign) {
    equal = 0;
  }

  s21_check_scale(&number1, &number2);
  for (int i = 95; i >= 0; i--) {
    int bit1 = s21_check_bit(i, number1);
    int bit2 = s21_check_bit(i, number2);
    if (bit1 != bit2) {
      equal = 0;
    }
  }
  return equal;
}

int s21_is_not_equal(s21_decimal number1, s21_decimal number2) {
  int result = TRUE;
  if (s21_is_equal(number1, number2)) {
    result = FALSE;
  }
  return result;
}

int s21_is_greater(s21_decimal number1, s21_decimal number2) {
  int result = 1;
  s21_normalize_scale(&number1, &number2);
  if (s21_getsign(number1) && !s21_getsign(number2)) {
    result = 0;
  } else if (!s21_getsign(number1) && s21_getsign(number2)) {
    result = 1;
  } else if (s21_getsign(number1) == s21_getsign(number2)) {
    for (int i = 2; i >= 0; i--) {
      if (number1.bits[i] > number2.bits[i]) {
        result = 1;
        break;
      } else if (number1.bits[i] < number2.bits[i]) {
        result = 0;
        break;
      } else if (number1.bits[i] == number2.bits[i]) {
        result = 0;
        continue;
      }
    }
    if (s21_getbit(number1, 127)) {
      if (result == 1)
        result = 0;
      else
        result = 1;
    }
  }
  return result;
}

int s21_is_greater_or_equal(s21_decimal number1, s21_decimal number2) {
  int result = s21_is_greater(number1, number2);
  if (s21_is_equal(number1, number2)) result = 1;
  return result;
}

int s21_is_less(s21_decimal number1, s21_decimal number2) {
  return (s21_is_greater(number2, number1));
}

int s21_is_less_or_equal(s21_decimal number1, s21_decimal number2) {
  int res = s21_is_less(number1, number2);
  if (s21_is_equal(number1, number2)) res = 1;
  return res;
}

int s21_getsign(s21_decimal number) {
  int sign = s21_check_bit(127, number) % 2;
  return sign;
}

void s21_check_scale(s21_decimal *number1, s21_decimal *number2) {
  int scale_1 = s21_getscale(*number1);
  int scale_2 = s21_getscale(*number2);
  if (scale_1 != scale_2) {
    s21_normalize_scale(number1, number2);
  }
}

int s21_normalize_scale(s21_decimal *number1, s21_decimal *number2) {
  s21_decimal number2_buffer;
  s21_give_null_decimal(&number2_buffer);
  int scale_number1 = s21_getscale(*number1);
  int scale_number2 = s21_getscale(*number2);
  int scale = scale_number1;

  if (scale_number1 > scale_number2 && scale_number1 <= 28) {
    s21_renew_number(*number2, &number2_buffer);
    for (int i = scale_number2; i < scale_number1; i++) {
      s21_multiply_decimal_by_ten(number2_buffer, &number2_buffer);
    }
    s21_renew_number(number2_buffer, number2);
    s21_setscale(scale_number1, number2);
    scale = scale_number1;
  } else if (scale_number1 < scale_number2 && scale_number2 <= 28) {
    s21_renew_number(*number1, &number2_buffer);
    for (int i = scale_number1; i < scale_number2; i++) {
      s21_multiply_decimal_by_ten(number2_buffer, &number2_buffer);
    }
    s21_renew_number(number2_buffer, number1);
    s21_setscale(scale_number2, number1);
    scale = scale_number2;
  }
  return scale;
}

// rewrite decimal with the assignment
int s21_renew_number(s21_decimal number1, s21_decimal *number2) {
  for (int i = 0; i < 128; i++) {
    s21_nullify_bit(i, number2);
    if (s21_check_bit(i, number1)) s21_add_bit(i, number2);
  }
  return 1;
}

int s21_getscale(s21_decimal decimal) {
  int how_many_tens = 0;
  for (int i = 119; i >= 112; i--)
    how_many_tens += s21_check_bit(i, decimal) * pow(2, i - 112);
  return how_many_tens;
}

int s21_setscale(int how_many_tens, s21_decimal *decimal) {
  for (int i = 112; i <= 119; i++) {
    s21_nullify_bit(i, decimal);
    if (s21_check_bit_number(how_many_tens, i - 112)) {
      s21_add_bit(i, decimal);
    }
  }
  return 1;
}

int s21_nullify_bit(int position, s21_decimal *dst) {
  int SetBitsArray = position / 32;
  int SetBit = position % 32;
  dst->bits[SetBitsArray] = dst->bits[SetBitsArray] & ~(1 << SetBit);
  return 1;
}

// Multiply decimal by ten to raise scale by 1
int s21_multiply_decimal_by_ten(s21_decimal decimal, s21_decimal *BuffDecimal) {
  int result = 0;
  for (int i = 1; i < 10; i++)
    result += s21_summa_simple(*BuffDecimal, decimal, BuffDecimal);
  return result;
}

// sum of two decimals having the same scale and sign
int s21_summa_simple(s21_decimal number1, s21_decimal number2,
                     s21_decimal *NumSumResult) {
  int in_mind = 0;
  for (int i = 0; i <= 95; i++) {
    int sum = s21_check_bit(i, number1) + s21_check_bit(i, number2) + in_mind;
    in_mind = 0;
    if (sum == 3 || sum == 1)
      s21_add_bit(i, NumSumResult);
    else
      s21_nullify_bit(i, NumSumResult);
    if (sum >= 2) in_mind = 1;
  }
  return in_mind;
}
