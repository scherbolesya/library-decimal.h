#include "./s21_decimal.h"

/*
Function rounds Decimal number to
the nearest integer towards negative infinity
*/
int s21_floor(s21_decimal value, s21_decimal *result) {
  int error = 0, sign = 0;
  s21_give_null_decimal(result);
  s21_decimal *ptr = result;
  sign = s21_check_bit_number(value.bits[3], 31);
  if (sign) {
    s21_truncate(value, ptr);
    s21_decimal *duplicate = ptr;
    if (!s21_is_equal(*duplicate, value)) {
      s21_nullify_bit(127, duplicate);
      s21_decimal one;
      one.bits[0] = 0b00000000000000000000000000000001;
      one.bits[1] = 0b00000000000000000000000000000000;
      one.bits[2] = 0b00000000000000000000000000000000;
      one.bits[3] = 0b00000000000000000000000000000000;
      s21_add(*duplicate, one, duplicate);
      s21_add_bit(127, duplicate);
    }
    *result = *duplicate;
  } else {
    s21_truncate(value, ptr);
    *result = *ptr;
  }
  return error;
}

/*
Rounds Decimal to the nearest integer
*/
int s21_round(s21_decimal value, s21_decimal *result) {
  s21_give_null_decimal(result);
  int return_value = SUCCESS;
  int sign = s21_check_bit(127, value);
  s21_setbit(&value, 127, 0);
  s21_decimal tmp;
  s21_give_null_decimal(&tmp);
  s21_truncate(value, &tmp);
  s21_decimal tmp_copy = tmp;
  s21_sub(value, tmp, &tmp);
  s21_decimal zero_five = {{5, 0, 0, 0}};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_setscale(1, &zero_five);
  if (!s21_is_greater_or_equal(tmp, zero_five)) {
    *result = tmp_copy;
  } else {
    return_value = s21_add(tmp_copy, one, result);
  }
  if (sign) {
    s21_add_bit(127, result);
  }
  return return_value;
}

/*
returns whole digits of the specified Decimal number;
any fractional digits are discarded, including trailing zeros
*/

int s21_truncate(s21_decimal value, s21_decimal *result) {
  s21_give_null_decimal(result);
  s21_decimal ten = {{10, 0, 0, 0}};
  s21_decimal tmp = {{0, 0, 0, 0}};
  int sign_value = s21_check_bit(127, value);
  int scale = s21_getscale(value);
  if (scale) {
    for (int i = scale; i > 0; i--) {
      s21_div_only_bits(value, ten, &tmp, result);
      value = *result;
    }
  } else {
    *result = value;
  }
  if (sign_value) s21_add_bit(127, result);
  return SUCCESS;
}

/*
Function returns the result of multiplying the specified Decimal by -1.
*/
int s21_negate(s21_decimal value, s21_decimal *result) {
  s21_give_null_decimal(result);
  *result = value;
  int sing = s21_check_bit(127, value);
  if (sing) {
    s21_nullify_bit(127, result);
  } else {
    s21_add_bit(127, result);
  }
  return SUCCESS;
}

/*
Shift decimal main part [0-95] step > 0 -> left or step < 0 -> right
*/
int s21_shift(s21_decimal *value, int step) {
  int result_out = s21_NORMAL_VALUE;
  int highest_bit = s21_last_bit(*value);
  if (highest_bit + step > 95) {
    result_out = s21_POS_INF_VAL;
  }
  if (step > 0) {
    for (int i = 0; i < step; i++) {
      int zero_block_arr_last_bit = s21_check_bit(31, *value);
      int one_block_arr_last_bit = s21_check_bit(63, *value);
      value->bits[0] <<= 1;
      value->bits[1] <<= 1;
      value->bits[2] <<= 1;
      if (zero_block_arr_last_bit) {
        s21_add_bit(32, value);
      }
      if (one_block_arr_last_bit) {
        s21_add_bit(64, value);
      }
    }
  } else {
    for (int i = 0; i < (-1) * step; i++) {
      int zero_block_arr_last_bit = s21_check_bit(31, *value);
      int one_block_arr_last_bit = s21_check_bit(63, *value);
      value->bits[0] <<= 1;
      value->bits[1] <<= 1;
      value->bits[2] <<= 1;
      if (zero_block_arr_last_bit) {
        s21_add_bit(32, value);
      }
      if (one_block_arr_last_bit) {
        s21_add_bit(64, value);
      }
    }
  }
  return 1;
  if (s21_check_bit(127, *value) && result_out) {
    result_out = s21_NEG_INF_VAL;
  }
  return result_out;
}

void s21_sub_only_bits(s21_decimal value_1, s21_decimal value_2,
                       s21_decimal *result) {
  memset(result->bits, 0, sizeof(result->bits));
  if (s21_is_equal(value_1, value_2) != TRUE) {
    int value_1_last_bit = s21_last_bit(value_1);
    int buffer = 0;
    for (int i = 0; i <= value_1_last_bit; i++) {
      int current_bit_of_value_1 = s21_check_bit(i, value_1);
      int current_bit_of_value_2 = s21_check_bit(i, value_2);
      if (!current_bit_of_value_1 && !current_bit_of_value_2) {
        if (buffer) {
          buffer = 1;
          s21_add_bit(i, result);  // change to one
        } else {
          s21_nullify_bit(i, result);  // change to zero
        }
      } else if (current_bit_of_value_1 && !current_bit_of_value_2) {
        if (buffer) {
          buffer = 0;
          s21_nullify_bit(i, result);  // put 0
        } else {
          s21_add_bit(i, result);  // put 1
        }
      } else if (!current_bit_of_value_1 && current_bit_of_value_2) {
        if (buffer) {
          buffer = 1;
          s21_nullify_bit(i, result);
        } else {
          buffer = 1;
          s21_add_bit(i, result);
        }
      } else if (current_bit_of_value_1 && current_bit_of_value_2) {
        if (buffer) {
          buffer = 1;
          s21_add_bit(i, result);
        } else {
          s21_nullify_bit(i, result);
        }
      }
    }
  }
}

int s21_last_bit(s21_decimal value) {
  int last_bit = 95;
  for (; last_bit >= 0 && s21_check_bit(last_bit, value) == 0; last_bit--) {
  }
  return last_bit;
}

void s21_div_only_bits(s21_decimal value_1, s21_decimal value_2,
                       s21_decimal *buf, s21_decimal *result) {
  s21_give_null_decimal(buf);
  s21_give_null_decimal(result);
  for (int i = s21_last_bit(value_1); i >= 0; i--) {
    if (s21_check_bit(i, value_1)) {
      s21_add_bit(0, buf);
    }
    if (s21_is_greater_or_equal(*buf, value_2) == TRUE) {
      s21_sub_only_bits(*buf, value_2, buf);
      if (i != 0) {
        s21_shift(buf, 1);
      }
      if (s21_check_bit(i - 1, value_1)) {
        s21_add_bit(0, buf);
      }
      s21_shift(result, 1);
      s21_add_bit(0, result);
    } else {
      s21_shift(result, 1);
      if (i != 0) {
        s21_shift(buf, 1);
      }
      if ((i - 1) >= 0 && s21_check_bit(i - 1, value_1)) {
        s21_add_bit(0, buf);
      }
    }
  }
}
