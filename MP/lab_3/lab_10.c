#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum Status{
    SUCCESS,
    ERROR_OF_MEMORY,
    WRONG_INPUT,
    ERROR_OF_FILE,
    ERROR
};

typedef struct Node{
    char data;
    struct Node ** children;
    struct Node * next;
    int count;
} Node, *Node_ptr, **Node_array;

Node_ptr create_node(char data){
    Node_ptr node = (Node_ptr)malloc(sizeof(Node));
    if (!node){
        return NULL;
    } 
    node->data = data;
    node->children = NULL;
    node->next = NULL;
    node->count = 0;
    return node;
}

void delete_tree(Node_ptr root){
    if (!root){
        return;
    }
    for (int i = 0; i < root->count; i++){
        delete_tree((root->children)[i]);
    }
    free(root->children);
    root->children = NULL;
    free(root);
    root = NULL;
}

int add_child(Node_ptr parent, Node_ptr child){
    parent->count += 1;
    Node_array temp = (Node_array)realloc(parent->children, (parent->count) * sizeof(Node_ptr));
    if (!temp){
        delete_tree(child);
        return ERROR_OF_MEMORY;
    }
    parent->children = temp;
    (parent->children)[parent->count - 1] = child;
}

void print_tree(FILE * file, Node_ptr root, int depth){
    if (root){
        for (int i = 0; i < depth; i++) fprintf(file, "    ");
        fprintf(file, "%c\n", root->data);
        for (int i = 0; i < root->count; i++) print_tree(file, (root->children)[i], depth + 1);
    }
}

Node_ptr read_expression_in_file(char * expression){
    int length = strlen(expression);
    Node_ptr root = create_node(expression[0]);
    Node_ptr current = root;
    Node_ptr node;
    Node_array stack = (Node_array)malloc(length * sizeof(Node_ptr));
    if (!stack){
        return NULL;
    } 
    int result;
    int size = 0;
    for (int i = 0; i < length; i++){
        if (expression[i] == '('){
            node = create_node(expression[i + 1]);
            stack[size] = current;
            size += 1;
            result = add_child(current, node);
            current = node;
        }
        else if (expression[i] == ','){
            node = create_node(expression[i + 1]);
            result = add_child(stack[size - 1], node);
            current->next = node;
            current = node;
        }
        else if (expression[i] == ')'){
            stack[size - 1] = NULL;
            size -= 1;
        }
    }
    free(stack);
    stack = NULL;
    return root;
}

void clean_file(Node_array trees, int length){
    for (int i = 0; i < length; i++){
        delete_tree(trees[i]);
    }
    free(trees);
    trees = NULL;
}

void print_to_file(FILE * output_file, Node_array trees, int length){
    for (int i = 0; i < length; i++){
        print_tree(output_file, trees[i], 0);
        fprintf(output_file, "\n");
        fprintf(output_file, "--------");
        fprintf(output_file, "\n");
    }
}

int get_expression(FILE * file, char ** expression){
    char symbol = fgetc(file);
    int length = 1;
    *expression = (char*)malloc(length * sizeof(char));
    if (!(*expression)){
        fclose(file);
        return ERROR_OF_MEMORY;
    }
    int count = 0;
    char * current;
    while (symbol != '\n' && symbol != EOF){
        if (length <= count){
            length *= 2;
            current = (char*)realloc(*expression, length * sizeof(char));
            if (!current){
                free(*expression);
                *expression = NULL;
                return ERROR_OF_MEMORY;
            }
            *expression = current;
        }
        if (symbol != ' '){
            (*expression)[count] = symbol;
            count += 1;
        }
        symbol = fgetc(file);
    }
    if (length <= count){
        length *= 2;
        current = (char*)realloc(*expression, length * sizeof(char));
        if (!current){
            free(*expression);
            *expression = NULL;
            return ERROR_OF_MEMORY;
        }
        *expression = current;
    }
    (*expression)[count] = 0;
    return SUCCESS;
}

int get_trees(FILE * input_file, Node_array * trees, int * length){
    Node_ptr root;
    Node_array current;
    char * expression = NULL;
    int result = 0;
    while (!feof(input_file)){
        result = get_expression(input_file, &expression);
        if (result != SUCCESS){
            clean_file(*trees, *length);
            return ERROR_OF_MEMORY;
        }
        root = read_expression_in_file(expression);
        if (!root){
            clean_file(*trees, *length);
            free(expression);
            expression = NULL;
            return ERROR_OF_MEMORY;
        }
        (*length) += 1;
        current = (Node_array)realloc(*trees, (*length) * sizeof(Node_ptr));
        if (!current){
            clean_file(*trees, *length);
            free(expression);
            expression = NULL;
            return ERROR_OF_MEMORY;
        }
        *trees = current;
        (*trees)[(*length) - 1] = root;
        free(expression);
        expression = NULL;
    }
    return SUCCESS;
}

int main(int argc, char * argv[]){
    if (argc != 3){
        printf("Ошибка ввода\n");
        return WRONG_INPUT;
    }
    char * file1 = argv[1];
    char * file2 = argv[2];
    FILE * input_file = fopen(file1, "r");
    if (!input_file){
        printf("Ошибка открытия файла\n");
        return ERROR_OF_FILE;
    }

    fseek(input_file, 0, SEEK_END);

    long size = ftell(input_file);
    if (size == 0) {
        printf("Файл пуст\n");
        return ERROR;
    } 

    fseek(input_file, 0, SEEK_SET);

    FILE * output_file = fopen(file2, "w");
    if (!output_file){
        fclose(input_file);
        printf("Ошибка открытия файла\n");
        return ERROR_OF_FILE;
    }
    Node_array trees = NULL;
    int length = 0;
    int result = get_trees(input_file, &trees, &length);
    if (result != SUCCESS){
        fclose(input_file);
        fclose(output_file);
        printf("Ошибка\n");
        return ERROR;
    }
    fclose(input_file);

    print_to_file(output_file, trees, length);
    fclose(output_file);

    clean_file(trees, length);

    printf("Проверьте выходной файл\n");
    return SUCCESS;
}