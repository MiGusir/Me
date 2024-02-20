#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int size;
    int MaxIndex;
    int MaxValue;
    int MinIndex;
    int MinValue;
    int Popular;
    double Average;
    int MaxDIfference;
} Statistic;


enum Status{
    SUCCESS = 0,
    INVALID_INPUT = -1,
    ERROR_OF_FILE = -2,
    OVERFLOW = -3,
    ARRAY_IS_EMPTY = -4,
    ERROR = -5
};

int Load(int array[], int* size, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL){
        return ERROR_OF_FILE;
    }

    *size = 0;
    int res;
    while (*size < BUFSIZ) {
        res = fscanf(file, "%d", &array[*size]);
        if (res == 1) {
            (*size) += 1;
        } 
        else if (res == 0) {
            fclose(file);
            return ERROR_OF_FILE;
        } 
        else {
            fclose(file);
            return ERROR;
        }
    }

    if (fscanf(file, "%d", &res) == 1) {
        return OVERFLOW;
    }

    fclose(file);

    return SUCCESS;
}

int Free(int array[], int* size) {
    for (int i = 0; i < *size; i++) {
        array[i] = 0;
    }
    *size = 0;

    return SUCCESS;
}

int Save(const int array[], int size, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        return ERROR_OF_FILE;
    }

    if (size == 0) {
        return ARRAY_IS_EMPTY;
    }

    for (int i = 0; i < size; i++) {
        fprintf(file, "%d ", array[i]);
    }

    fclose(file);

    return SUCCESS;
}

int Rand(int array[], int size, int left, int right) {
    if (size == 0) {
        return ARRAY_IS_EMPTY;
    }

    for (int i = 0; i < size; i++) {
        array[i] = rand() % (right - left + 1) + left;
    }
    
    return SUCCESS;
}

int Concat(int arr1[], int* len1, const int arr2[], int len2) {
    if (*len1 + len2 >= BUFSIZ) {
        return OVERFLOW;
    }

    for (int i = 0; i < len2; i++) {
        arr1[*len1 + i] = arr2[i];
    }
    
    *len1 += len2;

    if (*len1 == 0) {
        return ARRAY_IS_EMPTY;
    }

    return SUCCESS;
}

int Remove(int array[], int* size, int index, int number) {
    if (size == 0) {
        return ERROR;
    }

    if (*size - number == 0) {
        return ARRAY_IS_EMPTY;
    }

    if (index < 0 || index >= *size || number <= 0) {
        return INVALID_INPUT;
    }

    for (int i = index; i < *size - number; i++) {
        array[i] = array[i + number];
    }

    *size -= number;

    return SUCCESS;
}

int Copy(const int arr1[], int len1, int arr2[], int* len2, int from, int to) {
    if (len1 == 0) {
        return ARRAY_IS_EMPTY;
    }

    int size = to - from + 1;

    if (from < 0 || from >= len1 || to < from || to >= len1) {
        printf("Invalid copy parameters\n");
        return ERROR;
    }

    for (int i = 0; i < size; i++) {
        arr2[i] = arr1[from + i];
    }
    *len2 = size;

    return SUCCESS;
}

int CompareShuffle(){
    return rand() % 3 - 1; 
}

int Shuffle(int array[], int size) {
    if (size == 0) {
        return ARRAY_IS_EMPTY;
    }

    srand(time(NULL));
    qsort(array, size, sizeof(int), CompareShuffle);
    return SUCCESS;
}

int CompareAscending(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int CompareDescending(const void* a, const void* b){ 
    return (*(int*)b - *(int*)a);
}

Statistic Stats(const int array[], int size) {
    Statistic stats;

    if (size == 0) {
        stats.size = 0;
        stats.MaxIndex = -1;
        stats.MinValue = 0;
        stats.MinIndex = -1;
        stats.MaxValue = 0;
        stats.Popular = 0;
        stats.Average = 0.0;
        stats.MaxDIfference = 0;
        printf("There is no such array, so\n");
        return stats;
    }

    stats.size = size;
    stats.MinValue = array[0];
    stats.MinIndex = 0;
    stats.MaxIndex = 0;
    stats.MaxValue = array[0];
    stats.Popular = array[0];
    stats.Average = (double)array[0];
    stats.MaxDIfference = 0;

    int sum = array[0], deviation, maxCount = 1, currCount = 1, prev = array[0];
    for (int i = 1; i < size; i++) {
        if (array[i] > stats.MaxValue) {
            stats.MaxValue = array[i];
            stats.MaxIndex = i;
        } 
        else if (array[i] < stats.MinValue) {
            stats.MinValue = array[i];
            stats.MinIndex = i;
        }
        sum += array[i];

        deviation = abs(array[i] - sum / (i + 1));

        if (deviation > stats.MaxDIfference) {
            stats.MaxDIfference = deviation;
        }

        if (array[i] == array[i - 1]) {
            currCount += 1;
        } 
        else {
            currCount = 1;
        }

        if (currCount > maxCount || (currCount == maxCount && array[i] > prev)) {
            maxCount = currCount;
            stats.Popular = array[i];
            prev = array[i];
        }

        stats.Average = (double)sum / (i + 1);
    }

    return stats;
}

int Print(const int array[], int size, int index) {
    if (size == 0) {
        return ARRAY_IS_EMPTY;
    }

    if (index >= 0 && index < size) {
        printf("Element at position %d: %d\n", index, array[index]);
    } 
    else {
        return INVALID_INPUT;
    }
    
    return SUCCESS;
}

int PrintRange(const int array[], int size, int start, int end) {
    if (size == 0) {
        return ARRAY_IS_EMPTY;
    }

    if (start >= 0 && start < size && end >= start && end < size) {
        for (int i = start; i <= end; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");
    } 
    else {
        return INVALID_INPUT;
    }
    
    return SUCCESS;
}

int PrintAll(const int array[], int size);

int PrintAll(const int array[], int size) {
    if (size == 0) {
        return ARRAY_IS_EMPTY;
    }

    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    
    printf("\n");
    return SUCCESS;
}

void PrintErrors(int error) {
    switch (error) {
        case INVALID_INPUT:
            printf("\nWrong input\n");
            break;
        case OVERFLOW:
            printf("\nOverflow\n");
            break;
        case ERROR_OF_FILE:
            printf("\nError of file\n");
            break;
        case ARRAY_IS_EMPTY:
            printf("\nArray is empty\n");
            break;
        default:
            break;
    }
}

void menu(){
    printf("Commands\n\n");
    printf("Load <name of array>, <name of file>;\n");
    printf("Save <name of array>, <name of file>;\n");
    printf("Rand <name of array>, <number of elements>, <left border>, <right border>;\n");
    printf("Concat <name of array>, <name of array>;\n");
    printf("Free (<name of array>);\n");
    printf("Remove <name of array>, <left border>, <number of elements>;\n");
    printf("Copy <name of array from>, <left border>, <right border>, <name of array to>;\n");
    printf("Sort <name of aray>+;\n");
    printf("Sort <name of aray>-;\n");
    printf("Shuffle <name of array>;\n");
    printf("Stats <name of array>;\n");
    printf("Print <name of array>, <index>;\n");
    printf("Print <name of array>, <index from>, <index to>;\n");
    printf("Print <name of array>, all;\n");
    printf("Menu\n");
    printf("Exit\n\n");
}

int main() {
    int arrays[26][BUFSIZ];
    int arraySizes[26] = {0};

    char command[256];
    char filename[256];
    char arr1, arr2;
    int count, left, right, pos;
    menu();
    while (true) {
        printf("Enter command or write Menu: ");
        fgets(command, 256, stdin);

        int status = 0;
        if (sscanf(command, "Load %c, %255[^;];\n", &arr1, filename) == 2 && isalpha(arr1) && strchr(command, ';')) {
            if ((status = Load(arrays[toupper(arr1) - 'A'], &arraySizes[toupper(arr1) - 'A'], filename)) != SUCCESS) {
                PrintErrors(status);
            }
            printf("Array %c loaded from %s\n\n", arr1, filename);
        } 
        else if (sscanf(command, "Save %c, %19[^;];", &arr1, filename) == 2 && isalpha(arr1) && strchr(command, ';')) {
            Save(arrays[toupper(arr1) - 'A'], arraySizes[toupper(arr1) - 'A'], filename);
            printf("Array %c saved to %s\n\n", arr1, filename);
        } 
        else if (sscanf(command, "Rand %c, %d, %d, %d;", &arr1, &count, &left, &right) == 4 && isalpha(arr1) && strchr(command, ';')) {
            Rand(arrays[toupper(arr1) - 'A'], count, left, right);
            arraySizes[toupper(arr1) - 'A'] = count;
            printf("Array %c filled with %d random elements between %d and %d\n\n", arr1, count, left, right);
        } 
        else if (sscanf(command, "Concat %c, %c;", &arr1, &arr2) == 2 && isalpha(arr1) && isalpha(arr2) && strchr(command, ';')) {
            Concat(arrays[toupper(arr1) - 'A'], &arraySizes[toupper(arr1) - 'A'], arrays[arr2 - 'A'], arraySizes[arr2 - 'A']);
            printf("Arrays %c and %c concatenated\n\n", arr1, arr2);
        } 
        else if (sscanf(command, "Free (%c);", &arr1) == 1 && isalpha(arr1) && strchr(command, ';') && strchr(command, ';')) {
            Free(arrays[toupper(arr1) - 'A'], &arraySizes[toupper(arr1) - 'A']);
            printf("Array %c freed\n\n", arr1);
        } 
        else if (sscanf(command, "Remove %c, %d, %d;", &arr1, &left, &right) == 3 && isalpha(arr1) && strchr(command, ';')) {
            Remove(arrays[toupper(arr1) - 'A'], &arraySizes[toupper(arr1) - 'A'], left, right);
            printf("%d elements removed from array %c from index %d\n\n", right, arr1, left);
        } 
        else if (sscanf(command, "Copy %c, %d, %d, %c;", &arr1, &left, &right, &arr2) == 4 && isalpha(arr1) && strchr(command, ';')) {
            Copy(arrays[toupper(arr1) - 'A'], arraySizes[toupper(arr1) - 'A'], arrays[arr2 - 'A'], &arraySizes[arr2 - 'A'], left, right);
            printf("Elements copied from array %c to %c\n\n", arr1, arr2);
        } 
        else if (sscanf(command, "Sort %c+;", &arr1) == 1 && isalpha(arr1) && strchr(command, ';')) {
            if (arraySizes[toupper(arr1) - 'A'] == 0) {
                PrintErrors(ARRAY_IS_EMPTY);
            }
            else {
                qsort(arrays[toupper(arr1) - 'A'], arraySizes[toupper(arr1) - 'A'], sizeof(int), CompareAscending);
                printf("Array %c sorted\n\n", arr1);
            }
        } 
        else if (sscanf(command, "sort %c-;", &arr1) == 1 && isalpha(arr1) && strchr(command, ';')) {
            if (arraySizes[toupper(arr1) - 'A'] == 0) {
                PrintErrors(ARRAY_IS_EMPTY);
            }
            else {
                qsort(arrays[toupper(arr1) - 'A'], arraySizes[toupper(arr1) - 'A'], sizeof(int), CompareDescending);
                printf("Array %c sorted\n\n", arr1);
            }
        } 
        else if (sscanf(command, "Shuffle %c;", &arr1) == 1 && isalpha(arr1) && strchr(command, ';')) {
            if ((status = Shuffle(arrays[toupper(arr1) - 'A'], arraySizes[toupper(arr1) - 'A'])) != SUCCESS) {
                PrintErrors(status);
            } 
            else{
                printf("Array %c shuffled\n\n", arr1);
            }
        } 
        else if (sscanf(command, "Stats %c;", &arr1) == 1 && isalpha(arr1) && strchr(command, ';')) {
            Statistic stats = Stats(arrays[toupper(arr1) - 'A'], arraySizes[toupper(arr1) - 'A']);
            printf("Array %c statistics:\n", arr1);
            printf("Size of the array: %d\n", stats.size);
            printf("Max element: %d, index: %d\n", stats.MaxValue, stats.MaxIndex);
            printf("Min element: %d, index: %d\n", stats.MinValue, stats.MinIndex);
            printf("Most frequent element: %d\n", stats.Popular);
            printf("Average: %.2f\n", stats.Average);
            printf("Max difference with average: %d\n", stats.MaxDIfference);
            printf("\n\n");
        } 
        else if (sscanf(command, "Print %c, %d;", &arr1, &pos) == 2 && isalpha(arr1) && strchr(command, ';')) {
            if ((status = Print(arrays[toupper(arr1) - 'A'], arraySizes[toupper(arr1) - 'A'], pos)) != SUCCESS) {
                PrintErrors(status);
            }
        }
        else if (sscanf(command, "Print %c, %d, %d;", &arr1, &left, &right) == 3 && isalpha(arr1) && strchr(command, ';')) {
            if ((status = PrintRange(arrays[toupper(arr1) - 'A'], arraySizes[toupper(arr1) - 'A'], left, right)) != SUCCESS) {
                PrintErrors(status);
            }
        }
        else if (sscanf(command, "Print %c, all;", &arr1) == 1 && isalpha(arr1) && strchr(command, ';')) {
            if ((status = PrintAll(arrays[toupper(arr1) - 'A'], arraySizes[toupper(arr1) - 'A'])) != SUCCESS) {
                PrintErrors(status);
            }
        }
        else if (strcmp(command, "Menu\n") == 0) {
            menu();
        } 
        else if (strcmp(command, "Exit\n") == 0) {
            break;
        } 
        else {
            printf("Invalid command\n\n");
        }
    }

    return 0;
}