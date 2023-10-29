#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

enum Status {GOOD_ERROR, BAD_MEANS_ERROR, FILE_ERROR};

double find_root(double a, double b, double epsilon, double (*equation)(double)) {
    double c, f;

    while (fabsl(b - a) > epsilon) {
        c = (a + b) / 2.0;
        
        if (equation(c) * equation(a + epsilon) < 0) {
            b = c;
        } else {
            a = c;
        }
    }
    
    return (a + b) / 2.0;
}

double equation(double x) {
    return 10 * x - 25;
}

/*
Метод дихотомии не может быть использован для нахождения корней внутри участков функции, 
где она не является непрерывной или монотонной.
*/

int main() {
    double a = 0.0;
    double b = 18.0;
    double epsilon = 0.000000001;

    double root = find_root(a, b, epsilon, equation);

    printf("Корень уравнения: %lf\n", root);

    return 0;
}