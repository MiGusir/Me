
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

enum Status{
    SUCCESS,
    INVALID_BASE,
    ZERO_VALUE,
    ERROR
};

void printBinaryRepresentation(int *ptr) {
    unsigned char *bytePtr = (unsigned char *)ptr;

    for (int i = sizeof(int) - 1; i >= 0; --i) {
        for (int j = 7; j >= 0; --j) {
            printf("%d", (bytePtr[i] >> j) & 1);
        }
        printf(" ");
    }
}

enum Status convertToCustomBase(const int number, const int base) {
    if (base < 1 || base > 5) {
        return INVALID_BASE;
    }
    
    if (number == 0) {
        return ZERO_VALUE;
    } 
    else if (number < 0) {
        printf("-");
        convertToCustomBase(-1 * number, base);
        return SUCCESS;
    }

    unsigned char *bytePtr = (unsigned char *)&number;
    if (bytePtr == NULL) {
        return ERROR;
    }
    int bitsRead = 0;
    if (32 % base != 0) {
        bitsRead = base - (32 % base);
    }
    int zeroFlag = 0;
    int digit = 0;
    for (int i = sizeof(int) - 1; i >= 0; --i) {
        for (int j = 7; j >= 0; --j) {
            ++bitsRead;
            digit = digit | (bytePtr[i] >> j & 1) << (base - bitsRead);
            if (bitsRead == base) {
                if (digit != 0 || zeroFlag != 0) {
                    zeroFlag = 1;
                    char out = (digit < 10) ? (char)(digit + '0') : (char)(digit - 10 + 'A');
                    printf("%c", out);
                }
                bitsRead = 0;
                digit = 0;
            }
        }
    }
    printf("\n");
    return SUCCESS;
}

int main() {
    int number = -1000100;
    int r = 5;

    switch (convertToCustomBase(number, r)) {
        case INVALID_BASE:
            printf("Invalid base. The base must be between 1 and 5.\n");
            return 1;
        case ZERO_VALUE:
            printf("0\n");
            break;
        default:
            break;
    }
    return 0;
}