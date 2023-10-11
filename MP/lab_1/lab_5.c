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

long double func_a(long double eps, long double x) {
    long double sum = 1.0;
    long double sum_next = 1.0;
    int n = 1;

    while (sum_next > eps) {
        sum_next *= x / n;
        sum += sum_next;
        n += 1;
    }

    return sum;
}

long double func_b(long double eps, long double x) {
    long double sum = 1.0;
    long double sum_next = 1.0;
    int n = 1;

    while (sum_next > eps) {
        sum_next *= -1.0 * powl(x, 2) / ((2 * n) * (2 * n - 1));
        sum += sum_next;
        n += 1;
    }

    return sum;
}

long double func_c(long double eps, long double x) {
    long double sum = 1.0;
    long double sum_next = 1.0;
    int n = 1;

    while (sum_next > eps) {
        sum_next *= 27 * pow(n, 3) * pow(x, 2) / (n + 2) / (n + 1) / n;
        sum += sum_next;
        n += 1;
    }

    return sum;
}

// неправильно считает c при x = 0.5 y = 2.9
// неправильно считает d при x = 0.5 y = -1...


long double func_d(long double eps, long double x) {
    long double sum = 1.0;
    long double term = 1.0;
    int n = 1;

    while (term > eps) {
        term *= -1 * pow(x, 2) / ((2 * n + 2) * (2 * n + 1));
        sum += term;
        n++;
    }

    return sum;
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Передано неверное количество аргументов\n");
        return 0;
    }
    
    long double epsilon = atof(argv[1]);

    if (epsilon <= 0.0){
        printf("Слишком малый эпсилон\n");
        return 1;
    }

    int eps_n = 0;
    while (epsilon < 1) {
        eps_n += 1;
        epsilon *= 10;
    }
    
    epsilon = atof(argv[1]);
    long double x = 0.5;

    printf("a) %.*Lf\n", eps_n, func_a(epsilon, x));
    printf("b) %.*Lf\n", eps_n, func_b(epsilon, x));
    printf("c) %.*Lf\n", eps_n, func_c(epsilon, x));
    printf("d) %.*Lf\n", eps_n, func_d(epsilon, x));

    return 0;
}