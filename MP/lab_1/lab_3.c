#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>

#define is_double(x) strtod(x, NULL)
#define is_int(x) strtol(x, NULL, 10)

int is_flag(char* str) {
    if (strlen(str) != 2) return 0;
    char flags[3] = {'q', 'm', 't'};
    if (str[0] != '-' && str[0] != '/') return 0;
    for (int i = 0; i < 3; i += 1) { if (str[1] == flags[i]) return 1; }
    return 0;
}

int flag_q(float eps, float x, float y, float z, float* res) {
    int count = 0;
    float a, b, c;
    a = x;
    b = y;
    c = z;
    float buf = 0;

    for (int per = 0; per < 6; per += 1) {
        double discriminant = b * b - 4 * a * c;
        if (discriminant < 0){
            
        }
        else if(discriminant == 0 && a > eps){
            res[count] = -b / (2 * a);
            count += 1;
        } else if (a > eps) {
            res[count] = (-b + sqrt(discriminant)) / (2 * a);
            count += 1;
            res[count] = (-b - sqrt(discriminant)) / (2 * a);
            count += 1;
        }
        if (per % 2) {
            if (fabsf(a-b) > eps) {
                buf = a;
                a = b;
                b = buf;
            }
        } else {
            if (fabsf(a-b) > eps) {
                buf = b;
                b = c;
                c = buf;
            }
        }
    }
    return count;
}

int flag_m(int a, int b) {
    if (!a || !b) return 0;
    if (a % b) return 0;
    return 1;
}

int flag_t(float eps, float a, float b, float c) {
    if (a <= 0 || b <= 0 || c <= 0) return 0;
    if (a*a + b*b - c*c <= eps && a*a + b*b - c*c >= -eps) return 1;
    if (a*a + c*c - b*b <= eps && a*a + c*c - b*b >= -eps) return 1;
    if (c*c + b*b - a*a <= eps && c*c + b*b - a*a >= -eps) return 1;
    return 0;
}

int main(int argc, char *argv[]) {
    
    if (!is_flag(argv[1])) {
        printf("Неверный флаг\n");
        return 0;
    }
    char flag = argv[1][1];
    
    if (argc < 2) {
        printf("Передано неверное количество флагов\n");
        return 0;
    }

    int res;
    float mas[12];

    switch (flag) {

        case 'q': {

            if (argc != 6) {
                printf("Передано неверное количество флагов\n");
                break;
            }
            // if (not (is_double(argv[2]) && is_double(argv[3]) && is_double(argv[4]) && is_double(argv[5]))) {
            //     printf("Переданы не валидные аргументы\n");
            //     break;
            // }

            res = flag_q(is_double(argv[2]), is_double(argv[3]), is_double(argv[4]), is_double(argv[5]), mas);

            for(int i = 0; i < res; i += 1) {
                printf("%f ", mas[i]);
            }
            if (res) printf("\n");
            if (!res) printf("Решений нет\n");
            break;
        }

        case 'm': {

            if (argc != 4) {
                printf("Передано неверное количество флагов\n");
                break;
            }
            // if (not (is_int(argv[2]) && is_int(argv[3]))) {
            //     printf("Переданы не валидные аргументы\n");
            //     break;
            // }

            res = flag_m(is_int(argv[2]), is_int(argv[3]));

            if (res) {
                printf("Кратно\n");
            } else {
                printf("Не кратно\n");
            }
            break;
        }

        case 't': {

            if (argc != 6) {
                printf("Передано неверное количество флагов\n");
                break;
            }
            // if (not (is_double(argv[2]) && is_double(argv[3]) && is_double(argv[4]) && is_double(argv[5]))) {
            //     printf("Переданы не валидные аргументы\n");
            //     break;
            // }

            res = flag_t(is_double(argv[2]), is_double(argv[3]), is_double(argv[4]), is_double(argv[5]));

            if (res) {
                printf("Могут\n");
            } else {
                printf("Не могут\n");
            }
            break;
        }
    }

    return 0;
}