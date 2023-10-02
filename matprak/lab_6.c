#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned long long int fact(int a) {
    unsigned long long int res = 1;
    for (int i = 2; i <= a; i += 1) {
        res *= i;
    }
    return res;
}

long double func_a(long double eps) {
    long double res = 0;

    for (long double x = eps; x <= 1.0; x += eps) {
        res += log(1+x) * 1.0 / x * eps;
    }
    return res;
}

long double func_b(long double eps) {
    long double res = 0;

    for (long double x = 0; x <= 1.0; x += eps) {
        res += powl(expl(1), -powl(x, 2) / 2.0) * eps;
    }
    return res;
}

long double func_c(long double eps) {
    long double res = 0;

    for (long double x = 0; x <= 1.0; x += eps) {
        res += log(1.0 / (1 - x)) * eps;
    }
    return res;
}

long double func_d(long double eps) {
    long double res = 0;

    for (long double x = 0; x <= 1.0; x += eps) {
        res += powl(x, x) * eps;
    }
    return res;
}

int main(int argc, char* argv[]) {

    long double epsilon = atof(argv[1]);
    int eps_n = 0;
    while (epsilon < 1) {
        eps_n += 1;
        epsilon *= 10;
    }
    epsilon = atof(argv[1]);

    if (argc != 2) {
        printf("Передано неверное количество флагов\n");
        return 0;
    }

    printf("%.*Lf\n\n", eps_n, epsilon);
    printf("%.*Lf\n", eps_n, func_a(epsilon));
    printf("%.*Lf\n", eps_n, func_b(epsilon));
    printf("%.*Lf\n", eps_n, func_c(epsilon));
    printf("%.*Lf\n", eps_n, func_d(epsilon));


    return 0;
}