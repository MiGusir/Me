// #include <stdio.h>
// #include <stdlib.h>
// #include <math.h>
// #include <ctype.h>

// #define size_65 65

// int from_to_10(char* num, int base) {
//     int res = 0;
//     char* ptr = num;
//     if (*ptr == '-') {
//         ptr += 1;
//     }
//     while (*ptr) {
//         if (isdigit(*ptr) || isalpha(*ptr)) {
//             res = res * base + (isdigit(*ptr) ? *ptr - '0' : *ptr - 'A' + 10);
//         }
//         ptr += 1;
//     }
//     return res;
// }

// int main(int argc, char* argv[]) {

//     FILE* file_r;
//     FILE* file_w;

//     if (argc != 3) {
//         printf("Передано неверное количество флагов\n");
//         return 0;
//     }

//     if ((file_r = fopen(argv[1], "r+")) == NULL) {
//         if (file_r != NULL) fclose(file_r);
//         if (file_w != NULL) fclose(file_w);
//         return 1;
//     }
//     if ((file_w = fopen(argv[2], "w+")) == NULL) {
//         if (file_r != NULL) fclose(file_r);
//         if (file_w != NULL) fclose(file_w);
//         return 1;
//     }
    

//     if (file_r != NULL) {
//         int c;
//         int q = 0;
//         int tab = 0;
//         int no_zero = 0;
//         int min_mean = 0;
//         int mean;
//         char str[size_65] = "";


//         // обработка чисел 0000000000 asfad*fsaf -0000213124

//         while ((c = fgetc(file_r)) != EOF) {
//             if ((c == '\n' || c == ' ') && !tab) {
//                 tab = 1;
//                 no_zero = 0;
//                 q = 0;
//                 fprintf(file_w, "%s %d %d\n", str, min_mean+1, from_to_10(str, min_mean+1));
//                 min_mean = 0;
//                 for (int i = 0; i < size_65; i += 1) {
//                     str[i] = '\0';
//                 }
//             } else if (!no_zero && ((isdigit(c) ? c - '0' : c) != 0) && c != '\n' && c != ' ' || no_zero) {
//                 no_zero = 1;
//                 tab = 0;
//                 mean = isdigit(c) ? c - '0' : toupper(c) - 'A' + 10;
//                 if (min_mean < mean) min_mean = mean;
//                 str[q] = isdigit(c) ? c : toupper(c);
//                 q += 1;
//             }
//         }
//         fprintf(file_w, "%s %d %d", str, min_mean+1, from_to_10(str, min_mean+1));
//     }

//     fclose(file_r);
//     fclose(file_w);

//     return 0;
// }

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
        printf("Usage: %s input_file output_file\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    FILE *input = fopen(input_file, "r");
    FILE *output = fopen(output_file, "w");

    if (input == NULL || output == NULL) {
        printf("Failed to open file\n");
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