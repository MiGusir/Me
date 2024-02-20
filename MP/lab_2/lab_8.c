#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>


void addNumbers(int base, char* result, const char* number) {


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

char* sumNumbers(int base, int count, ...) {
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
        
        addNumbers(base, result, number);
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
    char* result = sumNumbers(10, 3, "123", "251", "145");
    printf("%s\n", result);
    free(result);

    return 0;
}