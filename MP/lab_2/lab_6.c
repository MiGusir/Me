#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

enum Status {GOOD_ERROR, BAD_MEANS_ERROR, FILE_ERROR};

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

int roman_to_int(const char* roman) {
    int len = strlen(roman);
    int result = 0;
    int prev_value = 0;

    for (int i = len - 1; i >= 0; i--) {
        char c = roman[i];
        int value = 0;

        switch (c) {
            case 'I':
                value = 1;
                break;
            case 'V':
                value = 5;
                break;
            case 'X':
                value = 10;
                break;
            case 'L':
                value = 50;
                break;
            case 'C':
                value = 100;
                break;
            case 'D':
                value = 500;
                break;
            case 'M':
                value = 1000;
                break;
            default:
                return 0; // некорректная римская цифра
        }

        if (value >= prev_value) {
            result += value;
        } else {
            result -= value;
        }

        prev_value = value;
    }

    return result;
}

void toZeckendorf(unsigned int n, char* result, int* len) {
    if (n == 0) {
        strcpy(result, "0");
        return;
    }

    int fib[32];
    fib[0] = 1;
    fib[1] = 2;

    for (int i = 2; i < 32; i += 1) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }

    int i = 31;
    int poz = 0;
    int length = 0;
    while (i >= 0) {

        if (poz == (*len - 2)) {
            *len *= 2;
            if ((result = (char*)realloc(result, sizeof(char) * *len)) == NULL) {
                return;
            }
        }

        if (n >= fib[i]) {
            result[poz] = '1';
            n -= fib[i];
            length += 1;
            poz += 1;
        } else if (length > 0) {
            result[poz] = '0';
            poz += 1;
        }        
        i -= 1;
    }

    result[poz] = '\0';
    strrev(result);
    result[poz] = '1';
    result[poz + 1] = '\0';
    return;
}

int from_to_10(char* num, int base) {
    int res = 0;
    char* ptr = num;
    if (*ptr == '-') {
        ptr += 1;
    }
    while (*ptr) {
        if (isdigit(*ptr) || isalpha(*ptr)) {
            res = res * base + (isdigit(*ptr) ? *ptr - '0' : *ptr - 'a' + 10);
        }
        ptr += 1;
    }
    return res;
}

int from_to_10_big(char* num, int base) {
    int res = 0;
    char* ptr = num;
    if (*ptr == '-') {
        ptr += 1;
    }
    while (*ptr) {
        if (isdigit(*ptr) || isalpha(*ptr)) {
            res = res * base + (isdigit(*ptr) ? *ptr - '0' : *ptr - 'A' + 10);
        }
        ptr += 1;
    }
    return res;
}

void from_10_to(int num, int base, char* bufer, int* len) {
    int r;
    int poz = 0;
    if (poz == (*len - 2)) {
        *len *= 2;
        if ((bufer = (char*)realloc(bufer, sizeof(char) * *len)) == NULL) {
            return;
        }
    }
    while (num > 0) {
        r = num % base;
        bufer[poz] = (r > 9) ? r - 10 + 'a' : r + '0';
        poz += 1;
        num /= base;
    }

    bufer[poz] = '\0';
    strrev(bufer);
    bufer[poz] = '\0';
    return;
}

int overfscanf(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    int written = 0;
    const char* p = format;
    char f;
    char* buf;
    int len = 16;
    int poz = 0;
    
    while (*p != '\0') {
        f = fgetc(stream);
        fprintf("%c", f);
        p += 1;

//         if (*p != '%') {

//             if (*p != )
//             p += 1;

//         } else if (*p == '%' && *(p + 1) == 'R' && *(p + 2) == 'o') {

//             p += 3;
//             char* arg = va_arg(args, char*);
//             char roman[10]; // болше 3999 чисел нет
//             int result = roman_to_int(arg);
//             // int result = fputs(roman, stream);
//             if (result < 0) {
//                 return result;
//             }
//             written += result;

//         } else if (*p == '%' && *(p + 1) == '%') {

//             fputc(*p, stream);
//             p += 2;
//             written += 1;

//         } else if (*p == '%' && *(p + 1) == 'Z' && *(p + 2) == 'r') {

//             p += 3;
//             int arg = va_arg(args, int);
//             if ((buf = (char*)malloc(sizeof(char) * len)) == NULL) {
//                 return 0;
//             }
//             toZeckendorf(arg, buf, &len);
//             int result = fputs(buf, stream);
//             if (result < 0) {
//                 return result;
//             }
//             written += result;
//             free(buf);
            
//         }  else if (*p == '%' && *(p + 1) == 'C' && *(p + 2) == 'v') {

//             p += 3;
//             int arg = va_arg(args, int);
//             int base = va_arg(args, int);
//             if (base > 26 || base < 2) {
//                 base = 10;
//             }
//             if ((buf = (char*)malloc(sizeof(char) * len)) == NULL) {
//                 return 0;
//             }
            
//             from_10_to(arg, base, buf, &len);
//             int result = fputs(buf, stream);
//             if (result < 0) {
//                 return result;
//             }
//             written += result;
//             free(buf);
            
//         } else if (*p == '%' && *(p + 1) == 'C' && *(p + 2) == 'V') {

//             p += 3;
//             int arg = va_arg(args, int);
//             int base = va_arg(args, int);
//             if (base > 26 || base < 2) {
//                 base = 10;
//             }
//             if ((buf = (char*)malloc(sizeof(char) * len)) == NULL) {
//                 return 0;
//             }
            
//             // from_10_to_big(arg, base, buf, &len);
//             int result = fputs(buf, stream);
//             if (result < 0) {
//                 return result;
//             }
//             written += result;
//             free(buf);
            
//         } else if (*p == '%' && *(p + 1) == 't' && *(p + 2) == 'o') {

//             p += 3;
//             char* arg = va_arg(args, char*);
//             int base = va_arg(args, int);
//             if (base > 26 || base < 2) {
//                 base = 10;
//             }
//             int result = fprintf(stream, "%d", from_to_10(arg, base));
//             if (result < 0) {
//                 return result;
//             }
//             written += result;
            
//         } else if (*p == '%' && *(p + 1) == 'T' && *(p + 2) == 'O') {

//             p += 3;
//             char* arg = va_arg(args, char*);
//             int base = va_arg(args, int);
//             if (base > 26 || base < 2) {
//                 base = 10;
//             }
//             int result = fprintf(stream, "%d", from_to_10_big(arg, base));
//             if (result < 0) {
//                 return result;
//             }
//             written += result;
            
//         } else {
            
//             written += 1;
//             poz = 0;
//             if ((buf = (char*)malloc(sizeof(char) * len)) == NULL) {
//                 return 0;
//             }
//             buf[poz] = '%';
//             poz += 1;

//             while ((*(p + 1) != ' ') && (*(p + 1) != '%') && (*(p + 1) != '\0')) {
//                 p += 1;
//                 if (poz == (len - 2)) {
//                     len *= 2;
//                     if ((buf = (char*)realloc(buf, sizeof(char) * len)) == NULL) {
//                         return 1;
//                     }
//                 }
//                 buf[poz] = *p;
//                 poz += 1;
//             }
//             p += 1;            

//             buf[poz] = '\0';
//             poz += 1;
//             int sum = vfprintf(stream, buf, args);
//             written += sum;
//             free(buf);
//         }
    }
    
    va_end(args);
    return written;
}

int oversscanf(char *stream, const char *format, ...) {

}

int main() {
    int num;
    int num1;
    overfscanf(stdin, "qw %d %Ro\n", &num1, &num);

    // char str[300];
    // oversscanf(str, "%Ro\n", &num);
    // printf("%s", str);

    return 0;
}
