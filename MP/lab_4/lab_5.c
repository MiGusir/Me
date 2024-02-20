#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum Status{
    SUCCESS = 0,
    WRONG_INPUT,
    OVERFLOW,
    ERROR_OF_FILE,
    INVALID_BRACKETS,
    WRONG_SYMBOL,
    ERROR_OF_ZERO
};

static const char* PrintErrors[] = {
    "Success",
    "Wrong input",
    "Overflow",
    "Error of file",
    "Invalid number of brackets",
    "Wrong symbol",
    "You can't divide by zero"
};

int Operator(const char symbol) {
    if (symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/' || symbol == '%' || symbol == '^'){
        return 1;
    }
    return 0;
}

int Priority(const char symbol) {
    switch(symbol) {
        case '+': return 1;
        case '-': return 1;
        case '*': return 2;
        case '/': return 2;
        case '%': return 2;
        case '^': return 3;
        default: return -1;    
    }
}

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct Stack {
    Node* head;
} Stack;

Node* CreateNode(const int data) {
    Node* new = (Node*)malloc(sizeof(Node));
    if (new == NULL){
        return NULL;
    }
    new->data = data;
    new->next = NULL;
    return new;
}

void CreateStack(Stack* stack) {
    stack->head = NULL;
}

void DeleteStack(Stack* stack) {
    while (stack->head != NULL) {
        Node* temp = stack->head;
        stack->head = stack->head->next;
        free(temp);
    }
}

void Push(Stack* stack, const int data) {
    Node* new = CreateNode(data);
    if (new == NULL){
        return;
    }
    new->next = stack->head;
    stack->head = new;
}

int IsEmpty(Stack* stack) {
    return (stack->head == NULL);
}

int Pop(Stack* stack) {
    if (IsEmpty(stack)) {
        printf("Stack is empty\n");
        return -1;
    }
    
    int pop = stack->head->data;
    Node* temp = stack->head;
    stack->head = stack->head->next;
    free(temp);
    return pop;
}

int FindHead(Stack* stack) {
    if (IsEmpty(stack)) {
        printf("Stack is empty\n");
        return -1;
    }
    return stack->head->data;
}

enum Status CheckOverflow(const int i, char output[100]) {
    if (i >= 100-1){
        output[100-1] = '\0';
        return OVERFLOW;
    }
    return SUCCESS;
}

enum Status InfixToPostfix(const char expression[], int length, char output[100]) {
    Stack stack;
    CreateStack(&stack);
    memset(output, '\0', 100);
    int j = 0;

    for (int i = 0; i < length; ++i) {
        if (CheckOverflow(j, output) != SUCCESS) {
            DeleteStack(&stack);
            return OVERFLOW;
        }
        char chr = expression[i];

        if (isdigit(chr)){
            output[j++] = chr;
            while (isdigit(expression[++i])) {
                output[j++] = expression[i];
            }
            output[j++] = ' ';
            --i;
            continue;
        } 
        else if (Operator(chr)) {
            char opr1 = chr;
            char opr2 = ' ';
            if (!IsEmpty(&stack)){
                opr2 = FindHead(&stack);
            }
            if (opr1 != '^'){ 
                while (!IsEmpty(&stack) && Priority(opr1) <= Priority(opr2) && Operator(opr2)){
                    opr2 = Pop(&stack);
                    output[j++] = opr2;
                    output[j++] = ' ';
                    if (CheckOverflow(j, output) != SUCCESS){
                        DeleteStack(&stack);
                        return OVERFLOW;
                    }
                    if (!IsEmpty(&stack)){
                        opr2 = FindHead(&stack);
                    } 
                    else{
                        break;
                    }
                }
                Push(&stack, opr1);
            }
            else{
                while (!IsEmpty(&stack) && Priority(opr1) < Priority(opr2) && Operator(opr2)){
                    opr2 = Pop(&stack);
                    output[j++] = opr2;
                    output[j++] = ' ';
                    if (CheckOverflow(j, output) != SUCCESS){
                        DeleteStack(&stack);
                        return OVERFLOW;
                    }
                    if (!IsEmpty(&stack)){
                        opr2 = FindHead(&stack);
                    } 
                    else{
                        break;
                    }
                }
                Push(&stack, opr1);
            }
        }
        else if (chr == '('){
            Push(&stack, '(');
        }
        else if (chr == ')'){
            while (!IsEmpty(&stack) && FindHead(&stack) != '('){
                output[j++] = Pop(&stack);
                output[j++] = ' ';
            }
            if (IsEmpty(&stack)){
                DeleteStack(&stack);
                return INVALID_BRACKETS;
            } 
            else{
                Pop(&stack);
            }
        }
        else if (!isspace(chr) && chr != '\0' && chr != '\n'){
            DeleteStack(&stack);
            return WRONG_SYMBOL;
        }
    }
    while (!IsEmpty(&stack)){
        if (CheckOverflow(j, output) != SUCCESS){
            DeleteStack(&stack);
            return OVERFLOW;
        }
        if (FindHead(&stack) == '('){
            DeleteStack(&stack);
            return INVALID_BRACKETS;
        }

        output[j++] = Pop(&stack);
        output[j++] = ' ';
    }
    return SUCCESS;
}


enum Status Calculation(const char expression[], const size_t length, int *result) {
    if (result == NULL){
        return WRONG_INPUT;
    }
    Stack stack;
    CreateStack(&stack);
    char input[100];
    for (size_t i = 0; i < length; ++i)
        input[i] = expression[i];

    char *token = strtok(input, " ");
    while (token != NULL){
        if (isdigit(token[0])){
            char *end;
            long num = strtol(token, &end, 10);
            if (*end != '\0'){
                DeleteStack(&stack);
                return WRONG_INPUT;
            }
            Push(&stack, (int)num);
        } 
        else{
            if (IsEmpty(&stack)){
                DeleteStack(&stack);
                return WRONG_INPUT;
            }
            int operand2 = Pop(&stack);
            if (IsEmpty(&stack)){
                DeleteStack(&stack);
                return WRONG_INPUT;
            }
            int operand1 = Pop(&stack);
            int res;
            switch (token[0]){
                case '+':
                    res = operand1 + operand2;
                    break;
                case '-':
                    res = operand1 - operand2;
                    break;
                case '*':
                    res = operand1 * operand2;
                    break;
                case '/':
                    if (operand2 == 0){
                        DeleteStack(&stack);
                        return ERROR_OF_ZERO;
                    }
                    res = operand1 / operand2;
                    break;
                case '%':
                    if (operand2 == 0){
                        DeleteStack(&stack);
                        return ERROR_OF_ZERO;
                    }
                    res = operand1 % operand2;
                    break;
                case '^':
                    res = 1;
                    for (int i = 0; i < operand2; i++) {
                        res *= operand1;
                    }
                    break;
                default:
                    DeleteStack(&stack);
                    return WRONG_INPUT;
            }
            Push(&stack, res);
        }
        token = strtok(NULL, " ");
    }

    if (IsEmpty(&stack)){
        DeleteStack(&stack);
        return WRONG_INPUT;
    }
    *result = Pop(&stack);
    if (!IsEmpty(&stack)) {
        DeleteStack(&stack);
        return WRONG_INPUT;
    }

    DeleteStack(&stack);
    return SUCCESS;
}

enum Status NewFile(const char inputFile[], const int fileExists, FILE** file) {
    if (inputFile == NULL) {
        return WRONG_INPUT;
    }    
    size_t inputLen = strnlen(inputFile, 100);
    size_t extenLen = sizeof("_out.txt");
    if (inputLen > 100 - extenLen) { 
        return WRONG_INPUT;
    }

    char outputFile[100];
    memset(outputFile, '\0', 100);
    strncpy(outputFile, inputFile, inputLen);
    strncpy(outputFile + inputLen, "_out.txt", extenLen);

    if (fileExists) {
        *file = fopen(outputFile, "a");
    } 
    else {
        *file = fopen(outputFile, "w");
    }
    if (*file == NULL) {
        return ERROR_OF_FILE;
    }

    return SUCCESS;
}


int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Invalid input. It should be <file1> <file2> ...\n");
        return WRONG_INPUT;
    }

    for (int i = 1; i < argc; ++i){
        FILE *inputFile = fopen(argv[i], "r");

        if (inputFile == NULL){
            printf("Error of openning %s\n", argv[i]);
            continue;
        }
        printf("File: %s\n", argv[i]);
        fflush(stdout);

        char line[100];
        size_t lineLen = sizeof(line);
        memset(line, '\0', lineLen);
        int lineNumber = 0;
        int outputFileExists = 0;
        while (fgets(line, sizeof(line), inputFile) != NULL){
            char output[100];
            size_t outputLen = sizeof(output);
            int result;
            printf("Original expression: %s\n", line);

            enum Status shuntingYardProblem = InfixToPostfix(line, lineLen, output);

            for (int j = 1; j <= 2; ++j){
                if (shuntingYardProblem != SUCCESS) {
                    FILE *outputFile;
                    enum Status newFileStatus = NewFile(argv[i], outputFileExists, &outputFile);
                    switch (newFileStatus){
                        case SUCCESS:
                            outputFileExists = 1;
                            break;
                        default:
                            fclose(inputFile);
                            return newFileStatus;
                    }
                    printf("Expression has error. Check output file.\n\n");
                    fprintf(outputFile, "expression %d: %s has error '%s'\n", lineNumber, line, PrintErrors[shuntingYardProblem]);
                    fflush(outputFile);
                    fclose(outputFile);
                    break;
                } 
                else if (j == 1){
                    printf("Reverse polish form: %s\n", output);
                    shuntingYardProblem = Calculation(output, outputLen, &result);
                } 
                else if (j == 2){
                    printf("Result: %d\n\n", result);
                }
            }
            memset(line, '\0', lineLen);
            ++lineNumber;
        }
        fclose(inputFile);
    }
    return SUCCESS;
}