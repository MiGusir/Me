#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>


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

int from_to_10(char* num, int base) {
    int res = 0;
    char* ptr = num;
    while (*ptr) {
        if (isdigit(*ptr) || isalpha(*ptr)) {
            res = res*base+(isdigit(*ptr) ? *ptr-'0' : *ptr-'A'+10);
        }
        ptr += 1;
    }
    return res;
}

int from_10_to(int num, int base, char* res) {
    int n = num;
    int r;
    char* ptr = res + 32;
    for (int i = 0; i < 33; i += 1) {
        *(ptr-i) = 0;
    }
    *ptr --= 0;
    while (n > 0) {
        r = n % base;
        *ptr--=(r>9) ? r-10+'A' : r+'0';
        n /= base;
    }
    return 1;
}

int main(int argc, char* argv[]) {

    char string[33] = "";
    int base;
    int max_mean = 0;

    printf("Введите основание системы счисления [2-36]: ");
    scanf("%d", &base);
    printf("Введите числа до Stop:\n");  

    while (strcmp(string, "Stop")) {
        max_mean = get_max(max_mean, from_to_10(string, base));
        scanf("%s", string);
    }

    printf("%d\n", max_mean);

    from_10_to(max_mean, 9, string);
    for (int i = 0; i < 32; i += 1) {
        if (string[i]) printf("%c", string[i]); 
    }
    printf("\n");

    from_10_to(max_mean, 18, string);
    for (int i = 0; i < 32; i += 1) {
        if (string[i]) printf("%c", string[i]); 
    }
    printf("\n");

    from_10_to(max_mean, 27, string);
    for (int i = 0; i < 32; i += 1) {
        if (string[i]) printf("%c", string[i]); 
    }
    printf("\n");

    from_10_to(max_mean, 36, string);
    for (int i = 0; i < 32; i += 1) {
        if (string[i]) printf("%c", string[i]); 
    }
    printf("\n");


    return 0;
}