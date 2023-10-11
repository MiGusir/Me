#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void xor8(FILE *file) {
    int byte, result = 0;

    while ((byte = fgetc(file)) != EOF) {
        result ^= byte;
    }

    printf("Result: %02X\n", result);
}

void xor32(FILE *file) {
    int dword;
    int result = 0;

    while (fread(&dword, sizeof(int), 1, file) == 1) {
        result ^= dword;
    }

    printf("Result: %08X\n", result);
}

void mask(FILE *file, int maska) {
    int dword;
    int count = 0;

    while (fread(&dword, sizeof(int), 1, file) == 1) {
        if ((dword & maska) == maska) {
            count++;
        }
    }

    printf("Count: %d\n", count);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Wrong number of arguments. Usage: %s <filename> <flag>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    char *flag = argv[2];

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return 2;
    }

    if (strcmp(flag, "xor8") == 0) {
        xor8(file);
    } 
    else if (strcmp(flag, "xor32") == 0) {
        xor32(file);
    } 
    else if (strcmp(flag, "mask") == 0) {
        if (argc < 4) {
            printf("Mask value is missing.\n");
            fclose(file);
            return 3;
        }

        int maska = strtol(argv[3], NULL, 16);
        mask(file, maska);
    } 
    else {
        printf("Wrong flag.\n");
    }

    fclose(file);

    return 0;
}