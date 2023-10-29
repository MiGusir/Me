#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>

enum Status {GOOD_ERROR, BAD_MEANS_ERROR, FILE_ERROR};

double average_geometric(int count, ...) {
    va_list args;
    va_start(args, count);

    double product = 1.0;
    for (int i = 0; i < count; i += 1) {
        double num = va_arg(args, double);
        product *= num;
    }

    va_end(args);

    double geometric_mean = powl(product, 1.0 / count);
    return geometric_mean;
}

double pow_recursive(double base, int exponent) {
    if (exponent == 0) {
        return 1;
    } else if (exponent % 2 == 0) {
        double result = pow_recursive(base, exponent / 2);
        return result * result;
    } else {
        double result = pow_recursive(base, (exponent - 1) / 2);
        return base * result * result;
    }
}

int main() {
    double result = average_geometric(4, (double)8.0, (double)18.0, (double)80.0, (double)99900.0);
    printf("Average Geometric: %lf\n", result);
    result = pow_recursive(2.5, 5);
    printf("result: %.2f\n", result);

    return 0;
}