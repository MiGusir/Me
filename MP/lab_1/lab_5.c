#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int is_digit(char *number) {
    if (!strcmp(number, "-")) return 0;
    if (*number == '-') number += 1;
    while (*number != '\0') {
        if (!(*number >= '0' && *number <= '9')) return 0;
        number += 1;
    }
    return 1;
}

int is_double(char* number) {
    int c = 0;
    if (strcmp(number, "-") == 0) return 0;
    while (*number != '\0') {
        if (*number == '.') {
            if (c == 0) c += 1;
            else return 0;
        }
        else if (!(*number >= '0' && *number <= '9')) return 0;
        number += 1;
    }
    return 1;
}

double func_a(double eps, double x) {
    double n = 1.0;
    double sum = 1.0 + x, element = x;
    do {
        element *= x / (n + 1.0);
        sum += element;
        n += 1;
    } while (fabs(element) > eps);
    return sum;
}

double func_b(double eps, double x) {
    double n = 1.0;
    double sum = 1.0 - ((x * x) / 2.0), element = sum - 1.0;
    do {
        element *= ((-1.0) * x * x) / ((2.0 * n + 2.0) * (2.0 * n + 1.0));
        sum += element;
        n += 1;
    } while (fabs(element) > eps);
    return sum;
}


double func_c(double eps, double x) {
    double n = 1.0;
    double sum = 1.0 + ((9.0 * x * x) / 2.0), element = ((9.0 * x * x) / 2.0);
    do {
        element *= (9.0 * (n + 1.0) * (n + 1.0) * x * x) / ((3.0 * n + 2.0) * (3.0 * n + 1.0));
        sum += element;
        n += 1;
    } while (fabs(element) > eps);
    return sum;
}

double func_d(double eps, double x) {
    double n = 1.0;
    double sum = ((-1.0) * x * x) / 2.0, element = sum;
    do {
        element *= ((-1.0) * (2.0 * n + 1.0) * x * x) / (2.0 * n + 2.0);
        sum += element;
        n += 1;
    } while (fabs(element) > eps);
    return sum;
}

int is_epsilon(char* eps) {
    if (is_double(eps)) {
        float result = atof(eps);
        if (result > 0 && result <= 1) return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Передано неверное количество аргументов\n");
        return 0;
    }
    
    if (!is_epsilon(argv[1])) {
        printf("Невалидный эпсилон\n");
        return 0;
    }

    long double epsilon = atof(argv[1]);

    int eps_n = 0;
    while (epsilon < 1) {
        eps_n += 1;
        epsilon *= 10;
    }
    
    epsilon = atof(argv[1]);
    long double x = 0.5;

    printf("a) %.*f\n", eps_n, func_a(epsilon, x));
    printf("b) %.*f\n", eps_n, func_b(epsilon, x));
    printf("c) %.*f\n", eps_n, func_c(epsilon, x));
    printf("d) %.*f\n", eps_n, func_d(epsilon, x));

    return 0;
}