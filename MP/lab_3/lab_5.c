#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum Status {
    SUCCESS,
    ERROR_OPENING_FILE,
    ERROR_MEMORY,
    ERROR_OPENING_OUTPUT_FILE,
    WRONG_DATA_IN_FILE,
    WRONG_GRADES,
    WRONG_NUMBER_ARGUMENTS,
    FILE_IS_EMPTY
};

typedef struct {
    int id;
    char name[256];
    char surname[256];
    char group[256];
    unsigned char* grades;
} Student;

int count_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Ошибка открытия файла: %s\n", filename);
        return -1;
    }

    int count = 0;
    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        count++;
    }

    fclose(file);
    return count;
}

int is_valid_name(const char *name) {
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha(name[i])) {
            return 0;
        }
    }
    return 1;
}

int is_valid_id(int id) {
    return id >= 0;
}

int are_valid_grades(const unsigned char *grades) {
    for (int i = 0; i < 5; i++) {
        if (grades[i] < 0 || grades[i] > 100) {
            return 0;
        }
    }
    return 1;
}

int compare_id(const void* a, const void* b) {
    const Student* student1 = (const Student*)a;
    const Student* student2 = (const Student*)b;
    return student1->id - student2->id;
}

int compare_surname(const void* a, const void* b) {
    const Student* student1 = (const Student*)a;
    const Student* student2 = (const Student*)b;
    return strcmp(student1->surname, student2->surname);
}

int compare_name(const void* a, const void* b) {
    const Student* student1 = (const Student*)a;
    const Student* student2 = (const Student*)b;
    return strcmp(student1->name, student2->name);
}

int compare_group(const void* a, const void* b) {
    const Student* student1 = (const Student*)a;
    const Student* student2 = (const Student*)b;
    return strcmp(student1->group, student2->group);
}

void print_student(Student* student) {
    printf("Идентификатор: %d\n", student->id);
    printf("Имя: %s\n", student->name);
    printf("Фамилия: %s\n", student->surname);
    printf("Группа: %s\n", student->group);
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += student->grades[i];
    }
    float average = (float)sum / 5.0;
    printf("Average Grade: %.2f\n", average);
}

int main(int argc, char* argv[]) {
    enum Status status = SUCCESS;
    if (argc < 3) {
        printf("Usage: program <input_file> <output_file>\n");
        return WRONG_NUMBER_ARGUMENTS;
    }

    char* input_file = argv[1];
    char* output_file = argv[2];

    FILE* file = fopen(input_file, "r");
    if (file == NULL) {
        printf("Ошибка открытия файла:\n");
        return ERROR_OPENING_FILE;
    }



    int num_students = count_lines(input_file);

    Student* students = (Student *)malloc(sizeof(Student) * num_students);
    if (students == NULL) {
        printf("Error allocating memory\n");
        fclose(file);
        return ERROR_MEMORY;
    }

    for (int i = 0; i < num_students; i++) {
        int items_read = fscanf(file, "%d %s %s %s", &students[i].id, students[i].name, students[i].surname, students[i].group);
        if (items_read > 1) {
            if (!is_valid_id(students[i].id) || !is_valid_name(students[i].name) || !is_valid_name(students[i].surname)) {
                printf("Error: Invalid ID or name/surname format for student %d\n", i);
                free(students);
                fclose(file);
                return WRONG_DATA_IN_FILE;
            }

            students[i].grades = malloc(sizeof(unsigned char) * 5);
            if (students[i].grades == NULL) {
                printf("Error: Memory allocation failed for student %d\n", i);
                free(students);
                fclose(file);
                return ERROR_MEMORY;
            }

            int num_grades_read = 0;
            while (num_grades_read < 5 && fscanf(file, " %hhu", &students[i].grades[num_grades_read]) == 1) {
                num_grades_read++;
            }

            if (num_grades_read != 5) {
                printf("Error: Expected %d grades for student %d, but %d grades were read\n", 5, i + 1, num_grades_read);
                for (int i = 0; i < num_students; i++) {
                    free(students[i].grades);
                }    
                free(students);
                fclose(file);
                return WRONG_GRADES;
            }
            int extra;
            while ((extra = fgetc(file)) != '\n' && extra != EOF) {
                if (!isspace(extra)) {
                    printf("Error: Extra data found after grades for student %d\n", i + 1);
                    for (int i = 0; i < num_students; i++) {
                        free(students[i].grades);
                    }
                    free(students);
                    fclose(file);
                    return WRONG_GRADES;
                }
            }
            if (!are_valid_grades(students[i].grades)) {
                printf("Error: Invalid grades for student %d\n", i);
                for (int i = 0; i < num_students; i++) {
                    free(students[i].grades);
                }    
                free(students);
                fclose(file);
                return WRONG_GRADES;
            }
        }
        else{
            printf("File has empty string\n");
            free(students);
            fclose(file);
            return WRONG_DATA_IN_FILE;
        }
    }

    fclose(file);

    int operation;
    while(1){
        printf("1. Search by ID\n");
        printf("2. Search by Surname\n");
        printf("3. Search by Name\n");
        printf("4. Search by Group\n");
        printf("5. Sort by ID\n");
        printf("6. Sort by Surname\n");
        printf("7. Sort by Name\n");
        printf("8. Sort by Group\n");
        printf("9. Print students with above average grades\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &operation);

        switch(operation){
            case 0: 
                for (int i = 0; i < num_students; i++) {
                    free(students[i].grades);
                }    
                free(students);
                return SUCCESS;
            case 1: 
                int searchId;
                printf("Enter ID: ");
                scanf("%d", &searchId);

                for (int i = 0; i < num_students; i++) {
                    if (students[i].id == searchId) {
                        print_student(&students[i]);
                    }
                }
                for (int i = 0; i < num_students; i++) {
                    if (students[i].id == searchId) {
                        FILE *outputFile = fopen(output_file, "a");
                        if (!outputFile) {
                            for (int i = 0; i < num_students; i++) {
                                free(students[i].grades);
                            }    
                            free(students);
                            printf("Failed to open trace file\n");
                            return ERROR_OPENING_OUTPUT_FILE;
                        }
                        fprintf(outputFile, "ID: %d\n", students[i].id);
                        fprintf(outputFile, "Name: %s\n", students[i].name);
                        fprintf(outputFile, "Surname: %s\n", students[i].surname);
                        fprintf(outputFile, "Group: %s\n", students[i].group);

                        int totalGrades = 0;
                        for (int j = 0; j < 5; j++) {
                            totalGrades += students[i].grades[j];
                        }
                        double average = (double)totalGrades / 5;
                        fprintf(outputFile, "Average Grade: %.2f\n", average);
                        fclose(outputFile);
                    }
                    
                }
                break;
            case 2: 
                char searchSurname[256];
                printf("Enter Surname: ");
                scanf("%s", searchSurname);

                for (int i = 0; i < num_students; i++) {
                    if (strcmp(students[i].surname, searchSurname) == 0) {
                        print_student(&students[i]);
                    }
                }
                break;
            case 3: 
                char searchName[256];
                printf("Enter Name: ");
                scanf("%s", searchName);

                for (int i = 0; i < num_students; i++) {
                    if (strcmp(students[i].name, searchName) == 0) {
                        print_student(&students[i]);
                    }
                }
                break;
             case 4: 
                char searchGroup[256];
                printf("Enter Group: ");
                scanf("%s", searchGroup);

                for (int i = 0; i < num_students; i++) {
                    if (strcmp(students[i].group, searchGroup) == 0) {
                        print_student(&students[i]);
                    }
                }
                break;
            case 5:
                qsort(students, num_students, sizeof(Student), compare_id);
                printf("Sorted by ID:\n");
                for (int i = 0; i < num_students; i++) {
                    printf("%d %s %s %s " , students[i].id, students[i].name, students[i].surname, students[i].group);
                    for (int j = 0; j < 5; j++) {
                        printf("%hhu ", students[i].grades[j]);
                    }
                    printf("\n");
                }
                break;
            case 6:
                qsort(students, num_students, sizeof(Student), compare_surname);
                printf("Sorted by Surname:\n");
                for (int i = 0; i < num_students; i++) {
                    printf("%d %s %s %s " , students[i].id, students[i].name, students[i].surname, students[i].group);
                    for (int j = 0; j < 5; j++) {
                        printf("%hhu ", students[i].grades[j]);
                    }
                    printf("\n");
                }
                break;
            case 7:
                qsort(students, num_students, sizeof(Student), compare_name);
                printf("Sorted by Name:\n");
                for (int i = 0; i < num_students; i++) {
                    printf("%d %s %s %s " , students[i].id, students[i].name, students[i].surname, students[i].group);
                    for (int j = 0; j < 5; j++) {
                        printf("%hhu ", students[i].grades[j]);
                    }
                    printf("\n");
                }
                break;
            case 8:
                qsort(students, num_students, sizeof(Student), compare_group);
                printf("Sorted by Group:\n");
                for (int i = 0; i < num_students; i++) {
                    printf("%d %s %s %s " , students[i].id, students[i].name, students[i].surname, students[i].group);
                    for (int j = 0; j < 5; j++) {
                        printf("%hhu ", students[i].grades[j]);
                    }
                    printf("\n");
                }
                break;
            case 9:
                FILE *outputFile = fopen(output_file, "a");
                if (!outputFile) {
                    for (int i = 0; i < num_students; i++) {
                        free(students[i].grades);
                    }    
                    free(students);
                    printf("Failed to open trace file\n");
                    return ERROR_OPENING_OUTPUT_FILE;
                }
                int sum = 0;
                for (int i = 0; i < num_students; i++) {
                    for (int j = 0; j < 5; j++) {
                        sum += students[i].grades[j];
                    }
                }
                float overall_average = (float)sum / (5.0 * num_students);

                fprintf(outputFile, "Students above average:\n");
                for (int i = 0; i < num_students; i++) {
                    int student_sum = 0;
                    for (int j = 0; j < 5; j++) {
                        student_sum += students[i].grades[j];
                    }
                    float student_average = (float)student_sum / 5.0;
                    if (student_average > overall_average) {
                        fprintf(outputFile, "%s %s\n", students[i].surname, students[i].name);
                    }
                }
                printf("Average scores have been successfully written to the file\n");
                fclose(outputFile);
                break;
            default:
                printf("Invalid operation\n");
                break;
        }
        printf("\n");
    }
    for (int i = 0; i < num_students; i++) {
        free(students[i].grades);
    }    
    free(students);
    return status;
}