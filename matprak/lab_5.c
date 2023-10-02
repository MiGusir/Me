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

unsigned long long int fact_fact(int a) {
    unsigned long long int res = 1;
    for (int i = 3; i <= a; i += 2) {
        res *= i;
    }
    return res;
}

long double func_a(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 0;
    int x = 3;

    while (n+1) {   
        res = powl(x, n) * 1.0 / fact(n);
        if (fabsl(res) < eps) {
            return res_midl;
        }
        n += 1;
        res_midl += res;
    }
    return res_midl;
}

long double func_b(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 0;
    int x = 3;

    while (n+1) {   
        res = pow(-1, n) * 1.0 * powl(x, 2*n) / fact(2*n);
        if (fabsl(res) < eps) {
            return res_midl;
        }
        n += 1;
        res_midl += res;
    }
    return res_midl;
}

long double func_c(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 0;
    float x = 0.2;

    while (n+1) {   
        res = powl(3, 3*n) * 1.0 * powl(x, 2*n) * powl(fact(n), 3) / fact(3*n);
        if (fabsl(res) < eps) {
            return res_midl;
        }
        n += 1;
        res_midl += res;
    }
    return res_midl;
}

long double func_d(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;
    float x = 0.2;

    while (n) {   
        res = pow(-1, n) * 1.0 * fact_fact(2*n-1) * powl(x, 2*n) / fact_fact(2*n);
        if (fabsl(res) < eps) {
            return res_midl;
        }
        n += 1;
        res_midl += res;
    }
    return res_midl;
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

    printf("%.*Lf\n", eps_n, epsilon);
    printf("%.*Lf\n", eps_n, func_a(epsilon));
    printf("%.*Lf\n", eps_n, func_b(epsilon));
    printf("%.*Lf\n", eps_n, func_c(epsilon));
    printf("%.*Lf\n", eps_n, func_d(epsilon));

    return 0;
}