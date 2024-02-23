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

int get_size(const char *string) {
   char const *start = string;
   while (*string != '\0')
   {
      string++;
   }
   return string - start;
}

enum Status len_concut(char **result, char *prev_string, char* prev_arguments[], int size, unsigned int seed) {
   srand(seed);
   int length = 0;
   int new_length = 0;
   int string_length;
   *result = (char*)malloc(sizeof(char) * length);
   if (*result == NULL) return FILE_ERROR;

   char** arguments = (char**)malloc(size * sizeof(char*) + 1);
   if (arguments == NULL) return FILE_ERROR;

   arguments[0] = prev_string;
   for (int i = 1; i <= size; ++i)
   {
      arguments[i] = prev_arguments[i - 1];
   }
   size++;

   char *string;
   char *temp;
   int a = 0, b = size - 1;

   int index;

   for (int i = 0; i < size - 1; ++i)
   {
      index = rand() % (size - i) + i;
      temp = arguments[index];
      arguments[index] = arguments[i];
      arguments[i] = temp;
   }
   for (int i = 0; i < size; ++i)
   {
      string_length = get_size(arguments[i]);
      new_length = length + string_length;
      temp = (char*)realloc(*result, sizeof(char) * (new_length + 1));
      if (temp == NULL)
      {
         free(*result);
         *result = NULL;
         return FILE_ERROR;
      }
      *result = temp;
      for (int j = length, k = 0; j < new_length, k < string_length; ++j, ++k)
      {
         (*result)[j] = arguments[i][k];
      }
      (*result)[new_length] = '\0';
      length = new_length;
   }
   free(arguments);
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

         char *seed_string;
         int seed;
         char *new_string;
         int result;
         char *string = argv[2];

         seed = atoi(argv[3]);
         new_string = NULL;
         result = len_concut(&new_string, string, argv + 4, argc - 4, seed);
         if (result == FILE_ERROR) {
            printf("Не смогли выделить память\n");
            free(new_string);
            return result;
         }
         printf("%s\n", new_string);
         free(new_string);
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
