#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

#define BUFFER_SIZE 512

typedef enum{
    SUCCESS,
    ERROR,
    INVALID_INPUT,
    END_OF_FILE,
    ERROR_OF_MEMORY,
    ERROR_OF_FILE
} Status;

typedef struct Variable{
    int value;
    struct Variable *next;
} Variable; 

typedef struct{
    int degree;
    Variable *first;
    Variable *last;
} Polynomial; 

void swap(Polynomial **a, Polynomial **b){
    Polynomial **temp = a;
    a = b;
    b = temp;
}

Status CreatePominomial(Polynomial **result){
    Polynomial *polynom = (Polynomial *) malloc(sizeof(Polynomial));
    if (polynom == NULL){
        return ERROR_OF_MEMORY;
    }

    polynom->degree = 0;
    polynom->first = (Variable *) malloc(sizeof(Variable));
    if (polynom->first == NULL){
        free(polynom); 
        return ERROR_OF_MEMORY;
    }

    polynom->last = polynom->first;
    polynom->first->next = NULL;

    *result = polynom;
    return SUCCESS;
}

Variable* SearchDegree(const Polynomial *polynom, int index){
    Variable *ptr = polynom->first;
    int i = 0;
    while (i < index){
        ptr = ptr->next;
        i += 1;
    }
    return ptr;
}

void FreeElements(Variable *current){
    if (current == NULL){ 
        return;
    }

    if (current->next != NULL){
        FreeElements(current->next);
    }

    free(current);
}

Status FreePolynom(Polynomial *polynom){
    if (polynom == NULL){
        return INVALID_INPUT;
    }

    if (polynom->first != NULL){ 
        FreeElements(polynom->first);
    }

    free(polynom);
    polynom = NULL;
    return SUCCESS;
}

void PrintElements(Variable *expression, char variable, int degree){
    if (expression == NULL){
        return;
    }
    printf("%d*%c^%d", expression->value, variable, degree);

    if (expression->next != NULL){
        printf(" + ");
        PrintElements(expression->next, variable, degree - 1);
    }
}

void PrintPolynom(Polynomial *polynom, char variable){
    if (polynom == NULL){
        return;
    }

    printf("Степень текущего полинома равна %d : ", polynom->degree - 1);
    PrintElements(polynom->first, variable, polynom->degree - 1);
    printf("\n");
}

Status CalculatePolynom(const Polynomial *polynom, long long *res, int variable){
    if (polynom == NULL){
        return INVALID_INPUT;
    }

    *res = 0;
    Variable *ptr = polynom->first;
    for (int i = 0; i < polynom->degree; i++){
        if (ptr == NULL){  
            break;
        }
        *res = (*res) * variable;
        *res = (*res) + ptr->value;
        ptr = ptr->next;
    }
    return SUCCESS;
}

Status Diff(Polynomial *polynom){
    if (polynom == NULL){
        return INVALID_INPUT;
    }

    if (polynom->degree - 1 == 0){
        polynom->first->value = 0;
        return SUCCESS;
    }

    Variable *ptr = polynom->first;
    for (int i = 0; i < polynom->degree - 1; i++){
        ptr->value = ptr->value * ((polynom->degree) - i - 1);
        ptr = ptr->next;
    }
    
    Variable *expression = SearchDegree(polynom, polynom->degree - 2);
    FreeElements(ptr);
    (polynom->degree) = (polynom->degree) - 1;
    expression->next = NULL;
    
    return SUCCESS;
}

Status Add(Polynomial *polynom, int k){
    if (polynom->degree == 0){
        polynom->degree = polynom->degree + 1;
        polynom->first->value = k;
        return SUCCESS;
    }

    Variable *ptr = polynom->last;
    ptr->next = (Variable *) malloc(sizeof(Variable));
    if (ptr->next == NULL){
        return ERROR_OF_MEMORY;
    }

    ptr->next->next = NULL;
    polynom->last = ptr->next;
    polynom->last->value = k;

    polynom->degree = (polynom->degree) + 1;

    return SUCCESS;
}

Status Initialization(Polynomial **polynom, int value){
    Status status = CreatePominomial(polynom);
    if (status != SUCCESS){
        return status;
    } 

    status = Add(*polynom, value);
    if (status != SUCCESS){
        FreePolynom(*polynom);
    } 
    return status;
}

Status Increase(Polynomial *polynom, int size){
    if (size < polynom->degree ){
        return INVALID_INPUT;
    }

    while (size > polynom->degree){
        Status status = Add(polynom, 0);
        if (status != SUCCESS){
            return status;
        }
    }
    return SUCCESS;
}

Status Sub(const Polynomial *first, const Polynomial *second, Polynomial **result){
    Polynomial *polynom;
    Status status = CreatePominomial(&polynom);
    if (status != SUCCESS){
        return status;
    } 

    int size = (first->degree > second->degree) ? first->degree : second->degree;
    status = Increase(polynom, size);
    if (status != SUCCESS){
        return status;
    } 

    for (int i = first->degree; i > 0; i--){
        Variable *a = SearchDegree(polynom, size - i);
        Variable *b = SearchDegree(first, first->degree - i);
        a->value = b->value;
    }

    for (int i = second->degree; i > 0; i--){
        Variable *a = SearchDegree(polynom, size - i);
        Variable *b = SearchDegree(second, second->degree - i);
        a->value =  a->value - b->value;
    }

    *result = polynom;
    return SUCCESS;
}

Status Summation(const Polynomial *first, const Polynomial *second, Polynomial **result){
    Polynomial *polynom;
    Status status = CreatePominomial(&polynom);
    if (status != SUCCESS){
        return status;
    } 

    int size = (first->degree > second->degree) ? first->degree : second->degree;
    status = Increase(polynom, size);
    if (status != SUCCESS){
        return status;
    } 

    for (int i = first->degree; i > 0; i--){
        Variable *a = SearchDegree(polynom, size - i);
        Variable *b = SearchDegree(first, first->degree - i);
        a->value = b->value;
    }

    for (int i = second->degree; i > 0; i--){
        Variable *a = SearchDegree(polynom, size - i);
        Variable *b = SearchDegree(second, second->degree - i);
        a->value =  a->value + b->value;
    }

    *result = polynom;
    return SUCCESS;
}

Status Mult(const Polynomial *a, const Polynomial *b, Polynomial **result){
    Polynomial *polynom;
    Status status = CreatePominomial(&polynom);
    if (status != SUCCESS){
        return status;
    }

    int size = a->degree + b->degree - 1;
    status = Increase(polynom, size);
    if (status != SUCCESS){
        return status;
    }

    for (int i = 0; i < a->degree; i++){
        Variable *ai = SearchDegree(a, i);
        for (int j = 0; j < b->degree; j++) {
            Variable *bj = SearchDegree(b, j);
            Variable *aij = SearchDegree(polynom, i + j);
            aij->value += (ai->value * bj->value);
        }
    }

    *result = polynom;
    return SUCCESS;
}

Status Devide(const Polynomial *a, const Polynomial *b, Polynomial **main_part, Polynomial **remainder_part){
    Polynomial *main, *remainder;
    Status status = CreatePominomial(&main);
    if (status != SUCCESS){
        return status; 
    }

    status = CreatePominomial(&remainder);
    if (status != SUCCESS){ 
        FreePolynom(main); 
        return status; 
    }

    int size = a->degree;
    status = Increase(remainder, size);
    if (status != SUCCESS){ 
        FreePolynom(remainder); 
        FreePolynom(main); 
        return status; 
    }

    status = Increase(main, size);
    if (status != SUCCESS){ 
        FreePolynom(remainder); 
        FreePolynom(main); 
        return status; 
    }


    if (a->degree < b->degree){
        return SUCCESS; 
    }

    Variable *ptr = a->first;
    Variable *ptr2 = remainder->first;
    for (int i = 0; i < a->degree; i++){
        ptr2->value = ptr->value;
        ptr = ptr->next;
        ptr2 = ptr2->next;
    }

    ptr = remainder->first;
    ptr2 = main->first;
    
    for (int i = 0; i <= (a->degree - b->degree); i++) {
        if (b->first->value == 0){
            FreePolynom(remainder); 
            FreePolynom(main);
            return INVALID_INPUT;
        }

        int coef = (int)(ptr->value / b->first->value);
        ptr2->value= coef;
        Variable *jPtr = b->first;
        for (int j = 0; j < b->degree; j++) {
            Variable *ijPtr = SearchDegree(remainder, i + j);
            ijPtr->value = ijPtr->value - coef * jPtr->value;
            jPtr = jPtr->next;
        }
        ptr = ptr->next;
        ptr2 = ptr2->next;
    }
    
    ptr = remainder->first;
    Variable *endpoint = remainder->first;
    int degree = 0;
    for (int i = 0; ptr != NULL; ptr = ptr->next){
        if (ptr->value != 0){
            endpoint = ptr;
            degree = i + 1;
        }
        i += 1;
    }

    remainder->degree = degree;
    FreeElements(endpoint->next);
    endpoint->next = NULL;
    if (remainder->degree == 0){
        FreePolynom(remainder);
        status = CreatePominomial(&remainder);
        if (status != SUCCESS){ 
            FreePolynom(main); 
            return status; 
        }

        status = Increase(remainder, 1);
        if (status != SUCCESS){ 
            FreePolynom(main); 
            return status; 
        }
        remainder->degree = 1;
    }
   
    degree = 0;
    ptr = main->first;
    endpoint = main->first;

    for (int i = 0; ptr != NULL; ptr = ptr->next){
        if (ptr->value != 0){
            endpoint = ptr;
            degree = i + 1;
        }
        i += 1;
    }
    
    main->degree = degree; 
    FreeElements(endpoint->next);
     
    endpoint->next = NULL;

    if (main->degree == 0){
        FreePolynom(main);
        status = CreatePominomial(&main);
        if (status != SUCCESS){ 
            FreePolynom(remainder); 
            return status; 
        }

        status = Increase(main, 1);
        if (status != SUCCESS){ 
            FreePolynom(remainder); 
            return status; 
        }
        main->degree = 1;
    }

    *main_part = main;
    *remainder_part = remainder;
    return SUCCESS;
}

Status Div(const Polynomial *a, const Polynomial *b, Polynomial **main_part){
    Polynomial *temp = NULL;
    Status status = Devide(a, b, main_part, &temp);
    if (status != SUCCESS){ 
        return status;
    }

    FreePolynom(temp);
    return SUCCESS;
}

Status Mod(const Polynomial *a, const Polynomial *b, Polynomial **reminder_part){
    Polynomial *temp = NULL;
    Status status = Devide(a, b, &temp, reminder_part);
    if (status != SUCCESS){ 
        return status;
    }

    FreePolynom(temp);
    return SUCCESS;
}

Status Multiply(Polynomial *a, int scalar){
    Variable *ptrA = a->first;
    for (int i = 0; i < a->degree; i++){
        ptrA->value *= scalar;
        ptrA = ptrA->next;
    }

    return SUCCESS;
}

Status Cmps(const Polynomial *A, const Polynomial *B, Polynomial **resulting) {
    Polynomial *result;
    Status status;

    status = CreatePominomial(&result);
    if (status != SUCCESS){
        return status;
    }

    Variable *ptrA = A->first;
    int degree = A->degree - 1;
    while (ptrA != NULL) {
        Polynomial *term = NULL;
        status = Initialization(&term, 1);
        if (status != SUCCESS){
            FreePolynom(result);
            return status;
        }

        while((degree--) > 0){
            status = Mult(term, B, &term);
            if (status != SUCCESS){
                FreePolynom(result);
                FreePolynom(term);
                return status;
            }
        }

        Multiply(term, ptrA->value);
        status = Summation(result, term, &result);
        if (status != SUCCESS){
            FreePolynom(result);
            FreePolynom(term);
            return status;
        }

        FreePolynom(term);
        ptrA = ptrA->next;
    }

    *resulting = result;
    return SUCCESS;
}

char **Split(const char *str, const char *delimiters, int *count){
    int token_count = 0;
    int allocated_size = 2;
    char **tokens = (char **)malloc(sizeof(char *) * allocated_size);
    if (!tokens){
        return NULL;
    }

    char *token = strtok((char *)str, delimiters);
    while (token != NULL) {
        tokens[token_count] = (char *)malloc(strlen(token) + 1);
        if (!tokens[token_count]){
            for (int i = 0; i < token_count; i++){
                free(tokens[i]);
            }
            free(tokens);
            return NULL;
        }

        strcpy(tokens[token_count], token);
        token_count += 1;

        if (token_count >= allocated_size) {
            allocated_size *= 2;
            char **temp = (char **)realloc(tokens, sizeof(char *) * allocated_size);
            if (!temp){
                for (int i = 0; i < token_count - 1; i++){ 
                    free(tokens[i]); 
                }
                free(tokens);
                return NULL;
            }
            tokens = temp;
        }
        token = strtok(NULL, delimiters);
    }

    *count = token_count;
    return tokens;
}

Status GetFilePtr(const char *file_path, FILE **file_result){
    FILE *file = fopen(file_path, "r");
    if (file == NULL){
        return ERROR_OF_FILE;
    }

    *file_result = file;
    return SUCCESS;
}

Status ReadFile(FILE *file, char **line){
    char *command_line = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    if(!command_line){
        return ERROR_OF_MEMORY;
    }

    int buffer_size = BUFFER_SIZE;
    int character_count = 0;

    char current_char;
    bool is_comment = false;
    bool is_big_comment = false;

    while ((current_char = fgetc(file)) != EOF && !(current_char == ';' && !is_comment && !is_big_comment)){
        if (buffer_size - 1 <= character_count){
            char *new_command_line = (char *)realloc(command_line, sizeof(char) * buffer_size * 2);
            if (new_command_line == NULL){
                free(command_line);
                return ERROR_OF_MEMORY;
            }
            command_line = new_command_line;
            buffer_size *= 2;
        }

        if (!is_big_comment && current_char == '%'){
            is_comment = true;
        } 
        if (!is_comment && current_char == '['){
            is_big_comment = true;
        } 
        if (current_char != '\n' && current_char != ' ' && current_char != '\t' && !is_comment && !is_big_comment){
            command_line[character_count] = current_char;
            character_count += 1;
        }
        if (current_char == '\n' && is_comment){
            is_comment = false;
        } 
        if (is_big_comment && current_char == ']'){
            is_big_comment = false;
        } 
    }

    command_line[character_count] = '\0';
    *line = command_line;

    if ((current_char = fgetc(file)) == EOF){
        return END_OF_FILE;
    } 

    return SUCCESS;
}

Status PrebuildPolynom(char *line, int *pos, Polynomial **result, char variable){
    Polynomial *ptr;
    int length = strlen(line);
    int coeff = -1;
    int i = *pos;

    int sign = 1;
    if (line[i] == '+' || line[i] == '-'){
        sign = line[i] == '-' ? -1 : 1;
        i += 1;
    }

    while (i < length && isdigit(line[i]) != 0){
        if (coeff == -1){
            coeff = 0;
        } 
        coeff *= 10;
        coeff += (line[i] - '0');
        i += 1;
    }
    if (coeff == -1){
        coeff = 1;
    } 

    coeff *= sign;
    if (i >= length || line[i] != variable){
        Status status = CreatePominomial(&ptr);
        if (status != SUCCESS){
            return status;
        }

        status = Add(ptr, coeff);
        if (status != SUCCESS){
            FreePolynom(ptr);
            return status;
        }
        *result = ptr;
        *pos = i;
        return SUCCESS;
    }
    i += 1;

    if (i >= length || line[i] != '^'){
        Status status = CreatePominomial(&ptr);
        if (status != SUCCESS){
            return status;
        }

        status = Add(ptr, coeff);
        if (status != SUCCESS){
            FreePolynom(ptr);
            return status;
        }

        status = Add(ptr, 0);
        if (status != SUCCESS){
            FreePolynom(ptr);
            return status;
        }

        *result = ptr;
        *pos = i;
        return SUCCESS;
    }

    i += 1;

    Status status = CreatePominomial(&ptr);
    if (status != SUCCESS){
        return status;
    } 

    int power = -1;
    while (i < length && isdigit(line[i]) != 0){
        if (power == -1){
            power += 1;
        } 
        power *= 10;
        power += (line[i] - '0');
        i += 1;
    }

    status = Add(ptr, coeff);
    if (status != SUCCESS){
        FreePolynom(ptr);
        return status;
    }

    while (power > 0){
        power -= 1;
        status = Add(ptr, 0);
        if (status != SUCCESS){
            return status;
        } 
    }
    *result = ptr;
    *pos = i;
    return SUCCESS;
}

void FreeArray(char **input, int n){
    for (int i = 0; i < n; i++){
        if (input[i] != NULL){
            free(input[i]);
        } 
    }
    free(input);
}

Status BuildPolynom(char *input, int n, Polynomial **a, char variable){
    int l = 0;
    Polynomial *polynom = NULL; 
    Polynomial *current = NULL; 
    Polynomial *temper = NULL;
    Status status = Initialization(&current, 0);
    if (status != SUCCESS){
        return ERROR;
    }

    int length = strlen(input);
    while (l < length){
        status = PrebuildPolynom(input, &l, &polynom, variable);
        if (status != SUCCESS){
            FreePolynom(polynom);
            FreePolynom(current);
            return status;
        }

        status = Summation(current, polynom, &temper);
        if (status != SUCCESS){
            FreePolynom(polynom);
            FreePolynom(current);
            FreePolynom(temper);
            return status;
        }
        FreePolynom(current);
        FreePolynom(polynom);
        polynom = NULL;
        current = temper;
    }
    *a = current;
    return SUCCESS;
}

Status GetLong(char *num, int *res){
    char *endptr;
    long result = strtol(num, &endptr, 10);
    if (*endptr != '\0'){
        return INVALID_INPUT;
    } 

    *res = (int)result;
    return SUCCESS;
}

Status ValidatePolynom(char *s, char variable){
    int len = strlen(s);
    if (len == 0){
        return INVALID_INPUT;
    } 

    int i = 0;
    bool flag = (s[0] == '+' || s[0] == '-') ? true : false;
    bool is_exponent_allowed = true;
    while (i < len){
        if ((s[i] == '+' || s[i] == '-') && flag){
            i += 1;
            flag = false;
            is_exponent_allowed = true;
        }
        else if (s[i] == variable && is_exponent_allowed){
            i += 1;
            if (i < len && s[i] == '^'){
                i += 1;
                if (i < len && isdigit(s[i])){
                    while (i < len && isdigit(s[i])){ 
                        i += 1; 
                    }
                }
                else{
                    return INVALID_INPUT;
                } 
            }
            is_exponent_allowed = false;
            flag = true;
        }
        else if (isdigit(s[i]) && !flag){
            while (i < len && isdigit(s[i])){ 
                i += 1; 
            }
            flag = true;
        }
        else{
            return INVALID_INPUT;
        } 
    }
    return flag ? SUCCESS : INVALID_INPUT;
}

int CountOfTarget(char *str, char target) {
    int count = 0;
    while (*str){
        if (*str == target){
            count += 1;
        } 
        str += 1;
    }
    return count;
}

Status ValidateLine(char *s){
    int size = strlen(s);
    if (s[size - 1] != ')' || CountOfTarget(s, ',') > 1){
        return INVALID_INPUT;
    } 
    for (int i = 0; i < size-1; i++){
        if (!(isdigit(s[i]) == 0) && !(isalpha(s[i]) == 0 ) && s[i] != ',' && s[i] != '('){
            return INVALID_INPUT;
        } 
    }
    return SUCCESS;
}

Status Command(char *line, Polynomial **results, int **Value, char variable, int ** crnt_v){
    int size = strlen(line);
    int cmp1 = CountOfTarget(line, '(');
    int cmp2 = CountOfTarget(line, ')');

    if (strncmp(line, "Mult(", 5) != 0 && strncmp(line, "Mod(", 4) != 0 && strncmp(line, "Div(", 4) != 0 && strncmp(line, "Add(", 4) != 0 && strncmp(line, "Sub(", 4) != 0 && strncmp(line, "Diff(", 5) != 0 && strncmp(line, "Cmps(", 5) != 0 && strncmp(line, "Eval(", 5) != 0){
        return INVALID_INPUT;
    }

    if (cmp1 != cmp2 || cmp1 != 1 || ValidateLine(line) != SUCCESS){
        return INVALID_INPUT;
    } 

    char *delimiter = "()";

    int count;
    char **tokens = Split(line, delimiter, &count);
    if (tokens == NULL){
        return ERROR;
    } 

    if (!(count > 1 && tokens[1] != NULL && strlen(tokens[1]) > 0)){
        for (int i = 0; i < count; i++){
            if (tokens[i] != NULL){
                free(tokens[i]);
            } 
        }
        free(tokens);
        return ERROR;
    }

    char *polys = tokens[1];
    int n;
    char **input = Split(polys, ",", &n);

    for (int i = 0; i < count; i++){
        if (tokens[i] != NULL){
            free(tokens[i]);
        } 
    }
    free(tokens);

    for (int i = 0; i < n; i++){
        Status statust = ValidatePolynom(input[i], variable);
        if (statust != SUCCESS){
            FreeArray(input, n);
            return statust;
        }
    }
    if (strncmp(line, "Mult", 4) == 0 && n > 0){
        Polynomial *pol1 = NULL; 
        Polynomial *pol2 = NULL;
        Status status = BuildPolynom(input[0], n, &pol1, variable);
        if (status != SUCCESS){
            FreePolynom(pol1);
            FreePolynom(pol2);
            FreeArray(input, n);
            return status;
        }
        if (n == 2){
            status = BuildPolynom(input[1], n, &pol2, variable);
            FreePolynom(*results);
            if (status != SUCCESS){
                FreePolynom(pol1);
                FreePolynom(pol2);
                FreeArray(input, n);
                return status;
            }
        }
        else{
            pol2 = *results;
            swap(&pol1, &pol2);
        }

        status = Mult(pol1, pol2, results);
        FreePolynom(pol1);
        FreePolynom(pol2);
        if (status != SUCCESS){
            FreeArray(input, n);
            return status;
        }
    }
    else if (strncmp(line, "Add", 3) == 0 && n > 0){
        Polynomial *pol1 = NULL;
        Polynomial *pol2 = NULL;
        Status status = BuildPolynom(input[0], n, &pol1, variable);
        if (status != SUCCESS){
            FreePolynom(pol1);
            FreePolynom(pol2);
            FreeArray(input, n);
            return status;
        }
        if (n == 2){
            status = BuildPolynom(input[1], n, &pol2, variable);
            FreePolynom(*results);
            if (status != SUCCESS){
                FreePolynom(pol1);
                FreePolynom(pol2);
                FreeArray(input, n);
                return status;
            }
        }
        else{
            pol2 = *results;
            swap(&pol1, &pol2);
        }

        status = Summation(pol1, pol2, results);
        FreePolynom(pol1);
        FreePolynom(pol2);
        if (status != SUCCESS){
            FreeArray(input, n);
            return status;
        }
        
    }
    else if (strncmp(line, "Div", 3) == 0 && n > 0){
        Polynomial *pol1 = NULL;
        Polynomial *pol2 = NULL;
        Status status = BuildPolynom(input[0], n, &pol1, variable);
        if (status != SUCCESS){
            FreePolynom(pol1);
            FreePolynom(pol2);
            FreeArray(input, n);
            return status;
        }
        if (n == 2){
            status = BuildPolynom(input[1], n, &pol2, variable);
            FreePolynom(*results);
            if (status != SUCCESS){
                FreePolynom(pol1);
                FreePolynom(pol2);
                FreeArray(input, n);
                return status;
            }
        }
        else{
            pol2 = *results;
            swap(&pol1, &pol2);
        }

        status = Div(pol1, pol2, results);
        FreePolynom(pol1);
        FreePolynom(pol2);
        if (status != SUCCESS){
            FreeArray(input, n);
            return status;
        }
    }
    else if (strncmp(line, "Mod", 3) == 0 && n > 0){
        Polynomial *pol1 = NULL;
        Polynomial *pol2 = NULL;
        Status status = BuildPolynom(input[0], n, &pol1, variable);
        if (status != SUCCESS){
            FreePolynom(pol1);
            FreePolynom(pol2);
            FreeArray(input, n);
            return status;
        }
        if (n == 2){
            status = BuildPolynom(input[1], n, &pol2, variable);
            FreePolynom(*results);
            if (status != SUCCESS){
                FreePolynom(pol1);
                FreePolynom(pol2);
                FreeArray(input, n);
                return status;
            }
        }
        else{
            pol2 = *results;
            swap(&pol1, &pol2);
        }

        Polynomial * pol3;
        status = Mod(pol1, pol2, &pol3);
        FreePolynom(pol1);
        FreePolynom(pol2);
        *results = pol3;
        if (status != SUCCESS){
            FreeArray(input, n);
            return status;
        }
    }
    else if (strncmp(line, "Sub", 3) == 0 && n > 0){
        Polynomial *pol1 = NULL;
        Polynomial *pol2 = NULL;
        Status status = BuildPolynom(input[0], n, &pol1, variable);
        if (status != SUCCESS){
            FreePolynom(pol1);
            FreePolynom(pol2);
            FreeArray(input, n);
            return status;
        }
        if (n == 2){
            FreePolynom(*results);
            Status status = BuildPolynom(input[1], n, &pol2, variable);
            if (status != SUCCESS){
                FreePolynom(pol1);
                FreePolynom(pol2);
                FreeArray(input, n);
                return status;
            }
        }
        else{
            pol2 = *results;
            swap(&pol1, &pol2);
        }

        status = Sub(pol1, pol2, results);
        FreePolynom(pol1);
        FreePolynom(pol2);
        if (status != SUCCESS){
            FreeArray(input, n);
            return status;
        }
        
    }
    else if (strncmp(line, "Cmps", 4) == 0 && n > 0){
        Polynomial *pol1 = NULL;
        Polynomial *pol2 = NULL;

        Status status = BuildPolynom(input[0], n, &pol1, variable);
        if (status != SUCCESS){
            FreePolynom(pol1);
            FreeArray(input, n);
            return status;
        }
        if (n == 2){
            status = BuildPolynom(input[1], n, &pol2, variable);
            FreePolynom(*results);
            if (status != SUCCESS){
                FreePolynom(pol2);
                FreeArray(input, n);
                return status;
            }
        }
        else{
            pol2 = *results;
            swap(&pol1, &pol2);
        }

        status = Cmps(pol1, pol2, results);
        FreePolynom(pol1);
        FreePolynom(pol2);
        if (status != SUCCESS){
            FreeArray(input, n);
            return status;
        }
    }
    else if (strncmp(line, "Diff", 4) == 0){
        Polynomial *pol;
        if (n == 1){
            Status status = BuildPolynom(input[0], n, &pol, variable);
            FreePolynom(*results);
            if (status != SUCCESS){
                FreePolynom(pol);
                FreeArray(input, n);
                return status;
            }
        }
        else{
            pol = *results;
        } 

        Status status = Diff(pol);
        if (status != SUCCESS){
            FreePolynom(pol);
            FreeArray(input, n);
            return status;
        }
        *results = pol;
    }
    else if (strncmp(line, "Eval", 4) == 0 && n > 0){
        Polynomial *pol;
        int kk = 0;
        if (n == 2){
            FreePolynom(*results);
            Status status = BuildPolynom(input[kk++], n, &pol, variable);
            if (status != SUCCESS){
                FreePolynom(pol);
                FreeArray(input, n);
                return status;
            }
        }
        else{
            pol = *results;
        } 
        long long val;
        int num;
        Status status = GetLong(input[kk], &num);
        if (status != SUCCESS){
            FreeArray(input, n);
            return status;
        }

        status = CalculatePolynom(pol, &val, num);
        if (status != SUCCESS){
            FreePolynom(pol);
            FreeArray(input, n);
            return status;
        }
        
       *Value = (int *) malloc(sizeof(int));
       if (*Value == NULL){
            return ERROR_OF_MEMORY;
        }
        *crnt_v = (int *) malloc(sizeof(int));
       if (*crnt_v == NULL){
            free(*Value); 
            return ERROR_OF_MEMORY;
        }
       *results = pol;
       **Value = val;
       **crnt_v = num;
    }
    else{
        FreeArray(input, n);
        return ERROR;
    }

    FreeArray(input, n);
    return SUCCESS;
}

Status Run(const char * file_name, char variable){
    FILE *file;

    Status status = GetFilePtr(file_name, &file);
    if (status != SUCCESS){
        fclose(file);
        return status;
    }
    
    Polynomial *polynom, *newp;
    status = Initialization(&polynom, 0);
    if (status != SUCCESS){
        fclose(file);
        return status;
    }

    char *command;
    while ((status = ReadFile(file, &command)) == SUCCESS){
        int *temp = NULL;
        int *crnt_variable = NULL;
        status = Command(command, &polynom, &temp, variable, &crnt_variable);
        free(command);
        command = NULL;
        if (status != SUCCESS){
            break;
        } 
        if (temp == NULL){
            PrintPolynom(polynom, variable);
        }
        else{
            printf("Polynomial eval with %c = %d is : %d\n", variable, *crnt_variable, *temp);
            free(temp);
        }
       
    }
    if (command != NULL){
        free(command);
    } 
    FreePolynom(polynom);
    fclose(file);
    return status;
}

void PrintStatus(Status code){
    switch (code){
        case INVALID_INPUT:
            printf("Ошибка ввода\n");
            break;
        case END_OF_FILE:
            printf("Конец файла\n");
            break;
        case ERROR_OF_MEMORY:
            printf("Ошибка памяти\n");
            break;
        case ERROR_OF_FILE:
            printf("Ошибка файла\n");
            break;
        default:
            printf("Неизвестная ошибка\n");
            break;
    }
}

int main(int argc, char * argv[]){
    if (argc != 3){
        printf("неправильный ввод. Напишите <input.txt> и переменную (char).\n");
        return INVALID_INPUT;
    }

    if (strlen(argv[2]) != 1){
        printf("неправильный ввод. Напишите <input.txt> и переменную (char).\n");
        return INVALID_INPUT;
    }

    PrintStatus(Run(argv[1], *(argv[2])));

    return 0;
}