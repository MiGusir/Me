#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

enum Status {GOOD_ERROR, BAD_MEANS_ERROR, FILE_ERROR};

int strlen1(char* str) {
   int len = 0;
   while (*str != '\0') {
      str += 1;
      len += 1;
   }
   return len;
}

enum Status len_str(char* str, int* len) {
   char* ptr = str;
   while (*ptr != '\0') {
      *len += 1;
      ptr += 1;
   }
   return GOOD_ERROR;
}

enum Status len_revers(char* str, char* rts) {
   rts[strlen1(str)] = '\0';
   for (int i = 0; i != strlen1(str); i += 1) {
      rts[strlen1(str) - i - 1] = str[i];
   }
   return GOOD_ERROR;
}

enum Status len_upper(char* str, char* up_str) {
   up_str[strlen1(str)] = '\0';
   for (int i = 0; i != strlen1(str); i += 1) {
      if (i % 2) {
         up_str[i] = toupper(str[i]);
      } else {
         up_str[i] = str[i];
      }
   }
   return GOOD_ERROR;
}

enum Status len_sortirovka(char* str, char* sortirovka) {
   sortirovka[strlen1(str)] = '\0';
   int len = 0;

   for (int i = 0; i != strlen1(str); i += 1) {
      if (isdigit(str[i])) {
         len += 1;
         sortirovka[len-1] = str[i];
      }
   }

   for (int i = 0; i != strlen1(str); i += 1) {
      if (isalpha(str[i])) {
         len += 1;
         sortirovka[len-1] = str[i];
      }
   }

   for (int i = 0; i != strlen1(str); i += 1) {
      if (!isalpha(str[i]) && !isdigit(str[i])) {
         len += 1;
         sortirovka[len-1] = str[i];
      }
   }

   return GOOD_ERROR;
}

void my_strcat(char* dest, const char *src) {
   while (*dest) {
      dest += 1;
   }

   while (*src) {
      *dest = *src;
      dest += 1;
      src += 1;
      
   }
   *dest = '\0';
}

enum Status len_concut(char** str, char* ret, const int n, int len, int seed) {
   srand(seed);
   int indices[n];
   int index, isDuplicate;
   int ass = rand() % n;

   for (int i = 0; i < n; i += 1) {
      if (ass == i) {
         my_strcat(ret, str[2]);
      } else {
         do {
            index = rand() % (n -1);
            isDuplicate = 0;
            for (int j = 0; j < i + 1; j += 1) {
               if (indices[j] == index) {
                  isDuplicate = 1;
                  break;
               }
            }
         } while (isDuplicate);
         indices[i + 1] = index;
         my_strcat(ret, str[index + 4]);
      }
   }
   
   return GOOD_ERROR;
}

int main(int argc, char *argv[]) {

   if (argc < 3) {
      printf("Неправильное количество аргументов\n");
      return 0;
   }


   if ((argv[1][0] == '-') && (strlen1(argv[1]) == 2)) {

      switch (argv[1][1]) {

      case 'l':

         if (argc != 3) {
            printf("Неправильное количество аргументов\n");
            return 0;
         }

         int len = 0;
         len_str(argv[2], &len);
         printf("%d\n", len);
         break;

      case 'r':

         if (argc != 3) {
            printf("Неправильное количество аргументов\n");
            return 0;
         }

         char* revers;
         if ((revers = (char*)malloc(sizeof(char) * strlen1(argv[2]) + 1)) == NULL) {
            printf("Не смогли выделить память\n");
            return 0;
         }

         len_revers(argv[2], revers);
         printf("%s\n", revers);

         free(revers);
         break;

      case 'u':

         if (argc != 3) {
            printf("Неправильное количество аргументов\n");
            return 0;
         }

         char* upper;
         if ((upper = (char*)malloc(sizeof(char) * strlen1(argv[2]) + 1)) == NULL) {
            printf("Не смогли выделить память\n");
            return 0;
         }

         len_upper(argv[2], upper);
         printf("%s\n", upper);

         free(upper);
         break;

      case 'n':

         if (argc != 3) {
            printf("Неправильное количество аргументов\n");
            return 0;
         }

         char* sortirovka;
         if ((sortirovka = (char*)malloc(sizeof(char) * strlen1(argv[2]) + 1)) == NULL) {
            printf("Не смогли выделить память\n");
            return 0;
         }

         len_sortirovka(argv[2], sortirovka);
         printf("%s\n", sortirovka);

         free(sortirovka);
         break;

      case 'c':

         if (argc < 4) {
            printf("Неправильное количество аргументов\n");
            return 0;
         }

         int len_seed = atoi(argv[3]);
         if (len_seed < 0) {
            len_seed *= -1;
         }

         int l = strlen1(argv[2]);
         int n = 1;
         for (int i = 4; i < argc; i += 1) {
            n += 1;
            l += strlen1(argv[i]);
         }

         char* con;
         if ((con = (char*)malloc(sizeof(char) * l + 1)) == NULL) {
            printf("Не смогли выделить память\n");
            return 0;
         }

         len_concut(argv, con, n, l, len_seed);
         printf("%s\n", con);

         free(con);
         break;
      
      default:
         printf("Флаг отсутствует\n");
         return 0;

      }
   } else {
      printf("Некоректный флаг\n");
      return 0;
   }


   return 0;
}
