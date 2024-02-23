#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>

typedef struct {
   double *coordinates;
   int dimension;
} Vector;


int norm_8(Vector *vector, double *result) {
   if (vector->dimension <= 0) {
      return 0;
   }

   double max = vector->coordinates[0];
   for (int i = 1; i < vector->dimension; i += 1) {
      if (vector->coordinates[i] > max) {
         max = vector->coordinates[i];
      }
   }
   *result = max;
   return 1;
}

int norm_p(Vector *vector, int p, double *result) {
   if (p < 1 || vector->dimension <= 0) {
      return 0;
   }

   double sum = 0;
   for (int i = 0; i < vector->dimension; i += 1) {
      sum += pow(fabs(vector->coordinates[i]), p);
   }
   *result = pow(sum, 1.0 / p);
   return 1;
}

int norm_a(Vector *vector, double *result) {
   if (vector->dimension <= 0) {
      return 0;
   }

   int matrix[vector->dimension][vector->dimension];
   double data[vector->dimension];

   for (int i = 0; i < vector->dimension; i += 1) {
      for (int j = 0; j < vector->dimension; j += 1) {
         if (i == j) {
            matrix[i][j] = 1;
         } else {
            matrix[i][j] = 0;
         }
      }
   }

   for (int i = 0; i < vector->dimension; i += 1) {
      double sum = 0;
      for (int j = 0; j < vector->dimension; j += 1) {
         sum += matrix[i][j] * vector->coordinates[j];
      }
      data[i] = sum;
   }

   double value = 0;
   for (int i = 0; i < vector->dimension; i += 1) {
      value += data[i] * data[i];
   }

   *result = sqrt(value);
   return 1;
}

int find_vectors(double *result, Vector *vector, int p, int (*norm1)(Vector *, double *), int (*norm2)(Vector *, int, double *), int (*norm3)(Vector *, double *), int count, ...) {
   va_list normFunctions;
   va_start(normFunctions, count);

   double max_vals[3] = {INT_MIN, INT_MIN, INT_MIN};
   int numLongestVectors[3] = {0};

   for (int i = 0; i < count; i++) {
      Vector v = va_arg(normFunctions, Vector);
      double res[3];

      if (norm1(&v, &(res[0])) != 1 || norm2(&v, p, &(res[1])) != 1 || norm3(&v, &(res[2])) != 1) {
         return 0;
      }

      for (int j = 0; j < 3; j++) {
         if (res[j] > max_vals[j]) {
            max_vals[j] = res[j];
            result[j] = max_vals[j];
            numLongestVectors[j] = 1;
            vector[j].coordinates = v.coordinates;
            vector[j].dimension = v.dimension;
         } else if (res[j] == max_vals[j]) {
            numLongestVectors[j]++;
         }
      }
   }

   va_end(normFunctions);
   return 1;
}

int main() {
   int n = 3;
   int p = 1;

   double vec1_data[] = {1.5, -2.0, 3.0};
   double vec2_data[] = {-1.0, 0.7, 1.0};
   double vec3_data[] = {3.0, 2.12, 2.0};

   Vector vec1 = {vec1_data, n};
   Vector vec2 = {vec2_data, n};
   Vector vec3 = {vec3_data, n};

   int (*norm1)(Vector *, double *) = norm_8;
   int (*norm2)(Vector *, int, double *) = norm_p;
   int(*norm3)(Vector *, double *) = norm_a;

   double result[3];
   Vector longest_vectors[3];
   int numLongestVectors[3];

   int status = find_vectors(result, longest_vectors, p, norm1, norm2, norm3, 3, vec1, vec2, vec3);

   if (status == 1) {
      for (int i = 0; i < 3; i += 1) {
         printf("Самый длинный вектор %d: (", i + 1);
         for (int j = 0; j < longest_vectors[i].dimension; j++) {
            printf("%f", longest_vectors[i].coordinates[j]);
            if (j < longest_vectors[i].dimension - 1) {
               printf(", ");
            }
         }
         printf(")\n");
      }
   } else {
      printf("Ошибка при поиске векторов\n");
   }

   return 0;
}