#include "./s21_decimal.h"

// addition

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res = SUCCESS;
  int value_1_sign = s21_get_sign(&value_1);
  int value_2_sign = s21_get_sign(&value_2);
  unsigned int mask = 1u << 31;
  memset(result->bits, 0, sizeof(result->bits));
  if (!value_1_sign && !value_2_sign) {
    int value_1_scale = (char)(value_1.bits[3] >> 16);
    int value_2_scale = (char)(value_2.bits[3] >> 16);
    if (value_1_scale != value_2_scale) {
      s21_normalize_scale(&value_1, &value_2);
    }
    int bitsadded = s21_NORMAL_VALUE;
    s21_decimal temporary;
    bitsadded = s21_addbits(value_1, value_2, &temporary);
    if (bitsadded == s21_POS_INF_VAL) {
      res = s21_POS_INF_VAL;
    } else {
      *result = temporary;
      result->bits[3] = value_1.bits[3];
    }
  } else if (value_1_sign && !value_2_sign) {
    value_1.bits[3] &= ~mask;
    res = s21_sub(value_2, value_1, result);
  } else if (!value_1_sign && value_2_sign) {
    value_2.bits[3] &= ~mask;
    res = s21_sub(value_1, value_2, result);
  } else {
    value_1.bits[3] &= ~mask;
    value_2.bits[3] &= ~mask;
    res = s21_add(value_1, value_2, result);
    if (res == s21_POS_INF_VAL)
      res = s21_NEG_INF_VAL;
    else
      result->bits[3] |= mask;
  }
  return res;
}

// subtraction

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int return_value = SUCCESS, resultSign;
  memset(result->bits, 0, sizeof(result->bits));
  int sign_1 = s21_get_sign(&value_1), sign_2 = s21_get_sign(&value_2);
  int scale_1 = (char)(value_1.bits[3] >> 16);
  int scale_2 = (char)(value_2.bits[3] >> 16);
  if (scale_1 != scale_2) {
    s21_normalize_scale(&value_1, &value_2);
  }
  unsigned int mask = 1u << 31;
  s21_set_sign(&value_1, sign_1);
  s21_set_sign(&value_2, sign_2);
  if (s21_get_sign(&value_1) != s21_get_sign(&value_2)) {
    resultSign = s21_get_sign(&value_1);
    value_1.bits[3] &= ~mask;
    value_2.bits[3] &= ~mask;
    return_value = s21_add(value_1, value_2, result);
    if (return_value == s21_POS_INF_VAL) {
      return_value = s21_NEG_INF_VAL;
    } else {
      s21_set_sign(result, resultSign);
    }
  } else {
    if (s21_is_equal(value_1, value_2) == TRUE) {
    } else {
      int sign1 = s21_get_sign(&value_1);
      int sign2 = s21_get_sign(&value_2);
      value_1.bits[3] &= ~mask;
      value_2.bits[3] &= ~mask;
      s21_decimal *small_value, *big_value;
      if (s21_is_less(value_1, value_2) == TRUE) {
        small_value = &value_1;
        big_value = &value_2;
        resultSign = !sign2;
      } else {
        small_value = &value_2;
        big_value = &value_1;
        resultSign = sign1;
      }
      s21_sub_only_bits(*big_value, *small_value, result);
      int scale_value_1 = s21_getscale(value_1);
      s21_setscale(scale_value_1, result);
      s21_set_sign(result, resultSign);
    }
  }
  return return_value;
}

// multiplication

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_give_null_decimal(result);
  int return_value = s21_NORMAL_VALUE;
  int sign_result;

  if (s21_check_bit(127, value_1) != s21_check_bit(127, value_2)) {
    sign_result = 1;
  } else {
    sign_result = 0;
  }

  int last_bit_1 = s21_last_bit(value_1);
  s21_decimal tmp_res = {{0, 0, 0, 0}};
  int bit_addition_result = s21_NORMAL_VALUE;
  for (int i = 0; i <= last_bit_1; i++) {
    s21_give_null_decimal(&tmp_res);
    int value_bit_1 = s21_check_bit(i, value_1);
    if (value_bit_1) {
      tmp_res = value_2;
      s21_shift(&tmp_res, i);
      bit_addition_result = s21_addbits(*result, tmp_res, result);
    }
  }

  if (bit_addition_result == s21_POS_INF_VAL) {
    if (sign_result)
      return_value = s21_NEG_INF_VAL;
    else
      return_value = s21_POS_INF_VAL;
    s21_give_null_decimal(result);
  } else {
    int scale = s21_getscale(value_1) + s21_getscale(value_2);
    s21_setscale(scale, result);

    s21_set_sign(result, sign_result);
  }
  return return_value;
}

// division

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int return_result = s21_NORMAL_VALUE;
  s21_give_null_decimal(result);
  s21_decimal zero_value = {{0, 0, 0, 0}};
  if (s21_is_equal(value_2, zero_value) == TRUE) {
    return_result = s21_NAN;
  } else {
    int startScale = s21_getscale(value_1) - s21_getscale(value_2);
    int sing_result = s21_check_bit(127, value_1) - s21_check_bit(127, value_2);
    if (sing_result == -1) {
      sing_result *= -1;
    }
    s21_decimal value_under_point = {{0, 0, 0, 0}};
    s21_decimal buf = {{0, 0, 0, 0}};
    s21_setscale(0, &value_1);
    s21_setscale(0, &value_2);
    s21_nullify_bit(127, &value_1);
    s21_nullify_bit(127, &value_2);
    s21_div_only_bits(value_1, value_2, &value_under_point, &buf);
    *result = buf;
    s21_decimal limit_value = {{4294967295u, 4294967295, 4294967295u, 0}};
    s21_decimal ten_value = {{10, 0, 0, 0}};
    s21_setscale(1, &limit_value);
    int scale_general = 0;
    for (; scale_general <= 27 &&
           s21_is_equal(value_under_point, zero_value) == FALSE;) {
      if (s21_is_less(*result, limit_value) == FALSE) {
        break;
      }
      s21_mul(value_under_point, ten_value, &value_under_point);
      s21_div_only_bits(value_under_point, value_2, &value_under_point, &buf);
      s21_mul(*result, ten_value, result);
      s21_addbits(*result, buf, result);
      scale_general++;
    }
    s21_decimal cash;
    int full_scale = startScale + scale_general;
    while (full_scale > 28) {
      s21_div_only_bits(*result, ten_value, &cash, result);
      full_scale--;
    }
    while (full_scale < 0) {
      s21_mul(*result, ten_value, result);
      full_scale++;
    }
    s21_setscale(full_scale, result);
    s21_set_sign(result, sing_result);
  }
  return return_result;
}

// remainder of division

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  memset(result->bits, 0, sizeof(result->bits));
  int return_value = s21_NORMAL_VALUE;
  s21_decimal zero = {{0, 0, 0, 0}};
  if (s21_is_equal(value_2, zero)) {
    return_value = s21_NAN;
  } else {
    if (!s21_get_sign(&value_1) && !s21_get_sign(&value_2)) {
      while (s21_is_greater_or_equal(value_1, value_2)) {
        s21_sub(value_1, value_2, &value_1);
      }
    } else if (!s21_get_sign(&value_1) && s21_get_sign(&value_2)) {
      s21_set_sign(&value_2, 0);
      while (s21_is_greater_or_equal(value_1, value_2)) {
        s21_sub(value_1, value_2, &value_1);
      }
    } else if (s21_get_sign(&value_1) && !s21_get_sign(&value_2)) {
      s21_set_sign(&value_1, 0);
      while (s21_is_greater_or_equal(value_1, value_2)) {
        s21_sub(value_1, value_2, &value_1);
      }
      s21_set_sign(&value_1, 1);
    } else if (s21_get_sign(&value_1) && s21_get_sign(&value_2)) {
      s21_set_sign(&value_1, 0);
      s21_set_sign(&value_2, 0);
      while (s21_is_greater_or_equal(value_1, value_2)) {
        s21_sub(value_1, value_2, &value_1);
      }
      s21_set_sign(&value_1, 1);
    }
    *result = value_1;
  }
  return return_value;
}

int s21_get_sign(const s21_decimal *dst) {
  unsigned int mask = 1u << 31;
  return (dst->bits[3] & mask);
}

void s21_set_sign(s21_decimal *dst, int sign) {
  unsigned int mask = 1u << 31;
  if (sign == 0) {
    dst->bits[3] &= ~mask;
  } else {
    dst->bits[3] |= mask;
  }
}

int s21_addbits(s21_decimal number1, s21_decimal number2, s21_decimal *result) {
  int res = SUCCESS;
  int temp = 0;
  memset(result->bits, 0, sizeof(result->bits));
  for (int i = 0; i < 96; i++) {
    int number1_bit = s21_getbit(number1, i);
    int number2_bit = s21_getbit(number2, i);
    if (!number1_bit && !number2_bit) {
      if (temp) {
        s21_setbit(result, i, 1);
        temp = 0;
      } else {
        s21_setbit(result, i, 0);
      }
    } else if (number1_bit != number2_bit) {
      if (temp) {
        s21_setbit(result, i, 0);
        temp = 1;
      } else {
        s21_setbit(result, i, 1);
      }
    } else {
      if (temp) {
        s21_setbit(result, i, 1);
        temp = 1;
      } else {
        s21_setbit(result, i, 0);
        temp = 1;
      }
    }
    if (i == 95 && temp == 1) {
      res = s21_POS_INF_VAL;
    }
  }
  return res;
}

int s21_getbit(const s21_decimal dst, int bit) {
  int res = 0;
  if (bit / 32 < 4) {
    unsigned int mask = 1u << (bit % 32);
    res = dst.bits[bit / 32] & mask;
  }
  return !!res;
}

void s21_setbit(s21_decimal *dst, int bit, int number) {
  unsigned int mask = 1u << (bit % 32);
  if (bit / 32 < 4 && number) {
    dst->bits[bit / 32] |= mask;
  } else if (bit / 32 < 4 && !number) {
    dst->bits[bit / 32] &= ~mask;
  }
}
