#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

enum Status {GOOD_ERROR, BAD_MEANS_ERROR, FILE_ERROR};

void add_numbers_in_base(int base, char* result, const char* number) {


    char HexSymbols[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    int carry = 0;
    int digit1, digit2, sum;
    int max = (strlen(number) > strlen(result)) ? strlen(number) : strlen(result);
    int l = 0;

    for (int i = 0; i != max; i += 1) {

        if (isdigit(number[strlen(number) - i - 1])) {
            digit1 = (i < strlen(number)) ? (number[strlen(number) - i - 1] - '0') : 0;
        } else {
            digit1 = (i < strlen(number)) ? (number[strlen(number) - i - 1] - 'A' + 10) : 0;
        }

        if (isdigit(result[i])) {
            digit2 = (i < strlen(result)) ? (result[i] - '0') : 0;
        } else {
            digit2 = (i < strlen(result)) ? (result[i] - 'A' + 10) : 0;
        }

        sum = digit1 + digit2 + carry;
        carry = sum / base;

        result[i] = HexSymbols[sum % base];
        l += 1;
    }

    if (carry > 0) {
        result[l] = HexSymbols[carry];
        result[max + 1] = '\0';
    } else {
        result[max + 1] = '\0';
    }

    return;
}

void strrev(char* str)
{
    if (!str) {
        return;
    }
    int i = 0;
    int j = strlen(str) - 1;
 
    while (i < j) {
        char c = str[i];
        str[i] = str[j];
        str[j] = c;
        i++;
        j--;
    }
}

char* sum_in_base(int base, int count, ...) {
    va_list numbers;
    va_start(numbers, count);

    char* number = va_arg(numbers, char*);
    char* result;
    int len = strlen(number) + 1;
    if ((result = (char*)malloc(sizeof(char) * len)) == NULL) {
        return NULL;
    }
    for (int i = 0; i != strlen(number); i += 1) {
        result[i] = number[strlen(number) - i - 1];
    }
    result[len - 1] = '\0';

    for (int i = 0; i < count - 1; i += 1) {

        number = va_arg(numbers, char*);
        if (strlen(number) + 1 > len) {
            len = strlen(number) + 1;
            if ((result = (char*)realloc(result, sizeof(char) * len)) == NULL) {
            return NULL;
            }
        }

        // printf("%s %s\n", result, number);
        
        add_numbers_in_base(base, result, number);
    }

    va_end(numbers);
    
    if (result[strlen(result) -1] == '0') {
        for (int i = strlen(result); i != 0; i -= 1) {
            if (result[i] == '0') {
                result[i] = '\0';
            }
        }
    }

    strrev(result);
    return result;
}

int main() {

    char* result1 = sum_in_base(2, 3, "1010", "10", "101");
    printf("Результат: %s\n", result1);
    free(result1);

    char* result2 = sum_in_base(10, 3, "012", "012", "000112");
    printf("Результат: %s\n", result2);
    free(result2);

    char* result3 = sum_in_base(16, 3, "F", "1", "A1");
    printf("Результат: %s\n", result3);
    free(result3);

    return 0;
}

// #include <stdio.h>
// #include <stdarg.h>
// #include <stdbool.h>
// #include <string.h>

// int sum(int base, int count, ...) {
//     va_list args;
//     va_start(args, count);

//     int result = 0;

//     for (int i = 0; i < count; i++) {
//         const char* num_str = va_arg(args, const char*);
        
//         // Преобразование строки в число в заданной системе счисления
//         int num = 0;
//         int len = strlen(num_str);
//         int multiplier = 1;
        
//         for (int j = len - 1; j >= 0; j--) {
//             char digit = num_str[j];
//             int value;
            
//             if (digit >= '0' && digit <= '9') {
//                 value = digit - '0';
//             } else if (digit >= 'A' && digit <= 'Z') {
//                 value = 10 + digit - 'A';
//             } else if (digit >= 'a' && digit <= 'z') {
//                 value = 10 + digit - 'a';
//             } else {
//                 // Некорректный символ - пропускаем число
//                 break;
//             }
            
//             if (value >= base) {
//                 // Некорректная цифра - пропускаем число
//                 break;
//             }
            
//             // Умножаем цифру на текущий разряд
//             num += value * multiplier;
//             multiplier *= base;
//         }
        
//         result += num;
//     }

//     va_end(args);
//     return result;
// }

// int main() {
//     int sum1 = sum(10, 3, "12", "000000012", "0");
//     printf("Сумма: %d\n", sum1);

//     int sum2 = sum(16, 2, "A", "B");
//     printf("Сумма: %d\n", sum2);

//     return 0;
// }
