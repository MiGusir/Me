#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// gcc lab_1.c -lm && ./a.out 5            после точности 7 считает несколько секунд

long double fact(int a) {
    long double res = 1;
    for (int i = 2; i <= a; i += 1) {
        res *= i;
    }
    return res;
}

long double func_1_1(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;

    while (n) {
        res_midl = powl((1 + (1.0 / n)), n);
        n += 1;
        if (fabsl(res_midl - res) < eps) {
            return res_midl;
        }
        res = res_midl;
    }
}

long double func_2_1(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;

    while (n < 482) { // макстмум итераций без переполнения
        res_midl = powl(powl(2, n) * fact(n), 4) * 1.0 / n / powl(fact(2*n), 2);
        n += 1;
        if (fabsl(res_midl - res) < eps) {
            return res_midl;
        }
        res = res_midl;
    }
    return res_midl;
}

long double func_3_1(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;

    while (n) {
        res_midl = n * (powl(2, 1.0 / n) - 1);
        n += 1;
        if (fabsl(res_midl - res) < eps) {
            return res_midl;
        }
        res = res_midl;
    }
}

long double func_4_1(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;

    while (n) {
        res_midl = res_midl - (powl(res_midl, 2) / 2) + 1;
        n += 1;
        if (fabsl(res_midl - res) < eps) {
            return res_midl;
        }
        res = res_midl;
    }
}

long double func_5_1(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;

    while (n) {
        res_midl += 1.0 / n;
        n += 1;

        if (fabsl(res_midl - res) < eps) {
            return res_midl-logl(n);
        }
        res = res_midl;
    }
}

long double func_1_2(long double eps) {
    long double res = 1;
    long double res_midl = 1;
    int n = 1;

    while (res > eps) {
        res = 1.0 / fact(n);
        res_midl += res;
        n += 1;
    }
    return res_midl;
}

long double func_2_2(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;
    res = powl(-1, n-1) * 1.0 / (2*n -1);

    while (fabsl(res) > eps) {
        res_midl += res;
        n += 1;
        res = powl(-1, n-1) * 1.0 / (2*n -1);
    }
    return 4 * res_midl;
}

long double func_3_2(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;
    res = powl(-1, n-1) * 1.0 / n;

    while (fabsl(res) > eps) {
        res_midl += res;
        n += 1;
        res = powl(-1, n-1) * 1.0 / n;
    }
    return res_midl;
}

long double func_4_2(long double eps) {
    long double res = 0;
    long double res_midl = 1;
    int n = 2;
    res = powl(2, powl(2, -n));

    while (fabsl(res) > (eps + 1)) {
        res_midl *= res;
        n += 1;
        res = powl(2, powl(2, -n));
    }
    return res_midl;
}

long double func_5_2(long double eps) {
    return func_5_1(eps);
}

long double func_1_3(long double eps) {
    long double res = 0;

    while (fabsl(log(res) - 1) > eps) {
        res += eps;
    }
    return res;
}

long double func_2_3(long double eps) {
    long double res = 0;

    while (fabsl(cos(res) + 1) > eps) {
        res += eps;
    }
    return res;
}

long double func_3_3(long double eps) {
    long double res = 0;

    while (fabsl(exp(res) - 2) > eps) {
        res += eps;
    }
    return res;
}

long double func_4_3(long double eps) {
    long double res = eps;

    while (powl(res, 2) - 2 < eps) {
        res += eps;
    }
    return res;
}

long double func_5_3(long double eps) {
    long double res = 0;
    long double limit = 1;
    int n = 2;

    while (fabsl(limit - limit * (1-1.0/n)) > eps) {
        limit *= 1 - 1.0 / n;
        n += 1;
    }
    limit *= logl(n);

    while (fabsl(exp(-res) - limit) > eps) {
        res += eps;
    }
    return res;
}


int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Передано неверное количество флагов\n");
        return 0;
    }

    long double epsilon = 1.0 / (pow(10, atoi(argv[1])));

    printf("%.*Lf\n", atoi(argv[1]), epsilon);

    printf("\n");

    printf("%.*Lf\n", atoi(argv[1]), func_1_1(epsilon));
    printf("%.*Lf\n", atoi(argv[1]), func_2_1(epsilon));
    printf("%.*Lf\n", atoi(argv[1]), func_3_1(epsilon));
    printf("%.*Lf\n", atoi(argv[1]), func_4_1(epsilon));
    printf("%.*Lf\n", atoi(argv[1]), func_5_1(epsilon));

    printf("\n");

    printf("%.*Lf\n", atoi(argv[1]), func_1_2(epsilon));
    printf("%.*Lf\n", atoi(argv[1]), func_2_2(epsilon));
    printf("%.*Lf\n", atoi(argv[1]), func_3_2(epsilon));
    printf("%.*Lf\n", atoi(argv[1]), func_4_2(epsilon));
    printf("%.*Lf\n", atoi(argv[1]), func_5_2(epsilon));

    printf("\n");

    printf("%.*Lf\n", atoi(argv[1]), func_1_3(epsilon));
    printf("%.*Lf\n", atoi(argv[1]), func_2_3(epsilon));
    printf("%.*Lf\n", atoi(argv[1]), func_3_3(epsilon));
    printf("%.*Lf\n", atoi(argv[1]), func_4_3(epsilon));
    printf("%.*Lf\n", atoi(argv[1]), func_5_3(epsilon)); // что-то тут не так

    return 0;
}
