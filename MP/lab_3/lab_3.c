#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum Status {
    SUCCESS,
    ERROR_INVALID_FLAG,
    ERROR_INPUT_FILE,
    ERROR_OUTPUT_FILE,
    ERROR_OF_MEMORY,
    ERROR_INVALID_NAME,
    ERROR_INVALID_SALARY
};

enum SortingFlag {
    ASCENDING,
    DESCENDING
};

typedef struct {
    int id;
    char first_name[100];
    char last_name[100];
    double salary;
} Employee;

int compare_employees(const void* a, const void* b) {
    const Employee* emp1 = (const Employee*)a;
    const Employee* emp2 = (const Employee*)b;

    if (emp1->salary != emp2->salary) {
        return (emp1->salary < emp2->salary) ? -1 : 1;
    }

    int last_name_cmp = strcmp(emp1->last_name, emp2->last_name);
    if (last_name_cmp != 0) {
        return last_name_cmp;
    }

    int first_name_cmp = strcmp(emp1->first_name, emp2->first_name);
    if (first_name_cmp != 0) {
        return first_name_cmp;
    }

    return emp1->id - emp2->id;
}

int is_valid_name(const char* name) {
    while (*name) {
        if (!isalpha(*name)) {
            return 0;
        }
        name++;
    }
    return 1;
}

int is_valid_salary(const char* salary) {
    int dot_count = 0;
    while (*salary) {
        if (*salary == '.') {
            dot_count++;
        } else if (!isdigit(*salary)) {
            return 0;
        }
        salary++;
    }
    return (dot_count == 1);
}

enum Status read_employee_data(const char* input_file, Employee** employees, int* num_employees) {
    FILE* file = fopen(input_file, "r");
    if (file == NULL) {
        return ERROR_INPUT_FILE;
    }

    fseek(file, 0, SEEK_END);

    long size = ftell(file);
    if (size == 0) {
        printf("Файл пуст\n");
        return ERROR_INPUT_FILE;
    } 

    fseek(file, 0, SEEK_SET);

    int capacity = 10;
    *employees = malloc(capacity * sizeof(Employee));
    if (*employees == NULL) {
        fclose(file);
        return ERROR_OF_MEMORY;
    }

    *num_employees = 0;
    char buffer[100];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (*num_employees == capacity) {
            Employee* temp = realloc(*employees, 2 * capacity * sizeof(Employee));
            if (temp == NULL) {
                fclose(file);
                free(*employees);
                return ERROR_OF_MEMORY;
            }
            *employees = temp;
            capacity *= 2;
        }

        int id;
        char first_name[100];
        char last_name[100];
        char salary[100];

        if (sscanf(buffer, "%d %s %s %s", &id, first_name, last_name, salary) != 4) {
            fclose(file);
            free(*employees);
            return ERROR_INPUT_FILE;
        }

        if (!is_valid_name(first_name) || !is_valid_name(last_name)) {
            fclose(file);
            free(*employees);
            return ERROR_INVALID_NAME;
        }

        if (!is_valid_salary(salary)) {
            fclose(file);
            free(*employees);
            return ERROR_INVALID_SALARY;
        }

        Employee* current_employee = &((*employees)[*num_employees]);
        current_employee->id = id;
        strcpy(current_employee->first_name, first_name);
        strcpy(current_employee->last_name, last_name);
        current_employee->salary = atof(salary);

        (*num_employees)++;
    }
    fclose(file);

    return SUCCESS;
}

enum Status write_employee_data(const char* output_file, Employee* employees, int num_employees) {
    FILE* file = fopen(output_file, "w");
    if (file == NULL) {
        return ERROR_OUTPUT_FILE;
    }

    for (int i = 0; i < num_employees; i++) {
        fprintf(file, "%d %s %s %.2lf\n",
                employees[i].id,
                employees[i].first_name,

                employees[i].last_name,
                employees[i].salary);
    }

    fclose(file);
    return SUCCESS;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Неверное количество аргументов\n");
        return 1;
    }

    const char* input_file = argv[1];
    const char* flag = argv[2];
    const char* output_file = argv[3];

    Employee* employees = NULL;
    int num_employees = 0;

    enum Status status = read_employee_data(input_file, &employees, &num_employees);
    if (status != SUCCESS) {
        if (status == ERROR_INVALID_NAME) {
            printf("Некорректное имя \n");
        } else if (status == ERROR_INVALID_SALARY) {
            printf("Некорректная зарплата\n");
        } else {
            printf("Ошибка открытия файла\n");
        }
        free(employees);
        return 1;
    }

    enum SortingFlag sorting_flag;
    if (strcmp(flag, "-a") == 0) {
        sorting_flag = ASCENDING;
    } else if (strcmp(flag, "-d") == 0) {
        sorting_flag = DESCENDING;
    } else {
        printf("Флаг отсутствует\n");
        free(employees);
        return 1;
    }

    qsort(employees, num_employees, sizeof(Employee), compare_employees);

    if (sorting_flag == DESCENDING) {
        int i = 0;
        int j = num_employees - 1;
        while (i < j) {
            Employee temp = employees[i];
            employees[i] = employees[j];
            employees[j] = temp;
            i++;
            j--;
        }
    }

    status = write_employee_data(output_file, employees, num_employees);
    if (status != SUCCESS) {
        printf("Ошибка записи файла\n");
        free(employees);
        return 1;
    }

    free(employees);
    return 0;
}