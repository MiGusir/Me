#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void chanche(int arr[], int size) {
    int min = 0;
    int max = 0;
    
    for (int i = 1; i < size; i += 1) {
        if (arr[i] < arr[min]) {
            min = i;
        }
        if (arr[i] > arr[max]) {
            max = i;
        }
    }
    
    int temp = *(arr + min);
    *(arr + min) = *(arr + max);
    *(arr + max) = temp;
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

    const int x_mas = 12;
    int matrix[x_mas];

    for (int i = 0; i < x_mas; i += 1) {
        matrix[i] = rand() % (atoi(argv[2]) - atoi(argv[1]) + 1) + atoi(argv[1]);
    }

    for (int i = 0; i < x_mas; i += 1) {
        printf("%d ", matrix[i]);
    }
    printf("\n");

    chanche(&matrix[0], x_mas);
    printf("\n");

    for (int i = 0; i < x_mas; i += 1) {
        printf("%d ", matrix[i]);
    }
    printf("\n");

/////////////////////////////////////////

    printf("\n");

    // rand() % (10000 - 10 + 1) + 10;

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