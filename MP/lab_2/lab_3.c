#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

enum Status {GOOD_ERROR, BAD_MEANS_ERROR, FILE_ERROR};

enum Status find_substring(const char* substring, int file_count, ...) {

    va_list files;
    va_start(files, file_count);
    FILE* file;

    for (int i = 0; i < file_count; i++) {
        const char* file_path = va_arg(files, const char*);

        if ((file = fopen(file_path, "r+")) == NULL) {
            return FILE_ERROR;
        }

        char c;
        int line_number = 1;
        int lnb = 1;
        int ib = 0;
        int flag = 0;
        int index = 0;
        int iter = 0;

        while ((c = fgetc(file)) != EOF) {
            index += 1; // из-зи русских символов длина (n * 2) + 1
            if (substring[iter] == '\0') {
                flag = 0;
                printf("File: %s, Line: %d, Position: %d\n", file_path, lnb, ib);
                iter = 0;
            } else if (c != substring[iter]) {
                iter = 0;
                flag = 0;
                if (c == substring[0]) {
                    if (!flag) {
                        lnb = line_number;
                        ib = index;
                        flag = 1;
                    }
                    iter += 1;
                }
            } else {
                if (!flag) {
                    lnb = line_number;
                    ib = index;
                    flag = 1;
                }
                iter += 1;
            }

            if (c == '\n') {
                index = 0;
                line_number += 1; 
            }
        }
        if (substring[iter] == '\0') {
            printf("File: %s, Line: %d, Position: %d\n", file_path, lnb, ib);
        }

        fclose(file);
    }

    va_end(files);
    return GOOD_ERROR;
}

int main() {

    const char* substring = "hello";

    switch (find_substring(substring, 3, "1.txt", "2.txt", "3.txt")) {

        case GOOD_ERROR:
            break;

        case FILE_ERROR:
            printf("Поиск подстроки преостановлен из-за ошибки открытия файла\n");

    }

    return 0;
}