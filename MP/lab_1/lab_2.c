#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// gcc lab_1.c -lm && ./a.out 5            после точности 7 считает несколько секунд

int simple(unsigned int value) {

    if (value < 2) {
        return 0;
    }

    for (int i = 2; i <= sqrtl(value); i += 1) {
        if (value % i == 0 && i != value) {
            return 0;
        }
    }

    return 1;
}

long double fact(int a) {
    long double res = 1;
    for (int i = 2; i <= a; i += 1) {
        res *= i;
    }
    return res;
}

long double func_1_1(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;

    while (n) {
        res_midl = powl((1 + (1.0 / n)), n);
        n += 1;
        if (1.0 / n < eps) {
            return res_midl;
        }
        res = res_midl;
    }
}

long double func_1_2(long double eps) {
    long double res = 1.0;
    long double res_midl = 1;
    int n = 1;

    while (res > eps) {
        res /= n;
        res_midl += res;
        n += 1;
    }
    return res_midl;
}

long double func_1_3(long double eps) {
    long double a = 0;
    long double b = 100;
    long double c;
    while (fabsl(b - a) > eps) {
        c = (a + b) / 2.0;
        double f = logl(c);
        
        if (fabsl(f - 1) < eps) {
            return c;
        } else if (f < 1) {
            a = c;
        } else {
            b = c;
        }
    }
    
    return (a + b) / 2.0;
}



long double func_2_1(long double eps) {
    double res = 0.0;
    double res_midl = 1.0;
    int sign = 1;
    int n = 1;
    
    while (fabs(res_midl) >= eps) {
        res += res_midl;
        sign *= -1;
        n += 2;
        res_midl = sign * (1.0 / n);
    }
    
    return res * 4.0;
}

long double func_2_2(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;
    res = powl(-1, n-1) * 1.0 / (2*n -1);

    while (fabsl(res) > eps) {
        res_midl += res;
        n += 1;
        res = powl(-1, n-1) * 1.0 / (2 * n -1);
    }
    return 4 * res_midl;
}

long double func_2_3(long double eps) { // метод Ньютона нужен для приближенного вычисления
    long double x = 2;
    long double x0 = x;

    while (fabsl(cosl(x)+1) > eps) {
        double delta_x = -(cosl(x) + 1) / (-sinl(x) * 1.0);
        
        x += delta_x;
    }
    
    return x;
}



long double func_3_1(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;

    while (n) {
        res_midl = n * (powl(2, 1.0 / n) - 1);
        n += 1;
        if (1.0 / n < eps) {
            return res_midl;
        }
        res = res_midl;
    }
}

long double func_3_2(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;
    int sign = 1;
    res = sign * 1.0 / n;

    while (fabsl(res) > eps) {
        res_midl += res;
        sign *= -1;
        n += 1;
        res = powl(-1, n-1) * 1.0 / n;
    }
    return res_midl;
}

long double func_3_3(long double eps) {
    long double a = 0;
    long double b = 100;
    long double c;
    while (fabsl(b - a) > eps) {
        c = (a + b) / 2.0;
        double f = expl(c);
        
        if (fabsl(f - 2) < eps) {
            return c;
        } else if (f < 2) {
            a = c;
        } else {
            b = c;
        }
    }
    
    return (a + b) / 2.0;
}



long double func_4_1(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;

    while (n) {
        res_midl = res_midl - (powl(res_midl, 2) / 2) + 1;
        n += 1;
        if (fabsl(res_midl - res) < eps) {
            return res_midl;
        }
        res = res_midl;
    }
}

long double func_4_2(long double eps) {
    long double res = 0;
    long double res_midl = 1;
    int n = 2;
    res = powl(2, powl(2, -n));

    while (fabsl(res) > (eps + 1)) {
        res_midl *= res;
        n += 1;
        res = powl(2, powl(2, -n));
    }
    return res_midl;
}

long double func_4_3(long double eps) {
    long double a = 0;
    long double b = 100;
    long double c;
    while (fabsl(b - a) > eps) {
        c = (a + b) / 2.0;
        double f = powl(c, 2);
        
        if (fabsl(f - 2) < eps) {
            return c;
        } else if (f < 2) {
            a = c;
        } else {
            b = c;
        }
    }
    
    return (a + b) / 2.0;
}



long double func_5_1(long double eps) {
    long double res = 0;
    long double res_midl = 0;
    int n = 1;

    while (n) {
        res_midl += 1.0 / n;
        n += 1;

        if (fabsl(res_midl - res) < eps) {
            return res_midl-logl(n);
        }
        res = res_midl;
    }
}

long double func_5_2(long double eps) {
    double sum = 0;
    double prev_sum;
    int n = 1;

    do {
        prev_sum = sum;
        sum += 1.0 / n;
        n += 1;
    } while (fabs(sum - prev_sum) > eps);
    
    return sum - log(n);
}

long double func_5_3(long double eps) {
    long double res = 1;
    int n = 2;
    unsigned long long l = 1;

    while (1.0 / l * exp(1) > eps) {
        l += 1;
    }

    bool* sim = (bool*)malloc(sizeof(bool) * (l+ 1));

    for (int i = 2; i <= l; i += 1) {
        sim[i] = true;
    }

    for (int p = 2; p * p <= l; p += 1) {
        if (sim[p] == true) {
            for (int i = p * p; i <= l; i += p) {
                sim[i] = false;
            }
        }
    }

    for (int i = 2; i <= l; i++) {
        if (sim[i]) {
            res *= (i - 1) * 1.0 / i;
        }
    }
    
    free(sim);
    return -logl(res * logl(l));
}



int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Передано неверное количество флагов\n");
        return 0;
    }
    
    if (atoi(argv[1]) < 1) {
        printf("Недопустимый эпсилон\n");
        return 0;
    }

    long double epsilon = 1.0 / (pow(10, atoi(argv[1])));

    printf("%.*Lf\n", atoi(argv[1]), epsilon);

    printf("\n");

    printf("%.*Lf %.*Lf %.*Lf\n", atoi(argv[1]), func_1_1(epsilon), atoi(argv[1]), 
    func_1_2(epsilon), atoi(argv[1]), func_1_3(epsilon));
    printf("%.*Lf %.*Lf %.*Lf\n", atoi(argv[1]), func_2_1(epsilon), atoi(argv[1]), 
    func_2_2(epsilon), atoi(argv[1]), func_2_3(epsilon));
    printf("%.*Lf %.*Lf %.*Lf\n", atoi(argv[1]), func_3_1(epsilon), atoi(argv[1]), 
    func_3_2(epsilon), atoi(argv[1]), func_3_3(epsilon));
    printf("%.*Lf %.*Lf %.*Lf\n", atoi(argv[1]), func_4_1(epsilon), atoi(argv[1]), 
    func_4_2(epsilon), atoi(argv[1]), func_4_3(epsilon));
    printf("%.*Lf %.*Lf %.*Lf\n", atoi(argv[1]), func_5_1(epsilon), atoi(argv[1]), 
    func_5_2(epsilon), atoi(argv[1]), func_5_3(epsilon));

    return 0;
}
