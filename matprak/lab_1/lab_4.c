#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>


#define size_65 65

void flag_d(FILE* from, FILE* to) {
    if (from != NULL) {
        int c;
        while ((c = fgetc(from)) != EOF) {
            if (!isdigit(c)) {
                fputc(c, to);
            }
        }
    }
}

void flag_i(FILE* from, FILE* to) {
    if (from != NULL) {
        int c;
        int q = 0;
        while ((c = fgetc(from)) != EOF) {
            if (isalpha(c)) {
                q += 1;
            }
            if (c == '\n') {
                fputc('0' + q, to);
                q = 0;
                fputc(c, to);
            }
        }
        fputc('0' + q, to);
    }
}

void flag_s(FILE* from, FILE* to) {
    if (from != NULL) {
        int c;
        int q = 0;
        while ((c = fgetc(from)) != EOF) {
            if (isalpha(c) || isdigit(c) || c == ' ') {
            } else {
                q += 1;
            }
            if (c == '\n') {
                fputc('0' + q, to);
                q = 0;
                fputc(c, to);
            }
        }
        fputc('0' + q, to);
    }
}

void flag_a(FILE* from, FILE* to) {
    if (from != NULL) {
        int c;
        while ((c = fgetc(from)) != EOF) {
            if (!isdigit(c)) {
                fprintf(to, "%X", c);
            } else {
                fputc(c, to);
            }
        }
    }
}

// memcpy

void renam(char* name, char* renm) {
    char out[] = "out_";
    int i;
    for (i = 0; name[i] != '\0'; i += 1) {
        renm[i] = name[i];
    }
    while (name[i-1] != '/' && i > 0) {
        i -= 1;
    }
    for (int j = 0; j < 4; j += 1) {
        renm[i] = out[j];
        i += 1;
    }
    for (i; name[i-4] != '\0'; i += 1) {
        renm[i] = name[i-4];
    }
}

static char *CommandNames[] = {
    "-d",
    "-i",
    "-s",
    "-a",
    "-nd",
    "-ni",
    "-ns",
    "-na"
};

enum Command {
    flag_d_enum,
    flag_i_enum,
    flag_s_enum,
    flag_a_enum,
    flag_nd_enum,
    flag_ni_enum,
    flag_ns_enum,
    flag_na_enum
};

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1 += 1;
        str2 += 1;
    }

    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        printf("Передано неверное количество флагов\n");
        return 0;
    }

    for (int i = flag_d_enum; i <= flag_na_enum; i += 1) {
        if (!strcmp(argv[1], CommandNames[i])) { 

            FILE* file_from;
            FILE* file_to;
            FILE* file_to_out;
            char sd[size_65-1] = "";

            if ((file_from = fopen(argv[2], "r+")) == NULL) return 1;

            if (argc == 4) {  
                if ((file_to = fopen(argv[3], "w+")) == NULL) return 1;
            } else {
                renam(argv[2], sd);
                if ((file_to_out = fopen(sd, "w+")) == NULL) return 1;
            }
            
            switch (i) {
                case flag_d_enum:
                    
                    if (argc != 3) {
                        printf("Некорректные аргументы\n");
                        return 0;
                    }
                    flag_d(file_from, file_to_out);
                    break;

                case flag_nd_enum:

                    if (argc != 4) {
                        printf("Некорректные аргументы\n");
                        return 0;
                    }
                    flag_d(file_from, file_to);
                    break;

                case flag_i_enum:

                    if (argc != 3) {
                        printf("Некорректные аргументы\n");
                        return 0;
                    }
                    flag_i(file_from, file_to_out);
                    break;

                case flag_ni_enum:

                    if (argc != 4) {
                        printf("Некорректные аргументы\n");
                        return 0;
                    }
                    flag_i(file_from, file_to);
                    break;
                
                case flag_s_enum:
                    
                    if (argc != 3) {
                        printf("Некорректные аргументы\n");
                        return 0;
                    }
                    flag_s(file_from, file_to_out);
                    break;

                case flag_ns_enum:

                    if (argc != 4) {
                        printf("Некорректные аргументы\n");
                        return 0;
                    }
                    flag_s(file_from, file_to);
                    break;

                case flag_a_enum:

                    if (argc != 3) {
                        printf("Некорректные аргументы\n");
                        return 0;
                    }
                    flag_a(file_from, file_to);
                    break;

                case flag_na_enum:

                    if (argc != 4) {
                        printf("Некорректные аргументы\n");
                        return 0;
                    }
                    flag_a(file_from, file_to);
                    break;

                default:
                    printf("Флаг отсутствует\n");
                    break;

            }

            fclose(file_from);
            if (argc == 4) {
                fclose(file_to);
            } else {
                fclose(file_to_out);
            }
        }
    }


    return 0;
}