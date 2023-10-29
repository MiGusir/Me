#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

enum Status {GOOD_ERROR, BAD_MEANS_ERROR, FILE_ERROR};

typedef struct {
    double x;
    double y;
} Point;

int sign(double x) {
    if (x > 0) {
        return 1;
    } else if (x < 0) {
        return -1;
    } else {
        return 0;
    }
}

int orientation(Point p1, Point p2, Point p3) {
    double vector1_x = p2.x - p1.x;
    double vector1_y = p2.y - p1.y;
    double vector2_x = p3.x - p2.x;
    double vector2_y = p3.y - p2.y;

    return sign(vector1_x * vector2_y - vector1_y * vector2_x); // определитель матрицы
}

int is_polygon_convex(int vertex_count, ...) {
    va_list vertices;
    va_start(vertices, vertex_count);
    
    Point p1 = va_arg(vertices, Point);
    Point p2 = va_arg(vertices, Point);
    Point p3 = va_arg(vertices, Point);
    
    int prevOrientation = orientation(p1, p2, p3);
    for (int i = 0; i < vertex_count -1; i += 1) {

        int currOrientation = orientation(p1, p2, p3);
        p1 = p2;
        p2 = p3;
        p3 = va_arg(vertices, Point);
        
        if (currOrientation != prevOrientation) {
            return 0;
        }
        
        prevOrientation = currOrientation;
    }
    
    va_end(vertices);
    return 1;
}

double evaluate_polynomial(double x, int n, ...) {
    va_list coefficients;
    double result = 0.0;

    va_start(coefficients, n);

    for (int i = 0; i <= n; i += 1) {
        result *= x;
        double coefficient = va_arg(coefficients, double);
        result += coefficient;
    }

    va_end(coefficients);

    return result;
}

int main() {
    Point p1 = {0.0, -2.0};
    Point p2 = {5.0, 1.4};
    Point p3 = {0.0, 4.7};
    Point p4 = {0.0, 50.0};
    Point p5 = {1.0, 3.0};

    int result = is_polygon_convex(5, p1, p2, p3, p4, p5);
    if (result) {
        printf("Является.\n");
    } else {
        printf("Не является.\n");
    }

    double x = 2.0;
    int degree = 3;
    double coefficient3 = 1.0;
    double coefficient2 = 1.0;
    double coefficient1 = 1.0;
    double coefficient0 = 1.0;

    double res = evaluate_polynomial(x, degree, coefficient3, coefficient2, coefficient1, coefficient0);

    printf("Значение многочлена в точке: %lf\n", res);
    
    return 0;
}