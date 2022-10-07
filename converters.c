#include "./s21_decimal.h"
/*---------------------
How read!!!
sing -> point -> number
[3][0]...[3][31]...[2][0]...[2][31]...[1][0]...[1][31]...[0][0]...[0][31]
---------------------*/

/*---------------------
Function put null in array.
  Array is massive where will be contain binary numbers.
---------------------*/
int s21_give_null_decimal(s21_decimal *arr) {
  for (int i = 0; i < 4; i++) {
    arr->bits[i] = 0;
  }
  return 1;
}

/*---------------------
FUNCTION PUT BIT IN RIGHT POSTION OF ARR
  you give position (from 0 to 127)
---------------------*/
int s21_add_bit(int position, s21_decimal *arr) {
  int choose_index_arr = position / 32;
  int shift_one_to_correct_index = position % 32;
  // save bit in the right place of arrs
  arr->bits[choose_index_arr] =
      arr->bits[choose_index_arr] | (1 << shift_one_to_correct_index);
  return 1;
}

/*---------------------
Fucntion compare arr.bits[choose_index_arr] == 1 << SetBit)
  Example:
  A = 0011 1100
  B = 0000 1101
  (A & B) -> 0000 1100 - Only in two place there are equal
---------------------*/
int s21_check_bit(int position, s21_decimal arr) {
  int check_bit = 0;
  int choose_index_arr = position / 32;
  int shift_one_to_correct_index = position % 32;
  // & - give one result if in both they are equal
  if (arr.bits[choose_index_arr] & (1 << shift_one_to_correct_index))
    check_bit = 1;
  return check_bit;
}

/*---------------------
Fucntion transfer from int to binary format.
  first 3 index arr are contain number in binary
  4 index of arr brancgis sign and where must be point
  32 + 32 + 32 + 32 = 128 symbols contain arr
---------------------*/
int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int result = SUCCESS;
  if (dst) {
    s21_give_null_decimal(dst);
    if (src < 0) {
      // put 1 to arr[3][0] - sing of "minus"
      s21_add_bit(127, dst);
      src = src * (-1);
    }
    dst->bits[0] = src;
  } else {
    result = ERROR;
  }
  return result;
}

/*---------------------
Fucntion transfer from binary format to int.
  Get [0][0]...[0][31] - because this number begin
  for exmaple: 32 -> arr[0] -> [0 0 0 0 0 1]
  0 + 0 + 0 + 0 + 0 + 1 * 2 * 2 * 2 * 2 * 2 = 32
  29 = 11101. But in arr we save like this arr[0] = [10111]
  1 + 0 * 2 + 1 * 2 * 2 + 1 * 2 * 2 * 2 + 1 * 2 * 2 * 2 = 29
---------------------*/
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int result = SUCCESS;
  *dst = 0;
  s21_decimal buff;
  s21_give_null_decimal(&buff);
  s21_truncate(src, &buff);
  if (s21_last_bit(buff) < 32) {
    for (int i = 0; i < 32; i++) {
      // transer binary to int. Just multiply 2**index
      *dst += pow(2, i) * s21_check_bit(i, buff);
    }
    if (s21_check_bit(127, buff)) *dst = *dst * (-1);
  } else {
    result = ERROR;
  }
  return result;
}

/*---------------------
Function main function convert float to decimal
---------------------*/
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int result = SUCCESS;
  int binaryPower;
  value val;
  double number_double = (double)src;
  s21_give_null_decimal(dst);
  val.floatValue = number_double;
  binaryPower = s21_get_binary_power(val);
  if (isinf(src) || isnan(src)) {
    result = ERROR;
  } else {
    if (number_double != 0.0) {
      int tenPower = 0;
      for (; !(int)(number_double / 1E7); number_double *= 10) tenPower += 1;
      number_double = (float)number_double;
      while (fmod(number_double, 10.0) == 0 && tenPower > 0) {
        number_double = number_double / 10;
        tenPower -= 1;
      }
      if (tenPower <= 28 && (binaryPower > -95 && binaryPower <= 95)) {
        val.floatValue = number_double;
        binaryPower = s21_get_binary_power(val);
        s21_form_float_decimal(dst, binaryPower, tenPower, val);
      } else {
        result = ERROR;
      }
    }
  }
  return result;
}

/*---------------------
Fucntion return bit in position that we point
  get number and position. And look in this
  position we have 1 or 0.
---------------------*/
int s21_check_bit_number(unsigned int number, int position) {
  return (number & (1 << position));
}

/*---------------------
Fucntion convert from exponent (decimal) to normal int. And we get degree of
number take float decimal (val.integerValue) and take exponent [30, 23] and
convert yo normal number
---------------------*/
int s21_get_binary_power(value val) {
  int result = 0, mult;
  for (int i = 30; i > 22; i--) {
    if ((val.integerValue & (1 << i)) == 0) {
      mult = 0;
    } else {
      mult = 1;
    }
    result += mult * pow(2, i - 23);
  }
  result -= 127;
  return result;
}

/*
based function for convert float to decimal
*/
int s21_form_float_decimal(s21_decimal *arr, int two_pow, int ten_pow,
                           value val) {
  if (s21_check_bit_number(val.integerValue, 31)) {
    s21_add_bit(127, arr);
  }
  // binaryPower(начало) = val.integerValue(22)
  s21_add_bit(two_pow, arr);
  int j = two_pow;
  // Check integerValue from symbol[22]. If there is 1 and in arr[0][j]
  for (int i = 22; i >= 0; i--) {
    j--;
    if (val.integerValue & (1 << i)) {
      s21_add_bit(j, arr);
    }
  }
  for (int i = 112; ten_pow; i++) {
    if (ten_pow % 2) {
      s21_add_bit(i, arr);
    }
    ten_pow /= 2;
  }
  return 1;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int result = SUCCESS;
  int power_number = 0;
  for (int i = 119; i >= 112; i--) {
    power_number += s21_check_bit(i, src) * pow(2, i - 112);
  }
  if (!dst || power_number > 28) {
    result = ERROR;
  } else {
    double float_result = 0;
    for (int i = 0; i < 96; i++) {
      float_result += s21_check_bit(i, src) * pow(2, i);
    }
    float_result = float_result / pow(10, power_number);
    if (s21_check_bit(127, src)) {
      float_result *= -1;
    }
    *dst = (float)float_result;
  }
  return result;
}
