#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>


#define size_65 65

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1 += 1;
        str2 += 1;
    }

    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

int get_max(int x, int y) {
    if (x > y) return x;
    return y;
}

int get_max_base(char* num) {
    int res = 0;
    char* ptr = num;
    if (*ptr == '-') {
        ptr += 1;
    }
    while (*ptr) {
        if (isdigit(*ptr) || isalpha(*ptr)) {
            if (res < (isdigit(*ptr) ? *ptr - '0' : *ptr - 'A' + 11)) {
                res = isdigit(*ptr) ? *ptr - '0' : *ptr - 'A' + 11;
            }
        }
        ptr += 1;
    }
    return res;
}

int from_to_10(char* num, int base) {
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

char* from_10_to(int num, int base) {
    char bufer[size_65] = "";
    int r;
    char* ptr = bufer + size_65 - 1;
    *ptr = '\0';
    while (num > 0) {
        r = num % base;
        *--ptr = (r > 9) ? r - 10 + 'A' : r + '0';
        num /= base;
    }
    return ptr;
}

int main(int argc, char* argv[]) {

    if (argc != 1) {
        printf("Передано неверное количество флагов\n");
        return 0;
    }

    char string[size_65] = "";
    int base = 0;
    int max_mean = 0;

    printf("Введите основание системы счисления [2-36]: ");
    scanf("%d", &base);
    if (base < 2 || base > 36) return 1;
    printf("Введите числа до Stop:\n");  

    while (strcmp(string, "Stop")) {
        if (get_max_base(string) > base) {
            printf("Ошибка СС.\n");
            return 1;
        }
        max_mean = get_max(max_mean, from_to_10(string, base));
        scanf("%s", string);
    }

    if (max_mean) {
        printf("Максимальное число по модулю: %d\n", max_mean);
        printf("Максимальное число по модулю в системе счисления с основанием 9: %s\n", from_10_to(max_mean, 9));
        printf("Максимальное число по модулю в системе счисления с основанием 18: %s\n", from_10_to(max_mean, 18));
        printf("Максимальное число по модулю в системе счисления с основанием 27: %s\n", from_10_to(max_mean, 27));
        printf("Максимальное число по модулю в системе счисления с основанием 36: %s\n", from_10_to(max_mean, 36));
    }

    return 0;
}