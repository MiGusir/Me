#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

enum Status {
    SUCCESS = 0,
    ERROR_OF_FILE = -1,
    INVALID_INPUT = -2,
    ERROR_OF_MEMORY = -3,
    ERROR = -4,
};

typedef struct Node{
    char *word;
    int count;
    struct Node *left, *right;
} Node;

Node* CreateNode(char* word) {
    Node* new = (Node*)malloc(sizeof(Node));
    if(!new){
        return NULL;
    }
    new->word = strdup(word);
    new->count = 1;
    new->left = NULL;
    new->right = NULL;
    return new;
}

Node* InsertWord(Node* root, char* word){
    if (root == NULL){
        return CreateNode(word);
    }

    int cmp = strcmp(word, root->word);
    if (cmp < 0){
        root->left = InsertWord(root->left, word);
    } 
    else if (cmp > 0){
        root->right = InsertWord(root->right, word);
    } 
    else{
        root->count += 1;
    }

    return root;
}

Node* GetTree(Node* root, char* word, int count){
    if (root == NULL){
        Node* new = (Node*)malloc(sizeof(Node));
        if(!new){
            return NULL;
        }
        new->word = strdup(word);
        new->count = count;
        new->left = NULL;
        new->right = NULL;
        return new;
    }

    int cmp = strcmp(word, root->word);

    if (cmp < 0){
        root->left = GetTree(root->left, word, count);
    } 
    else if (cmp > 0){
        root->right = GetTree(root->right, word, count);
    }

    return root;
}

void FreeTree(Node* root){
    if (root == NULL){
        return;
    }
    FreeTree(root->left);
    FreeTree(root->right);
    free(root->word);
    free(root);
}

int CountFrequency(Node* root, char* word){
    if (root == NULL){
        return 0;
    }

    int cmp = strcmp(word, root->word);
    if (cmp < 0){
        return CountFrequency(root->left, word);
    } 
    else if (cmp > 0){
        return CountFrequency(root->right, word);
    } 
    else{
        return root->count;
    }
}

void PrintFrequency(Node *root) {
    if (root != NULL) {
        PrintFrequency(root->left);
        printf("Строка \"%s\" имеет частоту %d\n", root->word, root->count);
        PrintFrequency(root->right);
    }
}

void FindLongest(Node *root, char **longest){
    if (root == NULL){
        return;
    }
    FindLongest(root->right, longest);
    if (strlen(root->word) > strlen(*longest)){
        *longest = root->word;
    }
    FindLongest(root->left, longest);
}

void FindShortest(Node *root, char **shortest){
    if (root == NULL){
        return;
    }
    FindShortest(root->left, shortest);
    if (*shortest == NULL || strlen(root->word) < strlen(*shortest)){
        *shortest = root->word;
    }
    FindShortest(root->right, shortest);
}

int ValidChar(char ch[]){
    if (ch[1] != '\0'){
        return 0;
    }
    return 1;
}

void TraversalTree(Node* root, FILE* file, int depth) {
    if (root != NULL) {
        TraversalTree(root->right, file, depth + 1);

        for (int i = 0; i < depth; i++) {
            fprintf(file, "  ");
        }

        fprintf(file, "%d %s\n", root->count, root->word);

        TraversalTree(root->left, file, depth + 1);
    }
}

int FindHeight(Node* root){
    if (root == NULL){
        return 0;
    } 
    else{
        int height_left = FindHeight(root->left);
        int height_right = FindHeight(root->right);
        if(height_left > height_right){
            return height_left + 1;
        }
        else{
            return height_right + 1;
        }
    }
}

int LoadTree(Node** root, FILE* file){
    char word[BUFSIZ];
    int count;
    
    while (fscanf(file, "%s %d", word, &count) == 2){
        *root = GetTree(*root, word, count);
    }

    if (!feof(file)){
        return ERROR_OF_FILE;
    }

    return SUCCESS;
}

void CountMaximum(Node* root, int * count){
    if (!root){
        return;
    }

    if (root->left){
        CountMaximum(root->left, count);
    }

    if (root->count > *count){
        *count = root->count;
    }

    if (root->right) {
        CountMaximum(root->right, count);
    }
}

void PrintNode(Node* root, int * quantity, int length){
    if (!root){
        return;
    }

    if (root->right){
        PrintNode(root->right, quantity, length);
    }

    if (root->count == length && (*quantity)){
        (*quantity)--;
        printf("\"%s\" частота = %d\n", root->word, root->count);
    }

    if (root->left){
        PrintNode(root->left, quantity, length);
    }
}

void PrintMostQuantity(Node* root, int n){
    int count = 0;
    CountMaximum(root, &count);
    while (n != 0 && count != 0){
        PrintNode(root, &n, count);
        count -= 1;
    }
}

void InsertNode(Node** root, Node* newNode, int depth) {
    if (*root == NULL) {
        *root = newNode;
    } else {
        if (depth == 1) {
            if (newNode->count < (*root)->count) {
                (*root)->left = newNode;
            } else {
                (*root)->right = newNode;
            }
        } else if (depth > 1) {
            if ((*root)->left != NULL) {
                InsertNode(&((*root)->left), newNode, depth - 1);
            }
            if ((*root)->right != NULL) {
                InsertNode(&((*root)->right), newNode, depth - 1);
            }
        } else {
            fprintf(stderr, "Неправильный формат файла.\n");
        }
    }
}

int RestoreTree(Node** root, FILE* file) {
    char line[BUFSIZ];

    while (fgets(line, sizeof(line), file) != NULL) {
        int depth = 0;
        char* token = strtok(line, " ");
        
        while (*token == ' ') {
            depth++;
            token = strtok(NULL, " ");
        }
        
        int count = atoi(token);
        token = strtok(NULL, "\n");
        if (token == NULL) {
            fprintf(stderr, "Неправильный формат файла.\n");
            return ERROR_OF_FILE;
        }
        char word[BUFSIZ];
        strcpy(word, token);
        
        Node* newNode = malloc(sizeof(Node));
        if (newNode == NULL) {
            perror("Ошибка памяти");
            return ERROR_OF_MEMORY;
        }
        
        newNode->count = count;
        strcpy(newNode->word, word);
        newNode->left = newNode->right = NULL;
        
        InsertNode(root, newNode, depth);
    }
    if (ferror(file)){
        fprintf(stderr, "Ошибка файла.\n");
        return ERROR_OF_FILE;
    }
    
    return SUCCESS;
}

int main(int argc, char* argv[]){
    if (argc < 3) {
        printf("Некорректные аргументы\n");
        return INVALID_INPUT;
    }

    char* filename = argv[1];

    int capacity_sep = BUFSIZ;
    char* separators = (char*)calloc(capacity_sep + 1, sizeof(char));
    if (separators == NULL){
        printf("Ошибка памяти\n");
        return ERROR_OF_MEMORY;
    }

    for (int i = 0; i <= capacity_sep; i++){
        separators[i] = '\0';
    }

    int cnt_sep = 0;
    for (int i = 2; argv[i] != NULL; i++){
        int type;
        if ((type = ValidChar(argv[i])) == 0){
            printf("Ошибка ввода\n");
            return INVALID_INPUT;
        }

        separators[i - 2] = *argv[i];
        if (i - 2 >= capacity_sep){

            capacity_sep *= 2;
            char* tmp = (char*)realloc(separators, sizeof(char) * (capacity_sep + 1));
            if (tmp == NULL){
                free(separators);
                printf("Ошибка памяти\n");
                return ERROR_OF_MEMORY;
            }
            separators = tmp;
            separators[capacity_sep] = '\0';
        }
        cnt_sep += 1;
    }

    FILE* input_file = fopen(filename, "r");
    if (!input_file){
        free(separators);
        printf("Ошибка файла\n");
        return ERROR_OF_FILE;
    }

    Node* root = NULL;
    int capacity_str = 2;
    int cnt_str = 0;
    char* my_word = (char*)calloc(capacity_str, sizeof(char));
    if (my_word == NULL){
        printf("Ошибка файла\n");
        return ERROR_OF_MEMORY;
    }

    strcpy(my_word, "");

    char c;
    while ((c = fgetc(input_file)) != EOF){
        bool flag = true;
        for (int i = 0; i < cnt_sep && flag; i++){
            if (c == separators[i]){
                flag = false;
            }
        }
        if (!flag) {
            if (strlen(my_word) != 0){
                root = InsertWord(root, my_word);
            }
            for (int i = 0; i < cnt_str; i++){
                my_word[i] = '\0'; 
            }
            my_word[cnt_str] = '\0';
            cnt_str = 0;
        } 
        else{
            if (cnt_str >= capacity_str - 1){
                capacity_str *= 2;
                char* tmp = (char*)realloc(my_word, capacity_str * sizeof(char));
                if (tmp == NULL){
                    free(my_word);
                    printf("Ошибка памяти\n");
                    return ERROR_OF_MEMORY;
                }
                my_word = tmp;
                tmp = NULL;
            }
            my_word[cnt_str] = c;
            cnt_str += 1;
        }
    }

    if (strlen(my_word) != 0){
        root = InsertWord(root, my_word);
    }
    
    int option;
    while (true){
        printf("\nВыберите опцию:\n");
        printf("0. Выход\n");
        printf("1. Вывести количество всех слов в файле\n");
        printf("2. N самых популярных слов\n");
        printf("3. Самое длинное и короткое слово\n");
        printf("4. Найти слово в файле\n");
        printf("5. Вывести высоты дерева\n");
        printf("6. Сохранить дерево в файл\n");
        printf("Введите номер операции: ");
        if (scanf("%d", &option) != 1){
            printf("Ошибка ввода, повторите попытку\n");
            break;
        }

        printf("\n\n");
        if (option == 1){
            if (root == NULL){
                printf("Файл пуст\n");
            }
            else{
                PrintFrequency(root);
            }
        } 
        else if (option == 2){
            if (root == NULL) {
                printf("Файл пуст\n");
            }
            else{
                char* words;
                int n;
                printf("Введите номер слова ");
                if (scanf("%d", &n) != 1){
                    printf("Ошибка ввода\n");
                    break;
                }
                printf("\n");
                PrintMostQuantity(root, n);
            }
        }
        else if (option == 3){
            if (root == NULL){
                printf("Файл пуст\n");
            } 
            else{
                char* longest = "";
                char* shortest = NULL;

                FindLongest(root, &longest);
                FindShortest(root, &shortest);
                printf("Самое длинное: %s,\nСамое короткое: %s\n", longest, shortest);
            }
        } 
        else if (option == 4){
            printf("Введите слово");

            char user_word[BUFSIZ];
            if (scanf("%s", user_word) != 1){
                printf("Ошибка ввода\n");
                break;
            }

            int cnt_ocurr = CountFrequency(root, user_word);
            if (cnt_ocurr == 0){
                printf("Слово отсутствует в файле\n");
            } 
            else{
                printf("Слово находится в файле\n");
            }
        } 
        else if (option == 5){
            if (root == NULL){
                printf("Дерево пусто\n");
            } 
            else{
                int height = FindHeight(root);
                printf("Высота дерева: %d\n", height);
            }
        }  
        else if (option == 6){
            printf("Введите выходной файл: ");

            char file_save[BUFSIZ];
            if (scanf("%s", file_save) != 1){
                printf("Ошибка ввода\n");
                break;
            }

            if (strcmp(file_save, argv[1]) == 0){
                printf("\nОшибка, файлы совпадают\n");
                break;
            }
            else{
                FILE* output_file = fopen(file_save, "w");
                if (output_file == NULL){
                    printf("Ошибка открытия файла\n");
                    break;
                }
                int height = FindHeight(root);
                TraversalTree(root, output_file, height);
                printf("Проверьте выходной файл\n");
                fclose(output_file);
            }
        }
        else if (option == 0) {
            break;
        } 
        else {
            printf("Команда отсутствует\n");   
        }
    }

    FreeTree(root);
    free(my_word);
    free(separators);

    fclose(input_file);

    return 0;
}