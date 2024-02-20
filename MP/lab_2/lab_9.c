#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

enum Status {GOOD_ERROR, BAD_MEANS_ERROR, FILE_ERROR};

int have_to_base(int base, int count, ...) {
    va_list numbers;
    va_start(numbers, count);

    int flag;

    for (int i = 0; i < count; i += 1) {
        double number = va_arg(numbers, double);
        double n = number;
        
        if (number > 1.0 || number < 0.0) {
            printf("Дробь %lf не входит в диапазон [0, 1]\n", n);
        } else {

            flag = 0;
            for (int i = 0; i < 10; i += 1) {
                double fractional_part = fmod(number, 1.0);
                number *= base;
                
                if (fractional_part == 0 && !flag) {
                    flag = 1;
                    printf("Дробь %lf имеет конечное представление в системе %d\n", n, base);
                }
                if (fractional_part == 1 && !flag) {
                    flag = 1;
                    printf("Дробь %lf не имеет конечное представление в системе %d\n", n, base);
                }
            }

            if (!flag) printf("Дробь %lf не имеет конечное представление в системе %d\n", n, base);
        }

    }

    va_end(numbers);

    return 1;
}

int main() {

    have_to_base(10, 3, 0.000009, 1.0, 0.05);

    return 0;
}