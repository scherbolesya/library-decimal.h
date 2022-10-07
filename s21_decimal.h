#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#include <limits.h>
#include <math.h>
#include <string.h>

#define minimum 1e-28
#define inf 1.0 / 0.0
#define nan 0.0 / 0.0
#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define ERROR 1

#define s21_NORMAL_VALUE 0
#define s21_POS_INF_VAL 1
#define s21_NEG_INF_VAL 2
#define s21_NAN 3

#define S21_MAX_UINT 4294967295

#define OK 0
#define BIG_NUM 1
#define SMALL_NUM 2

/*---------------------
All members are have the same address.
Therefor data save in decimal format.
---------------------*/
typedef union {
  unsigned int integerValue;
  float floatValue;
} value;

typedef struct {
  int bits[4];
} s21_decimal;

// for converters
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);
int s21_give_null_decimal(s21_decimal *arr);
int s21_add_bit(int position, s21_decimal *arr);
int s21_check_bit(int position, s21_decimal dst);
int s21_check_bit_number(unsigned int number, int position);
int s21_get_binary_power(value val);
int s21_form_float_decimal(s21_decimal *arr, int two_pow, int ten_pow,
                           value val);

// for comparison
int s21_is_equal(s21_decimal number1, s21_decimal number2);
int s21_is_not_equal(s21_decimal number1, s21_decimal number2);
int s21_is_greater(s21_decimal number1, s21_decimal number2);
int s21_is_greater_or_equal(s21_decimal number1, s21_decimal number2);
int s21_is_less(s21_decimal number1, s21_decimal number2);
int s21_is_less_or_equal(s21_decimal number1, s21_decimal number2);
int s21_getsign(s21_decimal number);
void s21_check_scale(s21_decimal *number1, s21_decimal *number2);
int s21_normalize_scale(s21_decimal *number1, s21_decimal *number2);
int s21_renew_number(s21_decimal number1, s21_decimal *number2);
int s21_getscale(s21_decimal decimal);
int s21_setscale(int how_many_tens, s21_decimal *decimal);
int s21_nullify_bit(int position, s21_decimal *dst);
int s21_multiply_decimal_by_ten(s21_decimal decimal, s21_decimal *BuffDecimal);
int s21_summa_simple(s21_decimal number1, s21_decimal number2,
                     s21_decimal *NumSumResult);

// arithmetic
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_get_sign(const s21_decimal *dst);
void s21_set_sign(s21_decimal *dst, int sign);
int s21_addbits(s21_decimal number1, s21_decimal number2, s21_decimal *result);
int s21_getbit(const s21_decimal dst, int bit);
void s21_setbit(s21_decimal *dst, int bit, int number);

// other function
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);
int s21_last_bit(s21_decimal value);
int s21_shift(s21_decimal *value, int step);
void s21_div_only_bits(s21_decimal value_1, s21_decimal value_2,
                       s21_decimal *buf, s21_decimal *result);
void s21_sub_only_bits(s21_decimal value_1, s21_decimal value_2,
                       s21_decimal *result);

#endif  // SRC_S21_DECIMAL_H_
