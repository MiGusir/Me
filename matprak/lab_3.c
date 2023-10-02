#include <stdio.h>
#include <stdlib.h>
#include <math.h>


static char *CommandNames[] = {
    "-q",
    "-m",
    "-t"
};

enum Command {
    flag_q_enum,
    flag_m_enum,
    flag_t_enum
};

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1 += 1;
        str2 += 1;
    }

    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

int flag_q(double eps, double a, double b, double c, double* list) {
    double cof[3] = {a, b, c};
    if (a+b+c<eps) return 0;
    int n = 0;

    for (int i = 0; i != 3; i += 1) {
        double discriminant = cof[i%3] * cof[i%3] - 4 * cof[(i+1)%3] * cof[(i+2)%3];

        if (discriminant > eps) {
            list[n] = (-cof[i%3] + sqrt(discriminant)) / (2 * cof[(i+1)%3]);
            list[n+1] = (--cof[i%3] - sqrt(discriminant)) / (2 * cof[(i+1)%3]);
            n += 2;
        } else if (discriminant < eps && discriminant > -eps) {
            list[n] = -cof[i%3] / (2 * cof[(i+1)%3]);
            n += 1;
        }
    }

    return n;
}

int flag_m(int a, int b) {
    if (!a || !b) return 0;
    if (a % b) return 0;
    return 1;
}

int flag_t(double eps, double a, double b, double c) {
    if (a*a + b*b - c*c < eps && a*a + b*b - c*c > -eps) return 1;
    if (a*a + c*c - b*b < eps && a*a + c*c - b*b > -eps) return 1;
    if (c*c + b*b - a*a < eps && c*c + b*b - a*a > -eps) return 1;
    return 0;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Передано неверное количество флагов\n");
        return 0;
    }

    int res;
    double mas[6];

    for (int i = flag_q_enum; i <= flag_t_enum; i += 1) {
        if (!strcmp(argv[1], CommandNames[i])) {
            switch (i) {
                case flag_q_enum:

                    if (argc != 6) {
                        printf("Передано неверное количество флагов\n");
                        break;
                    }

                    res = flag_q(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), mas);
                    for(int i = 0; i < res; i += 1) {
                        printf("%lf ", mas[i]);
                    }
                    if (res) printf("\n");
                    if (!res) printf("Решений нет\n");
                    break;

                case flag_m_enum:

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

                case flag_t_enum:

                    if (argc != 6) {
                        printf("Передано неверное количество флагов\n");
                        break;
                    }

                    res = flag_t(atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]));
                    if (res) {
                        printf("Могут\n");
                    } else {
                        printf("Не могут\n");
                    }
                    break;

                default:
                    printf("Флаг отсутствует\n");
                    break;
            }
        }
    }

    return 0;
}