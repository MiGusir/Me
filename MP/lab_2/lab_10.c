#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

enum Status {GOOD_ERROR, BAD_MEANS_ERROR, FILE_ERROR};

int factorial(int n) {
    if (n <= 1)
        return 1;
    else
        return n * factorial(n - 1);
}

int binomialCoefficient(int n, int k) {
    if (k > n)
        return 0;

    int numerator = factorial(n);
    int denominator = factorial(k) * factorial(n - k);

    return numerator / denominator;
}

void polynomialExpansion(double epsilon, double a, double **result, int n, ...) {

    if ((*result = (double*) malloc((n + 1) * sizeof(double))) == NULL) {
        return;
    }
    
    for (int i = 0; i <= n; i += 1) {
        (*result)[i] = 0.0;
    }
    
    va_list args;
    va_start(args, n);

    for (int i = 0; i <= n; i += 1) {
        double coeff = va_arg(args, double);
        double c = coeff - (*result)[i];

        for (int j = i; j <= n; j += 1) {
            if (j == i)
                (*result)[j] = c;
            else
                (*result)[j] += binomialCoefficient(n - i, j - i) * pow(-a, j-i) * c;
        }
    }
    
    va_end(args);
}

int main() {

    double epsilon = 0.001;
    double a = 2.0;
    double *result;

    polynomialExpansion(epsilon, a, &result, 3, 4.0, 0.0, 1.0, 0.0); // 4x^3 + 2x^2 + x + 4
 
    printf("Разложенный многочлен:\n");
    for (int i = 0; i <= 3; i += 1) {
        printf("g%d = %.4f\n", 3 - i, result[i]);
    }
    
    free(result);

    return 0;
}