#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>

#define ERROUT stdout
#define error(x) fprintf(ERROUT, "%s\n", x)

int is_flag(char* str) {
    if (strlen(str) != 2) return 0;
    char flags[6] = {'h', 'p', 's', 'e', 'a', 'f'};
    if (str[0] != '-' && str[0] != '/') return 0;
    for (int i = 0; i < 6; i += 1) { if (str[1] == flags[i]) return 1; }
    return 0;
}

int strcmp(const char* str1, const char* str2) {
    while ('\0' != *str1 && (*str1 == *str2)) {
        str1 += 1;
        str2 += 1;
    }

    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

int flag_h(unsigned int value, unsigned long int* result) {

    if (value < 1 || value > 100) return 0;

    for (int i = 1; i < 101; i += 1) {
        if (i % value == 0) {
            result[result[0]+1] = i;
            result[0] += 1;
            result = (unsigned long int*)realloc(result, sizeof(unsigned long int) * (result[0] + 2));
        }
    }

    return 1;
}

int flag_p(unsigned int value) {

    if (value < 2) {
        return -1;
    }

    for (int i = 2; i <= sqrtl(value); i += 1) {
        if (value % i == 0 && i != value) {
            return 0;
        }
    }

    return 1;
}

int flag_s(unsigned int value, unsigned long int* result) {

    if (value == 0 ) {
        result[result[0]+1] = (value % 10);
        result[0] += 1;
        result = (unsigned long int*)realloc(result, sizeof(unsigned long int) * (result[0] + 2));
    }

    while (value > 0) {
        result[result[0]+1] = (value % 10);
        result[0] += 1;
        result = (unsigned long int*)realloc(result, sizeof(unsigned long int) * (result[0] + 2));
        value = (value - (value % 10)) / 10;
    }

    return 1;
}

int flag_e(unsigned int value, unsigned long int* result) {

    if (value > 10) {
        return 0;
    }

    for (int i = 1; i < 11; i += 1) {
        for (int j = 1; j <= value; j += 1) {
            result[result[0]+1] = pow(i, j);
            result[0] += 1;
            result = (unsigned long int*)realloc(result, sizeof(unsigned long int) * (result[0] + 2));
        }
    }

    return 1;
}

int flag_a(unsigned int value) {
    int result = 0;

    if (value < 1) {
        return result;
    }

    for (int i = 1; i <= value; i += 1) {
        result += i;
    }

    return result;
}

int flag_f(unsigned int value) {

    int result = 1;
    for (int i = 2; i <= value; i += 1) {
        result *= i;
    }

    return result;
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        error("Передано неверное количество флагов\n");
        return 0;
    }

    unsigned int value = atoi(argv[2]);
    unsigned long int* result = NULL;
    result = (unsigned long int*)malloc(sizeof(unsigned long int) * 2);
    result[0] = 0;
    int res;

    if (!is_flag(argv[1])) {
        error("argv is_flag");
        return 0;
    }
    char flag = argv[1][1];

    switch (flag) {

        case 'h':

            res = flag_h(value, result);

            if (res) {
                for (int i = 1; i <= result[0]; i += 1) {
                    printf("%ld ", result[i]);
                }
                printf("\n");
            } else {
                printf("Числа отсутствуют\n");
            }
            break;

        case 'p':

            res = flag_p(value);
            if (res == 1) {
                printf("Числo простое\n");
            } else if (!res) {
                printf("Числo составное\n");
            } else {
                printf("Числo не простое и не составное\n");
            }
            break;

        case 's':
            
            res = flag_s(value, result);
            for (int i = result[0]; i != 0; i -= 1) {
                printf("%ld ", result[i]);
            }
            printf("\n");
            break;
        
        case 'e':
            res = flag_e(value, result);

            if (res) {
                for (int i = 1; i <= result[0]; i += 1) {
                    if ((i - 1) % value == 0 && i > 1) {
                        printf("\n");
                    }
                    printf("%ld ", result[i]);
                }
                printf("\n");
            } else {
                printf("Введено число большее 10\n");
            }
            break;

        case 'a':
            res = flag_a(value);
            printf("%d\n", res);
            break;

        case 'f':
            res = flag_f(value);
            printf("%d\n", res);
            break;

        default:
            break;
    }

    free(result);

    return 0;
}
