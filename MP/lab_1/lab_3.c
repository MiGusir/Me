#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>

#define ERROUT stdout
#define error(x) fprintf(ERROUT, "%s: fail\n", x)

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
        else if(discriminant == 0){
            res[count] = -b / (2 * a);
            count += 1;
        } else {
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
        error("argv is_flag");
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

            res = flag_q(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), mas);
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

            res = flag_m(atoi(argv[2]), atoi(argv[3]));
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
            if (atof(argv[2]) >= 1) {
                printf("Некорректный эпсилое\n");
                break;
            }

            res = flag_t(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]));
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