#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

int from_to_10(char* num, int base) {
    int res = 0;
    char* ptr = num;
    while (*ptr) {
        if (isdigit(*ptr) || isalpha(*ptr)) {
            res = res*base+(isdigit(*ptr) ? *ptr-'0' : *ptr-'A'+10);
        }
        ptr += 1;
    }
    return res;
}

int main(int argc, char* argv[]) {

    FILE* file_r = fopen(argv[1], "r+");
    FILE* file_w = fopen(argv[2], "w+");

    if (file_r != NULL) {
        int c;
        int q = 0;
        int tab = 0;
        int no_zero = 0;
        int min_mean = 0;
        int mean;
        char str[17] = "";
        char res[16] = "";

        while ((c = fgetc(file_r)) != EOF) {
            if ((c == '\n' || c == ' ') && !tab) {
                tab = 1;
                no_zero = 0;
                q = 0;
                sprintf(res, "%s %d %d", str, min_mean+1, from_to_10(str, min_mean+1));
                fputs(res, file_w);
                min_mean = 0;
                fputc('\n', file_w);
            } else if (!no_zero && ((isdigit(c) ? c-'0' : toupper(c)-'A'+10) != 0) || no_zero) {
                no_zero = 1;
                tab = 0;
                mean = isdigit(c) ? c-'0' : toupper(c)-'A'+10;
                if (min_mean < mean) min_mean = mean;
                str[q] = isdigit(c) ? c : toupper(c);
                q += 1;
            }
        }
        sprintf(res, "%s %d %d", str, min_mean+1, from_to_10(str, min_mean+1));
        fputs(res, file_w);
    }

    fclose(file_r);
    fclose(file_w);

    return 0;
}