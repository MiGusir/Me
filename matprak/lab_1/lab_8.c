#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define size_65 65

int from_to_10(char* num, int base) {
    int res = 0;
    char* ptr = num;
    if (*ptr == '-') {
        ptr += 1;
    }
    while (*ptr) {
        if (isdigit(*ptr) || isalpha(*ptr)) {
            res = res * base + (isdigit(*ptr) ? *ptr - '0' : *ptr - 'A' + 10);
        }
        ptr += 1;
    }
    return res;
}

int main(int argc, char* argv[]) {

    FILE* file_r;
    FILE* file_w;

    if (argc != 3) {
        printf("Передано неверное количество флагов\n");
        return 0;
    }

    if ((file_r = fopen(argv[1], "r+")) == NULL) return 1;
    if ((file_w = fopen(argv[2], "w+")) == NULL) return 1;
    

    if (file_r != NULL) {
        int c;
        int q = 0;
        int tab = 0;
        int no_zero = 0;
        int min_mean = 0;
        int mean;
        char str[size_65] = "";

        while ((c = fgetc(file_r)) != EOF) {
            if ((c == '\n' || c == ' ') && !tab) {
                tab = 1;
                no_zero = 0;
                q = 0;
                fprintf(file_w, "%s %d %d\n", str, min_mean+1, from_to_10(str, min_mean+1));
                min_mean = 0;
                for (int i = 0; i < size_65; i += 1) {
                    str[i] = '\0';
                }
            } else if (!no_zero && ((isdigit(c) ? c - '0' : toupper(c) - 'A' + 10) != 0) || no_zero) {
                no_zero = 1;
                tab = 0;
                mean = isdigit(c) ? c - '0' : toupper(c) - 'A' + 10;
                if (min_mean < mean) min_mean = mean;
                str[q] = isdigit(c) ? c : toupper(c);
                q += 1;
            }
        }
        fprintf(file_w, "%s %d %d", str, min_mean+1, from_to_10(str, min_mean+1));
    }

    fclose(file_r);
    fclose(file_w);

    return 0;
}