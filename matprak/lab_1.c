#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// gcc lab_1.c -lm && ./a.out -p 12

static char *CommandNames[] = {
    "-h",
    "-p",
    "-s",
    "-e",
    "-a",
    "-f"
};

enum Command {
    flag_h_enum,
    flag_p_enum,
    flag_s_enum,
    flag_e_enum,
    flag_a_enum,
    flag_f_enum
};

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1 += 1;
        str2 += 1;
    }

    return *(unsigned char *)str1 - *(unsigned char *)str2;
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
        printf("Передано неверное количество флагов\n");
        return 0;
    }

    unsigned int value = atoi(argv[2]);
    unsigned long int* result = NULL;
    result = (unsigned long int*)malloc(sizeof(unsigned long int) * 2);
    result[0] = 0;
    int res;

    for (int i = flag_h_enum; i <= flag_f_enum; i += 1) {
        if (!strcmp(argv[1], CommandNames[i])) {
            switch (i) {

                case flag_h_enum:

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

                case flag_p_enum:

                    res = flag_p(value);
                    if (res == 1) {
                        printf("Числo простое\n");
                    } else if (!res) {
                        printf("Числo составное\n");
                    } else {
                        printf("Числo не простое и не составное\n");
                    }
                    break;

                case flag_s_enum:
                    
                    res = flag_s(value, result);
                    for (int i = result[0]; i != 0; i -= 1) {
                        printf("%ld ", result[i]);
                    }
                    printf("\n");
                    break;
                
                case flag_e_enum:
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

                case flag_a_enum:
                    res = flag_a(value);
                    printf("%d\n", res);
                    break;

                case flag_f_enum:
                    res = flag_f(value);
                    printf("%d\n", res);
                    break;

                default:
                    printf("Флаг отсутствует\n");
                    break;

            }
        }
    }

    free(result);

    return 0;
}
