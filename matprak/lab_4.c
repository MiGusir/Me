#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>


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
                fputc(48+q, to);
                q = 0;
                fputc(c, to);
            }
        }
        fputc(48+q, to);
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
                fputc(48+q, to);
                q = 0;
                fputc(c, to);
            }
        }
        fputc(48+q, to);
    }
}

void flag_a(FILE* from, FILE* to) {
    if (from != NULL) {
        int c;
        while ((c = fgetc(from)) != EOF) {
            if (!isdigit(c)) {
                char hexNum[10] = "";
                sprintf(hexNum, "%XL", c);
                for (int i = 0; hexNum[i] != 'L'; i += 1) {
                    fputc(hexNum[i], to);
                }
            } else {
                fputc(c, to);
            }
        }
    }
}

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
            FILE* file_from = fopen(argv[2], "r+");
            char sd[64] = "";

            switch (i) {
                case flag_d_enum:

                    renam(argv[2], sd);
                    FILE* file_nd = fopen(sd, "w+");
                    flag_d(file_from, file_nd);
                    fclose(file_nd);
                    break;

                case flag_nd_enum:
                
                    if (argc < 4) {
                        printf("Передано неверное количество флагов\n");
                        return 0;
                    }
                    FILE* file_d = fopen(argv[3], "w+");
                    flag_d(file_from, file_d);
                    fclose(file_d);
                    break;

                case flag_i_enum:

                    renam(argv[2], sd);
                    FILE* file_i = fopen(sd, "w+");
                    flag_i(file_from, file_i);
                    fclose(file_i);
                    break;

                case flag_ni_enum:

                    if (argc < 4) {
                        printf("Передано неверное количество флагов\n");
                        return 0;
                    }
                    FILE* file_ni = fopen(argv[3], "w+");
                    flag_d(file_from, file_ni);
                    fclose(file_ni);
                    break;
                
                case flag_s_enum:

                    renam(argv[2], sd);
                    FILE* file_s = fopen(sd, "w+");
                    flag_s(file_from, file_s);
                    fclose(file_s);
                    break;

                case flag_ns_enum:

                    if (argc < 4) {
                        printf("Передано неверное количество флагов\n");
                        return 0;
                    }
                    FILE* file_ns = fopen(argv[3], "w+");
                    flag_s(file_from, file_ns);
                    fclose(file_ns);
                    break;

                case flag_a_enum:

                    renam(argv[2], sd);
                    FILE* file_a = fopen(sd, "w+");
                    flag_a(file_from, file_a);
                    fclose(file_a);
                    break;

                case flag_na_enum:

                    if (argc < 4) {
                        printf("Передано неверное количество флагов\n");
                        return 0;
                    }
                    FILE* file_na = fopen(argv[3], "w+");
                    flag_a(file_from, file_na);
                    fclose(file_na);
                    break;

                default:
                    printf("Флаг отсутствует\n");
                    break;

            }
            fclose(file_from);
        }
    }


    return 0;
}