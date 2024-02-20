#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

enum Status{
    SUCCESS = 0,
    WRONG_INPUT = -1,
    ERROR_OF_FILE = -2,
    ERROR_OF_MEMORY = -3,
    ERROR = -4
};

int VariousRoman(char symbol){
    switch(symbol){
        case 'I': return 1;
        case 'V': return 5;
        case 'X': return 10;
        case 'L': return 50;
        case 'C': return 100;
        case 'D': return 500;
        case 'M': return 1000;
    }
    return 0;
}

int FromRoman(char * string){
    int result = 0;
    int value;
    for (int i = 0; string[i] != '\0'; i++){
        value = VariousRoman(string[i]);
        if (value < VariousRoman(string[i + 1])){
            result -= value;
        } 
        else{
            result += value;
        } 
    }
    return result;
}

unsigned int FromZeckendorf(char * string){
    int length = strlen(string);
    unsigned int result = 0;
    unsigned int current = 1, prev = 1, next = 1;
    for (int i = 0; i < length - 1; i++){
        current = next;
        if (string[i] == '1'){
            result += current;
        } 
		next = current + prev;
        prev = current;
    }
    return result;
}

int ToDecimal(char* number, int base){
    int result = 0;
    int flag = 0;
    char * ptr = number - 1;
    if (*(++ptr) == '-'){
        flag = 1;
    } 
    else{
        --ptr;
    } 

    while (*(++ptr)){
        result = result * base + (isdigit(*ptr) ? *ptr - '0' : tolower(*ptr) - 'a' + 10);
    }
    if (flag == 1){
        result *= -1;
    } 
    return result;
}

int CheckFlag(char * flag){
    char * flags[] = {"Ro", "Zr", "Cv", "CV"};
    int size = sizeof(flags) / sizeof(flags[0]);

    for (int i = 0; i < size; i++){
        if (!strcmp(flag, flags[i])){
            return SUCCESS;
        } 
    }
    return ERROR;
}

int GetString(FILE * file, char ** string){
    int length = 2;
    *string = (char*)malloc(length * sizeof(char));
    if (*string == NULL){
        return ERROR_OF_MEMORY;
    } 
    char * temp;
    int count = 0;
    char symbol = fgetc(file);

    while (symbol == ' ' || symbol == '\n' || symbol == ',' || symbol == '\t'){
        symbol = fgetc(file);
    } 

    while (symbol != ' ' && symbol != '\n' && symbol != ',' && symbol != '\t' && symbol != EOF){
        if (length <= count){
            length *= 2;
            temp = (char*)realloc(*string, length * sizeof(char));
            if (temp == NULL){
                free(string);
                string = NULL;
                return ERROR_OF_MEMORY;
            }
            *string = temp;
        }
        (*string)[count] = symbol;
        count += 1;
        symbol = fgetc(file);
    }
    (*string)[count] = '\0';
    return SUCCESS;
}

int Flags(FILE * file, char * flag, va_list * args){
    int num, base;
    unsigned int num2;
    int * res;
    unsigned int * res2;
    unsigned int zeckendorf;
    char * string = NULL;
    num = GetString(file, &string);
    if (num == ERROR_OF_MEMORY){
        return num;
    } 
    switch(flag[0]){
        case 'R':
            res = va_arg(*args, int*);
            num = FromRoman(string);
            *res = num;
            break;
        case 'Z':
            res2 = va_arg(*args, unsigned int*);
            num2 = FromZeckendorf(string);
            *res2 = num2;
            break;
        case 'C':
            res = va_arg(*args, int*);
            base = va_arg(*args, int);
            if (!(base >= 2 && base <= 36)){
                base = 10;
            } 
            num = ToDecimal(string, base);
            *res = num;
            break;
        default:
            break;
    }
    free(string);
    string = NULL;
    return SUCCESS;
}

int oversscanf(char * buffer, const char * format, ...){
    int len = strlen(buffer);
    if (len == 0){
        return 0;
    } 

    FILE * copy = tmpfile();
    if (!copy){
        return ERROR_OF_FILE;
    } 
    int i = 0;
    while (buffer[i] != '\0'){
        fputc(buffer[i], copy);
        i += 1;
    }
    rewind(copy);

    va_list arguments;
    va_start(arguments, format);
    int length = strlen(format);
    char * string = (char*)format;
    char * temp = (char*)malloc((length + 1) * sizeof(char));
    if (temp == NULL){
        va_end(arguments);
        return ERROR_OF_MEMORY;
    }
    for (int i = 0; i <= length; i++){
        temp[i] = 0;
    } 
    char current_flag[] = {'1', '1', '\0'};
    int count = 0;
    int temp_count = 0;
    int result;
    void * empty_flag;
    for (int i = 0; i < length; i++){
        if (format[i] == '%'){
            current_flag[0] = format[i + 1];
            current_flag[1] = format[i + 2];
            if (CheckFlag(current_flag) == SUCCESS){
                if (temp_count != 0){
                    temp[temp_count] = 0;
                    result = vfscanf(copy, temp, arguments);
                    if (result == EOF){
                        free(temp);
                        temp = NULL;
                        return ERROR;
                    }
                    count += result;
                    for (int j = 0; j < result; j++){
                        empty_flag = va_arg(arguments, void*);
                    } 
                }
                result = Flags(copy, current_flag, &arguments);
                count += 1;
                if (result == ERROR_OF_MEMORY){
                    free(temp);
                    temp = NULL;
                    return result;
                }

                for (int j = 0; j <= length; ++j){
                    temp[j] = 0;
                } 
                temp_count = 0;
                i += 3;
            }
        }
        temp[temp_count] = format[i];
        temp_count += 1;
    }
    if (temp_count != 0){
        temp[temp_count] = 0;
        result = vfscanf(copy, temp, arguments);
        if (result < 0){
            free(temp);
            temp = NULL;
            return ERROR;
        }
        count += result;
    }
    free(temp);
    temp = NULL;

    fclose(copy);
    remove("tmpfile");

    va_end(arguments);
    return count;
}

int overfscanf(FILE * file, const char * format, ...){
    va_list arguments;
    va_start(arguments, format);
    int length = strlen(format);
    char * string = (char*)format;
    char * temp = (char*)malloc((length + 1) * sizeof(char));
    if (temp == NULL){
        va_end(arguments);
        return ERROR_OF_MEMORY;
    }
    for (int i = 0; i <= length; i++){
        temp[i] = 0;
    } 
    char flag[] = {'1', '1', '\0'};
    int count = 0;
    int temp_count = 0;
    int result;
    void * empty_flag;
    for (int i = 0; i < length; i++){
        if (format[i] == '%'){
            flag[0] = format[i + 1];
            flag[1] = format[i + 2];
            if (CheckFlag(flag) == SUCCESS){
                if (temp_count != 0){
                    temp[temp_count] = 0;
                    result = vfscanf(file, temp, arguments);
                    if (result == EOF){
                        free(temp);
                        temp = NULL;
                        return ERROR;
                    }
                    count += result;
                    for (int j = 0; j < result; j++){
                        empty_flag = va_arg(arguments, void*);
                    } 
                }
                result = Flags(file, flag, &arguments);
                count += 1;
                if (result == ERROR_OF_MEMORY){
                    free(temp);
                    temp = NULL;
                    return result;
                }

                for (int j = 0; j <= length; j++){
                    temp[j] = 0;
                } 
                temp_count = 0;
                i += 3;
            }
        }
        temp[temp_count] = format[i];
        temp_count += 1;
    }
    if (temp_count != 0){
        temp[temp_count] = 0;
        result = vfscanf(file, temp, arguments);
        if (result < 0){
            free(temp);
            temp = NULL;
            return ERROR;
        }
        count += result;
    }
    free(temp);
    temp = NULL;

    va_end(arguments);
    return count;
}

int main(int argc, char * argv[]){
    if (argc != 2){
        printf("Invalid number of arguments\n");
        return WRONG_INPUT;
    }
    int a = 0;
    int b = overfscanf(stdin, "");
    printf("%d %d\n", b, a);
    char* input_file = argv[1];
    FILE * file = fopen(input_file, "r");
    if(file == NULL){
        printf("Error of openning file.\n");
        return ERROR_OF_FILE;
    }
    int num_int;
    double num_double;
    unsigned int num_unsigned;
    int rom;
    int num1;
    int num2;
    
    char string[10];
    int result = overfscanf(file, "%d %lf %s %Ro %Zr %CV %Cv", &num_int, &num_double, &string, &rom, &num_unsigned, &num1, 11 , &num2, 2);
    if (result == ERROR){
        printf("Error.\n");
        fclose(file);
        return result;
    }
    if (result == ERROR_OF_MEMORY){
        printf("Error of memory.\n");
        fclose(file);
        return result;
    }
    printf("overfscanf\n");
    printf("result: %d\nint: %d\ndouble: %lf\nstring: %s\nRo: %d\nZr: %d\nCV: %d\nCv: %d\n", result, num_int, num_double, string, rom, num_unsigned, num1, num2);
    printf("\n");
    
    char * buf = "101 3.14 abcd XVI 10010011 B 1001";
    result = oversscanf(buf, "%d %lf %s %Ro %Zr %CV %Cv", &num_int, &num_double, &string, &rom, &num_unsigned, &num1, 16 , &num2, 2);
    if (result == ERROR_OF_FILE){
        printf("Error.\n");
        fclose(file);
        return result;
    }
    if (result == ERROR_OF_MEMORY){
        printf("Error.\n");
        fclose(file);
        return result;
    }
    printf("oversscanf\n");
    printf("result: %d\nint: %d\ndouble: %lf\nstring: %s\nRo: %d\nZr: %d\nCV: %d\nCv: %d\n", result, num_int, num_double, string, rom, num_unsigned, num1, num2);
    fclose(file);
    return SUCCESS;
}