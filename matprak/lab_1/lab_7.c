#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define size_128 128

static char *CommandNames[] = {
    "-r",
    "-a"
};

enum Command {
    flag_r_enum,
    flag_a_enum
};

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1 += 1;
        str2 += 1;
    }

    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

int from_10_to(int num, int base, char* res) {
    int n = num;
    int r;
    char* ptr = res + 32;
    for (int i = 0; i < 33; i += 1) {
        *(ptr-i) = 0;
    }
    *ptr --= 0;
    while (n > 0) {
        r = n % base;
        *ptr--=(r>9) ? r-10+'A' : r+'0';
        n /= base;
    }
    return 1;
}

int main(int argc, char* argv[]) {

    if (argc < 4) {
        printf("Передано неверное количество флагов\n");
        return 0;
    }

    for (int i = flag_r_enum; i <= flag_a_enum; i += 1) {
        if (!strcmp(argv[1], CommandNames[i])) { 
            
            FILE* file_from_1;
            FILE* file_from_2;
            FILE* file_to_w;

            if (argc == 5) {  
                if ((file_from_1 = fopen(argv[2], "r+")) == NULL) return 1;
                if ((file_from_2 = fopen(argv[3], "r+")) == NULL) return 1;
                if ((file_to_w = fopen(argv[4], "w+")) == NULL) return 1;
            } else {
                if ((file_from_1 = fopen(argv[2], "r+")) == NULL) return 1;
                if ((file_to_w = fopen(argv[3], "w+")) == NULL) return 1;
            }

            switch (i) {
                case flag_r_enum:

                    if (argc != 5) {
                        printf("Передано неверное количество флагов\n");
                        return 0;
                    }

                    if (file_from_1 != NULL && file_from_2 != NULL) {
                        int c_1;
                        int c_2;
                        int q_1 = 0;
                        int tab = 0;
                        char res_1[size_128] = "";
                        char res_2[size_128] = "";

                        while ((c_1 = fgetc(file_from_1)) != EOF) {
                            if ((c_1 == '\n' || c_1 == ' ') && !tab) {
                                tab = 1;
                                res_1[q_1] = ' ';
                                q_1 += 1;
                            } else if (c_1 != '\n' && c_1 != ' ') {
                                tab = 0;
                                res_1[q_1] = c_1;
                                q_1 += 1;
                            }
                        }

                        int q_2 = 0;
                        tab = 0;

                        while ((c_2 = fgetc(file_from_2)) != EOF) {
                            if ((c_2 == '\n' || c_2 == ' ') && !tab) {
                                tab = 1;
                                res_2[q_2] = ' ';
                                q_2 += 1;
                            } else if (c_2 != '\n' && c_2 != ' ') {
                                tab = 0;
                                res_2[q_2] = c_2;
                                q_2 += 1;
                            }
                        }

                        // printf("%s\n", res_1);
                        // printf("%s\n", res_2);
                        
                        int iter_1 = 0;
                        int iter_2 = 0;

                        while (iter_1 < q_1 || iter_2 < q_2) {
                            while (res_1[iter_1] != ' ' && iter_1 < q_1) {
                                fputc(res_1[iter_1], file_to_w);
                                iter_1 += 1;
                            }
                            if (iter_1 <= q_1) fputc(' ', file_to_w);
                            iter_1 += 1;

                            while (res_2[iter_2] != ' ' && iter_2 < q_2) {
                                fputc(res_2[iter_2], file_to_w);
                                iter_2 += 1;
                            }
                            if (iter_2 <= q_2) fputc(' ', file_to_w);
                            iter_2 += 1;
                        }
                    }
                    break;

                case flag_a_enum:

                    if (argc != 4) {
                        printf("Передано неверное количество флагов\n");
                        return 0;
                    }

                    if (file_from_1 != NULL) {
                        int c;
                        int q = 0;
                        int tab = 0;
                        char trans[33] = "";
                        int iter = 0;
                        int lex = 1;

                        while ((c = fgetc(file_from_1)) != EOF) {
                            if ((c == '\n' || c == ' ') && !tab) {
                                tab = 1;
                                lex += 1;
                                fputc(' ', file_to_w);
                            } else if (c != '\n' && c != ' ') {
                                tab = 0;
                                if (lex % 10 == 0) {
                                    from_10_to(tolower(c), 4, trans);
                                    for (int i = 0; i < 33; i += 1) {
                                        if (isdigit(trans[i])) {
                                            fputc(trans[i], file_to_w);
                                        }
                                    }
                                } else if (lex % 2 == 0) {
                                    fputc(tolower(c), file_to_w);
                                } else if (lex % 5 == 0) {
                                    from_10_to(c, 8, trans);
                                    for (int i = 0; i < 33; i += 1) {
                                        if (isdigit(trans[i])) {
                                            fputc(trans[i], file_to_w);
                                        }
                                    }
                                } else {
                                    fputc(c, file_to_w);
                                }
                            }
                        }
                    }
                    break;

                default:
                    printf("Флаг отсутствует\n");
                    break;
            }

            fclose(file_to_w);
            fclose(file_from_1);
            if (argc == 5) {
                fclose(file_from_2);
            }
            
        }
    }
    return 0;
}