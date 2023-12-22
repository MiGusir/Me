#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int symbol(int c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

int base_to_10(char number[], int base){
    int res = 0;
    char *ptr = number;

    if (*ptr == '-') {
        ptr++;
    }

    while (*ptr){
        if (isdigit(*ptr)) { 
            res = res * base + (*ptr - '0');
        } 
        else {
            res = res * base + (toupper(*ptr) - 'A' + 10);
        }
        ptr++;
    }
    return res;
}

int find_min_base(char* number) {
    int max_digit = 0;
    int len = strlen(number);

    for (int i = 0; i < len; i++) {
        if (symbol(number[i])) {
            if (isdigit(number[i])) {
                int digit = number[i] - '0';
                if (digit > max_digit) {
                    max_digit = digit;
                }
            } else {
                int digit = toupper(number[i]) - 'A' + 10;
                if (digit > max_digit) {
                    max_digit = digit;
                }
            }
        }
    }

    return max_digit + 1;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Передано неверное количество флагов\n");
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    FILE *input = fopen(input_file, "r");
    FILE *output = fopen(output_file, "w");

    if (input == NULL || output == NULL) {
        printf("Ошибка открытия файла\n");
        return 1;
    }

    char buffer[100];
    while (fgets(buffer, sizeof(buffer), input) != NULL) {
        int i, start = -1, end = -1;
        for (i = 0; buffer[i] != '\0'; i++) {
            if (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\n') {
                start = i;
                break;
            }
        }
        if (start == -1) {
            continue;
        }
        for (i = start; buffer[i] != '\0'; i++) {
            if (buffer[i] == ' ' || buffer[i] == '\t' || buffer[i] == '\n') {
                end = i - 1;
                break;
            }
        }
        if (end == -1) {
            end = i - 1;
        }

        char num[50];
        int j;
        for (j = start; j <= end; j++) {
            num[j-start] = buffer[j];
        }
        num[j-start] = '\0';

        int min_base = find_min_base(num);
        int base10 = base_to_10(num, min_base);
        fprintf(output, "%s %d %d\n", num, min_base, base10);
    }

    fclose(input);
    fclose(output);

    return 0;
}