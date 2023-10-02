#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void chanche(int* mas, int y, int x) {
    int max = mas[0];
    int x_max = 0;
    int min = mas[0];
    int x_min = 0;

    for (int i = 0; i < y; i += 1) {
        for (int j = 0; j < x; j += 1) {
            if (mas[i*x+j] < min) {
                min = mas[i*x+j];
                x_min = i*x+j;
            }
            if (mas[i*x+j] > max) {
                max = mas[i*x+j];
                x_max = i*x+j;
            }
        }
    }
    int c = mas[x_max];
    mas[x_max] = mas[x_min];
    mas[x_min] = c;
}

int func_2(int* list, int n, int mean) {
    int res = list[0];
    for (int i = 0; i < n; i += 1) {
        if (abs(mean - list[i]) < abs(res - mean)) {
            res = list[i];
        }
    }
    return res;
}


int main(int argc, char* argv[]) {

    srand(time(0));

    if (argc != 3 || atoi(argv[2]) < atoi(argv[1])) {
        printf("Ошибка переданного диапозона\n");
        return 0;
    }

    const int x_mas = 3;
    const int y_mas = 4;

    int matrix[y_mas][x_mas];

    for (int i = 0; i < y_mas; i += 1) {
        for (int j = 0; j < x_mas; j += 1) {
            matrix[i][j] = rand() % (atoi(argv[2]) - atoi(argv[1]) + 1) + atoi(argv[1]);
        }
    }


    for (int i = 0; i < y_mas; i += 1) {
        for (int j = 0; j < x_mas; j += 1) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    chanche(&matrix[0][0], y_mas, x_mas);
    printf("\n");

    for (int i = 0; i < y_mas; i += 1) {
        for (int j = 0; j < x_mas; j += 1) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
/////////////////////////////////////////
    printf("\n");

    int a = rand() % (30 - 10 + 1) + 10;
    int b = rand() % (30 - 10 + 1) + 10;



    int* mas_a = (int*)malloc(a * sizeof(int*));
    for (int i = 0; i < a; i++) {
        mas_a[i] = rand() % 1000 * pow(-1, rand()%10);
    }

    int* mas_b = (int*)malloc(b * sizeof(int*));
    for (int i = 0; i < b; i++) {
        mas_b[i] = rand() % 1000 * pow(-1, rand()%10);
    }
    
    int* mas_c = (int*)malloc(a * sizeof(int*));
    for (int i = 0; i < a; i++) {
        mas_c[i] = mas_a[i] + func_2(mas_b, b, mas_a[i]);
    }

    for (int i = 0; i < a; i += 1) {
        printf("%d ", mas_a[i]);
    }
    printf("\n");

    for (int i = 0; i < b; i += 1) {
        printf("%d ", mas_b[i]);
    }
    printf("\n");

    for (int i = 0; i < a; i += 1) {
        printf("%d ", mas_c[i]);
    }
    printf("\n");


    free(mas_a);
    free(mas_b);
    free(mas_c);

    return 0;
}