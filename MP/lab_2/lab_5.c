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

void intToRoman(unsigned int num, char *roman) {
    int values[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    char *symbols[] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};

    if (num <= 0) {
        strcpy(roman, "\0");
        return;
    }

    for (int i = 0; i < 13; i += 1) {
        while (num >= values[i]) {
            strcat(roman, symbols[i]);
            num -= values[i];
        }
    }
    return;
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

void from_10_to_big(int num, int base, char* bufer, int* len) {
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
        bufer[poz] = (r > 9) ? r - 10 + 'A' : r + '0';
        poz += 1;
        num /= base;
    }

    bufer[poz] = '\0';
    strrev(bufer);
    bufer[poz] = '\0';
    return;
}

int print_memory_dump(const void* ptr, size_t size, FILE* stream) {
    int res = 0;
    const unsigned char* byte_ptr = (const unsigned char*)ptr;
    for (size_t i = 0; i < size; i += 1) {
        for (int j = 7; j >= 0; j -= 1) {
            unsigned char bit = (byte_ptr[i] >> j) & 1;
            res += fprintf(stream, "%u", bit);
        }
        if (i < size - 1)
            res += fprintf(stream, " ");
    }
    return res;
}

int print_memory_dump_str(const void* ptr, size_t size, char *stream) {
    int res = 0;
    const unsigned char* byte_ptr = (const unsigned char*)ptr;
    for (size_t i = 0; i < size; i += 1) {
        for (int j = 7; j >= 0; j -= 1) {
            unsigned char bit = (byte_ptr[i] >> j) & 1;
            res += 1;
            char qw[2];
            sprintf(qw, "%u", bit);
            qw[1] = '\0';
            strcat(stream, qw);
        }
        if (i < size - 1) {
            res += 1;
            strcat(stream, " ");
        }
    }
    return res;
}

int overfprintf(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    int written = 0;
    const char* p = format;
    char* buf;
    int len = 16;
    int poz = 0;
    
    while (*p != '\0') {

        if (*p != '%') {

            fputc(*p, stream);
            p += 1;
            written += 1;

        } else if (*p == '%' && *(p + 1) == 'R' && *(p + 2) == 'o') {

            p += 3;
            int arg = va_arg(args, int);
            char roman[10]; // болше 3999 чисел нет
            intToRoman(arg, roman);
            int result = fputs(roman, stream);
            if (result < 0) {
                return result;
            }
            written += result;

        } else if (*p == '%' && *(p + 1) == '%') {

            fputc(*p, stream);
            p += 2;
            written += 1;

        } else if (*p == '%' && *(p + 1) == 'Z' && *(p + 2) == 'r') {

            p += 3;
            int arg = va_arg(args, int);
            if ((buf = (char*)malloc(sizeof(char) * len)) == NULL) {
                return 0;
            }
            toZeckendorf(arg, buf, &len);
            int result = fputs(buf, stream);
            if (result < 0) {
                return result;
            }
            written += result;
            free(buf);
            
        }  else if (*p == '%' && *(p + 1) == 'C' && *(p + 2) == 'v') {

            p += 3;
            int arg = va_arg(args, int);
            int base = va_arg(args, int);
            if (base > 36 || base < 2) {
                base = 10;
            }
            if ((buf = (char*)malloc(sizeof(char) * len)) == NULL) {
                return 0;
            }
            
            from_10_to(arg, base, buf, &len);
            int result = fputs(buf, stream);
            if (result < 0) {
                return result;
            }
            written += result;
            free(buf);
            
        } else if (*p == '%' && *(p + 1) == 'C' && *(p + 2) == 'V') {

            p += 3;
            int arg = va_arg(args, int);
            int base = va_arg(args, int);
            if (base > 36 || base < 2) {
                base = 10;
            }
            if ((buf = (char*)malloc(sizeof(char) * len)) == NULL) {
                return 0;
            }
            
            from_10_to_big(arg, base, buf, &len);
            int result = fputs(buf, stream);
            if (result < 0) {
                return result;
            }
            written += result;
            free(buf);
            
        } else if (*p == '%' && *(p + 1) == 't' && *(p + 2) == 'o') {

            p += 3;
            char* arg = va_arg(args, char*);
            int base = va_arg(args, int);
            if (base > 36 || base < 2) {
                base = 10;
            }
            int result = fprintf(stream, "%d", from_to_10(arg, base));
            if (result < 0) {
                return result;
            }
            written += result;
            
        } else if (*p == '%' && *(p + 1) == 'T' && *(p + 2) == 'O') {

            p += 3;
            char* arg = va_arg(args, char*);
            int base = va_arg(args, int);
            if (base > 36 || base < 2) {
                base = 10;
            }
            int result = fprintf(stream, "%d", from_to_10_big(arg, base));
            if (result < 0) {
                return result;
            }
            written += result;
            
        } else if (*p == '%' && *(p + 1) == 'm' && *(p + 2) == 'u') {

            p += 3;
            int arg = va_arg(args, int);
            unsigned int qa = (unsigned int)arg;

            int result = print_memory_dump(&qa, sizeof(qa), stream);

            if (result < 0) {
                return result;
            }
            written += result;
            
        } else if (*p == '%' && *(p + 1) == 'm' && *(p + 2) == 'i') {

            p += 3;
            int arg = va_arg(args, int);

            int result = print_memory_dump(&arg, sizeof(arg), stream);

            if (result < 0) {
                return result;
            }
            written += result;
            
        } else if (*p == '%' && *(p + 1) == 'm' && *(p + 2) == 'd') {

            p += 3;
            double arg = va_arg(args, double);

            int result = print_memory_dump(&arg, sizeof(arg), stream);

            if (result < 0) {
                return result;
            }
            written += result;
            
        }  else if (*p == '%' && *(p + 1) == 'm' && *(p + 2) == 'f') {

            p += 3;
            double arg = va_arg(args, double);
            float qw = (float)arg;

            int result = print_memory_dump(&qw, sizeof(qw), stream);

            if (result < 0) {
                return result;
            }
            written += result;
            
        } else {
            
            written += 1;
            poz = 0;
            if ((buf = (char*)malloc(sizeof(char) * len)) == NULL) {
                return 0;
            }
            buf[poz] = '%';
            poz += 1;

            while ((*(p + 1) != ' ') && (*(p + 1) != '%') && (*(p + 1) != '\0')) {
                p += 1;
                if (poz == (len - 2)) {
                    len *= 2;
                    if ((buf = (char*)realloc(buf, sizeof(char) * len)) == NULL) {
                        return 1;
                    }
                }
                buf[poz] = *p;
                poz += 1;
            }
            p += 1;            

            buf[poz] = '\0';
            poz += 1;
            int sum = vfprintf(stream, buf, args);
            written += sum;
            free(buf);
        }
    }
    
    va_end(args);
    return written;
}

int oversprintf(char *stream, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    int written = 0;
    const char* p = format;
    char* buf;
    int len = 16;
    int poz = 0;
    
    while (*p != '\0') {

        if (*p != '%') {
            char q[2];
            q[0] = *p;
            q[1] = '\0';
            strcat(stream, q);
            p += 1;
            written += 1;

        } else if (*p == '%' && *(p + 1) == 'R' && *(p + 2) == 'o') {

            p += 3;
            int arg = va_arg(args, int);
            char roman[10]; // болше 3999 чисел нет
            roman[0] = '\0';
            intToRoman(arg, roman);
            int result = strlen(roman);
            strcat(stream, roman);
            if (result < 0) {
                return result;
            }
            written += result;

        } else if (*p == '%' && *(p + 1) == '%') {

            strcat(stream, p);
            p += 2;
            written += 1;

        } else if (*p == '%' && *(p + 1) == 'Z' && *(p + 2) == 'r') {

            p += 3;
            int arg = va_arg(args, int);
            if ((buf = (char*)malloc(sizeof(char) * len)) == NULL) {
                return 0;
            }
            toZeckendorf(arg, buf, &len);
            int result = strlen(buf);
            strcat(stream, buf);
            if (result < 0) {
                return result;
            }
            written += result;
            free(buf);
            
        }  else if (*p == '%' && *(p + 1) == 'C' && *(p + 2) == 'v') {

            p += 3;
            int arg = va_arg(args, int);
            int base = va_arg(args, int);
            if (base > 36 || base < 2) {
                base = 10;
            }
            if ((buf = (char*)malloc(sizeof(char) * len)) == NULL) {
                return 0;
            }
            
            from_10_to(arg, base, buf, &len);
            int result = strlen(buf);
            strcat(stream, buf);
            if (result < 0) {
                return result;
            }
            written += result;
            free(buf);
            
        } else if (*p == '%' && *(p + 1) == 'C' && *(p + 2) == 'V') {

            p += 3;
            int arg = va_arg(args, int);
            int base = va_arg(args, int);
            if (base > 36 || base < 2) {
                base = 10;
            }
            if ((buf = (char*)malloc(sizeof(char) * len)) == NULL) {
                return 0;
            }
            
            from_10_to_big(arg, base, buf, &len);
            int result = strlen(buf);
            strcat(stream, buf);
            if (result < 0) {
                return result;
            }
            written += result;
            free(buf);
            
        } else if (*p == '%' && *(p + 1) == 't' && *(p + 2) == 'o') {

            p += 3;
            char* arg = va_arg(args, char*);
            int base = va_arg(args, int);
            if (base > 36 || base < 2) {
                base = 10;
            }
            int l = 0;
            int w = from_to_10(arg, base);
            char qw[1111];
            sprintf(qw, "%d", w);
            strcat(stream, qw);
            while (w > 0) {
                l += 1;
                w /= 10;
            }
            int result = l;
            if (result < 0) {
                return result;
            }
            written += result;
            
        } else if (*p == '%' && *(p + 1) == 'T' && *(p + 2) == 'O') {

            p += 3;
            char* arg = va_arg(args, char*);
            int base = va_arg(args, int);
            if (base > 36 || base < 2) {
                base = 10;
            }
            int l1 = 0;
            int w1 = from_to_10_big(arg, base);
            char qw1[1111];
            sprintf(qw1, "%d", w1);
            strcat(stream, qw1);
            while (w1 > 0) {
                l1 += 1;
                w1 /= 10;
            }
            int result = l1;            
            if (result < 0) {
                return result;
            }
            written += result;
            
        } else if (*p == '%' && *(p + 1) == 'm' && *(p + 2) == 'i') {

            p += 3;
            int arg = va_arg(args, int);
            unsigned int as = (unsigned int)arg;

            int result = print_memory_dump_str(&as, sizeof(as), stream);

            if (result < 0) {
                return result;
            }
            written += result;
            
        }  else if (*p == '%' && *(p + 1) == 'm' && *(p + 2) == 'u') {

            p += 3;
            int arg = va_arg(args, int);

            int result = print_memory_dump_str(&arg, sizeof(arg), stream);

            if (result < 0) {
                return result;
            }
            written += result;
            
        }else if (*p == '%' && *(p + 1) == 'm' && *(p + 2) == 'd') {

            p += 3;
            double arg = va_arg(args, double);

            int result = print_memory_dump_str(&arg, sizeof(arg), stream);

            if (result < 0) {
                return result;
            }
            written += result;
            
        }  else if (*p == '%' && *(p + 1) == 'm' && *(p + 2) == 'f') {

            p += 3;
            double arg = va_arg(args, double);
            float qw = (float)arg;

            int result = print_memory_dump_str(&qw, sizeof(qw), stream);

            if (result < 0) {
                return result;
            }
            written += result;
            
        } else {
            
            written += 1;
            poz = 0;
            if ((buf = (char*)malloc(sizeof(char) * len)) == NULL) {
                return 0;
            }
            buf[poz] = '%';
            poz += 1;

            while ((*(p + 1) != ' ') && (*(p + 1) != '%') && (*(p + 1) != '\0')) {
                p += 1;
                if (poz == (len - 2)) {
                    len *= 2;
                    if ((buf = (char*)realloc(buf, sizeof(char) * len)) == NULL) {
                        return 1;
                    }
                }
                buf[poz] = *p;
                poz += 1;
            }
            p += 1;            

            buf[poz] = '\0';
            poz += 1;
            char* busi;
             if ((busi = (char*)malloc(sizeof(char) * strlen(buf) + 1)) == NULL) {
                return 0;
            }
            int sum = vsprintf(busi, buf, args);
            strcat(stream, busi);
            free(busi);
            free(buf);
            written += sum;
        }
    }
    
    va_end(args);
    return written;
}

int main() {
    int num = 955;
    overfprintf(stdout, "qwq %s %d %+.4f %Ro %Zr %Cv %CV %to %TO %mu / %mi / %md / %mf\n", "fas", num, 1.123456, num, 54, 10, 3, 13, 16, "a23", 16, "A23", 16, 35, 35, 1.12, 1.12);

    char str[300];
    str[0] ='\0';
    oversprintf(str, "qwq %s %d %+.4f %Ro %Zr %Cv %CV %to %TO %mu / %mi / %md / %mf\n", "fas", num, 1.123456, num, 54, 10, 3, 13, 16, "a23", 16, "A23", 16, 35, 35, 1.12, 1.12);
    printf("Сформированная строка: %s", str);

    return 0;
}
