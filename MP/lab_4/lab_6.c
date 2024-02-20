#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

enum Status{
    SUCCESS = 0,
    ERROR_OF_MEMORY = -1,
    WRONG_INPUT = -2,
    ERROR_OF_FILE = -3,
    RUNTIME_ERROR = -4,
    WRONG_NUMBER_OF_BRACKETS = -5,
    WRONG_ARGUMENTS = -6,
    WRONG_OPERATOR = -7
};

int operator(char data){
    char * operators = "&|~-+<=!?>";
    int i = 0;
    while (operators[i]){
        if (operators[i] == data){
            return SUCCESS;
        } 
        i += 1;
    }
    return WRONG_OPERATOR;
}

int priority(char data){
    switch(data){
        case '~': return 3;
        case '?': return 2;
        case '!': return 2;
        case '+': return 2;
        case '&': return 2;
        case '|': return 1;
        case '-': return 1;
        case '<': return 1;
        case '=': return 1;
        default: return WRONG_OPERATOR;
    }
}

typedef struct Node{
    char data;
    struct Node * left;
    struct Node * right;
} Node, *Node_ptr;

void PrintTree(Node_ptr root, int level){
    if (!root){
        return;
    } 
    PrintTree(root->left, level + 1);
    for (int i = 0; i < level; i++){
        printf("  ");
    } 
    printf("%c\n", root->data);
    PrintTree(root->right, level + 1);
}

void PrintInfix(FILE * file, Node_ptr root){
    if (!root){
        return;
    } 
    if (operator(root->data) == SUCCESS){
        fprintf(file, "(");
    } 
    PrintInfix(file, root->left);
    fprintf(file, "%c", root -> data);
    if (root->data == '-' || root->data == '+' || root->data == '<'){
        fprintf(file, ">");
    } 
    PrintInfix(file, root->right);
    if (operator(root->data) == SUCCESS){
        fprintf(file, ")");
    } 
}

Node_ptr CreateTreeNode(char data, Node_ptr left, Node_ptr right){
    Node_ptr node = (Node_ptr)malloc(sizeof(Node));
    if (!node){
        return NULL;
    } 
    node->data = data;
    node->left = left;
    node->right = right;
    return node;
}

void FreeTreeNode(Node_ptr node)
{
    node->left = NULL;
    node->right = NULL;
    free(node);
    node = NULL;
}

void FreeTree(Node_ptr root){
    if (!root){
        return;
    } 
    FreeTree(root->left);
    FreeTree(root->right);
    FreeTreeNode(root);
}

typedef struct CharStack{
    char data;
    struct CharStack * next;
} CharStack, *CharStackPtr;

CharStackPtr CreateNodeChar(char data){
    CharStackPtr node = (CharStackPtr)malloc(sizeof(CharStack));
    if (!node){
        return NULL;
    } 
    node->data = data;
    node->next = NULL;
    return node;
}

CharStackPtr GetLastChar(CharStackPtr head){
    if (!head){
        return NULL;
    } 
    CharStackPtr current = head;
    while (current->next){
        current = current->next;
    } 
    return current;
}

CharStackPtr GetPrevChar(CharStackPtr head){
    if (!head){
        return NULL;
    } 
    CharStackPtr prev = NULL;
    CharStackPtr current = head;
    while (current->next){
        prev = current;
        current = current->next;
    }
    return prev;
}

void FreeNodeChar(CharStackPtr node){
    if (!node){
        return;
    } 
    node->next = NULL;
    free(node);
    node = NULL;
}

void PopBackChar(CharStackPtr * head){
    if (!(*head)){
        return;
    } 
    CharStackPtr prev = GetPrevChar(*head);
    CharStackPtr last = GetLastChar(*head);
    FreeNodeChar(last);
    if (prev){
        prev->next = NULL;
    } 
    else{
        *head = NULL;
    } 
}

void FreeStackChar(CharStackPtr head){
    if (!head){
        return;
    } 
    CharStackPtr prev = NULL;
    while (head->next){
        prev = head;
        head = head->next;
        FreeNodeChar(prev);
    }
    FreeNodeChar(head);
}

int PushBackChar(CharStackPtr * head, char data){
    CharStackPtr last = GetLastChar(*head);
    CharStackPtr current = CreateNodeChar(data);
    if (!current){
        FreeStackChar(*head);
        return ERROR_OF_MEMORY;
    }
    if (!last){
        *head = current;
        return SUCCESS;
    }
    last->next = current;
    return SUCCESS;
}

typedef struct TreeStack{
    Node_ptr node;
    struct TreeStack * next;
} TreeStack, *TreeStackPtr;

TreeStackPtr CreatNodeTree(Node_ptr data){
    TreeStackPtr node = (TreeStackPtr)malloc(sizeof(TreeStack));
    if (!node){
        return NULL;
    } 
    node->node = data;
    node->next = NULL;
    return node;
}

TreeStackPtr GetLastTree(TreeStackPtr root){
    if (!root){
        return NULL;
    } 
    TreeStackPtr current = root;
    while (current->next){
        current = current->next;
    } 
    return current;
}

TreeStackPtr GetPrevTree(TreeStackPtr root){
    if (!root){
        return NULL;
    } 
    TreeStackPtr prev = NULL;
    TreeStackPtr current = root;
    while (current->next){
        prev = current;
        current = current->next;
    }
    return prev;
}

void FreeNodeTree(TreeStackPtr node){
    if (!node){
        return;
    } 
    node->next = NULL;
    node->node = NULL;
    free(node);
    node = NULL;
}

void PopBackTree(TreeStackPtr * root){
    if (!(*root)){
        return;
    } 
    TreeStackPtr prev = GetPrevTree(*root);
    TreeStackPtr last = GetLastTree(*root);
    FreeNodeTree(last);
    if (prev){
        prev->next = NULL;
    } 
    else{
        *root = NULL;
    } 
}

void FreeStackTree(TreeStackPtr root){
    if (!root){
        return;
    } 
    TreeStackPtr prev = NULL;
    while (root->next){
        prev = root;
        root = root->next;
        FreeNodeTree(prev);
    }
    FreeNodeTree(root);
}

int PushBackTree(TreeStackPtr * root, Node_ptr data){
    TreeStackPtr last = GetLastTree(*root);
    TreeStackPtr current = CreatNodeTree(data);
    if (!current){
        FreeStackTree(*root);
        return ERROR_OF_MEMORY;
    }
    if (!last){
        *root = current;
        return SUCCESS;
    }
    last->next = current;
    return SUCCESS;
}

int CheckOperator(char * variables, int count, char symbol){
    for (int i = 0; i < count; i++){
        if (variables[i] == symbol){
            return WRONG_OPERATOR;
        } 
    }
    return SUCCESS;
}

int ReadVariables(char ** variables, char * infix){
    int length = 0;
    char * temp;
    int len = strlen(infix);
    for (int i = 0; i < len; i++){
        if (isalpha(infix[i]) && CheckOperator(*variables, length, infix[i]) == SUCCESS){
            length += 1;
            temp = (char*)realloc(*variables, length * sizeof(char));
            if (!temp){
                if (*variables){
                    free(*variables);
                    *variables = NULL;
                }
                free(infix);
                infix = NULL;
                return ERROR_OF_MEMORY;
            }
            *variables = temp;
            (*variables)[length - 1] = infix[i];
        }
    }
    return length;
}

char * NewName(){
    srand(time(NULL));
    int max_len = 5;
    int min_len = 1;

    int len = rand() % (max_len - min_len + 1) + min_len;

    char * file = (char*)malloc((len + 6) * sizeof(char));
    file[len + 6] = 0;
    for (int i = 0; i <= len;){
        file[i] = rand() % ('Z' - '0' + 1) + '0';
        if (isalpha(file[i]) || isdigit(file[i])){
            i += 1;
        } 
    }
    file[len + 1] = '.';
    file[len + 2] = 't';
    file[len + 3] = 'x';
    file[len + 4] = 't';
    printf("Output file: %s\n", file);
    return file;
}

int CountTree(Node_ptr root, const char * variables, const int * values, const int count){
    if (!root){
        return 0;
    } 
    int left = CountTree(root->left, variables, values, count);
    int right = CountTree(root->right, variables, values, count);
    if (isalpha(root->data)){
        for (int i = 0; i < count; i++){
            if (root->data == variables[i]){
                return values[i];
            } 
        }
    }
    if (root->data == '0' || root->data == '1'){
        return root->data - '0';
    } 

    switch (root->data){
        case '&': return left & right;
        case '|': return left | right;
        case '~': return ~left;
        case '-': return left <= right;
        case '+': return ~(~left | right);
        case '<': return left != right;
        case '=': return left == right;
        case '!': return ~(left & right);
        case '?': return ~(left | right);
        default: return WRONG_OPERATOR;
    }
}

void PrintVariables(FILE * file, const char * variables, int count){
    for (int i = 0; i < count; i++){
        fprintf(file, " %c |", variables[i]);
    }
}

void BuildTruthTable(Node_ptr root, const int count, const char * variables){
    char * output_file = NewName();
    FILE * file = fopen(output_file, "w");
    Node_ptr current = root;

    PrintVariables(file, variables, count);
    fprintf(file, " ");
    PrintInfix(file, current);
    fprintf(file, "\n");

    int variants = 1 << count;
    int means[count];
    int result;
    for (int i = 0; i < variants; i++){
        for (int j = 0; j < count; j++){
            fprintf(file, " %d |", (i >> j) & 1);
            means[j] = (i >> j) & 1;
        }
        result = CountTree(root, variables, means, count);
        fprintf(file, " %d\n", result);
    }
    fclose(file);
    free(output_file);
    output_file = NULL;
}

int CheckBracket(char symbol){
    if (symbol == '(' || symbol == ')'){
        return SUCCESS;
    } 
    return WRONG_NUMBER_OF_BRACKETS;
}

int InfToPost(int * error, char ** postfix, const char * infix){
    CharStackPtr stack = NULL;
    int count = 0;
    char * temp;

    char * post = (char*)calloc((2 * strlen(infix) + 1), sizeof(char));
    if (!post){
        *error = RUNTIME_ERROR;
        return ERROR_OF_MEMORY;
    }

    int index = 0;
    CharStackPtr node;

    int res;
    int i = 0;
    int flag = 1;
    char symbol;
    while (infix[i] != '\n' && infix[i] != '\0'){
        symbol = infix[i];
        if ((isalpha(symbol) && isupper(symbol)) || symbol == '0' || symbol == '1'){
            post[index++] = symbol;
            flag = 0;
        }
        else if (operator(symbol) == SUCCESS){   
            if ((symbol == '+' || symbol == '-' || symbol == '<') && infix[i + 1] != '>'){
                *error = WRONG_OPERATOR;
                FreeStackChar(stack);
                free(post);
                post = NULL;
                return RUNTIME_ERROR;
            }
            if (i != 0){
                if (!(infix[i - 1] == '+' || infix[i - 1] == '-' || infix[i - 1] == '<') && symbol == '>'){
                    *error = WRONG_OPERATOR;
                    FreeStackChar(stack);
                    free(post);
                    post = NULL;
                    return RUNTIME_ERROR;
                }
            }
            if (symbol == '>'){
                i += 1;
                continue;
            }
            if (flag && symbol != '~'){
                FreeStackChar(stack);
                free(post);
                post = NULL;
                *error = WRONG_INPUT;
                return RUNTIME_ERROR;
            }
            else{
                flag = 1;
            } 

            node = GetLastChar(stack);
            while (stack && priority(node->data) >= priority(symbol)){
                post[index++] = node->data;
                PopBackChar(&stack);
                node = GetLastChar(stack);
            }
            res = PushBackChar(&stack, symbol);
            node = GetLastChar(stack);
            if (res != SUCCESS){
                FreeStackChar(stack);
                free(post);
                post = NULL;
                return ERROR_OF_MEMORY;
            }
        }
        else if (CheckBracket(symbol) == SUCCESS){
            if (symbol == '('){
                flag = 1;
                res = PushBackChar(&stack, symbol);
                node = GetLastChar(stack);
                if (res != SUCCESS){
                    FreeStackChar(stack);
                    free(post);
                    post = NULL;
                    return ERROR_OF_MEMORY;
                }
            }
            else if (symbol == ')'){
                node = GetLastChar(stack);
                while (stack && node->data != '('){
                    post[index++] = node->data;
                    PopBackChar(&stack);
                    node = GetLastChar(stack);
                }
                if (!stack){
                    FreeStackChar(stack);
                    free(post);
                    post = NULL;
                    *error = WRONG_NUMBER_OF_BRACKETS;
                    return RUNTIME_ERROR;
                }
                PopBackChar(&stack);
            }
        }
        else if (symbol != ' ' && symbol != '\n' && symbol != '\t'){
            FreeStackChar(stack);
            free(post);
            post = NULL;
            *error = WRONG_ARGUMENTS;
            return RUNTIME_ERROR;
        }
        i += 1;
    }

    while (stack){
        if (stack->data == '('){
            FreeStackChar(stack);
            free(post);
            post = NULL;
            *error = WRONG_NUMBER_OF_BRACKETS;
            return RUNTIME_ERROR;
        }
        node = GetLastChar(stack);
        post[index++] = node->data;
        PopBackChar(&stack);
    }
    post[index] = 0;
    if (post[index - 1] == 0){
        *error = WRONG_INPUT;
        FreeStackChar(stack);
        free(post);
        post = NULL;
        return RUNTIME_ERROR;
    }
    FreeStackChar(stack);
    *postfix = post;
    return SUCCESS;
}

int BuildTree(Node_ptr * tree, int * error, char * infix){
    Node_ptr root = NULL;
    char * postfix = NULL;
    int result = InfToPost(error, &postfix, infix);
    if (result != SUCCESS){
        return RUNTIME_ERROR;
    } 
    int length = strlen(postfix);
    Node_ptr node;
    TreeStackPtr stack = NULL;
    char symbol;

    TreeStackPtr variable1;
    TreeStackPtr variable2;
    for (int i = 0; i < length; i++){
        symbol = postfix[i];
        if ((isalpha(symbol) && isupper(symbol)) || symbol == '0' || symbol == '1'){
            node = CreateTreeNode(symbol, NULL, NULL);
            if (!node){
                FreeTree(root);
                free(postfix);
                FreeStackTree(stack);
                postfix = NULL;
                *error = ERROR_OF_MEMORY;
                return RUNTIME_ERROR;
            }
            result = PushBackTree(&stack, node);
            if (result != SUCCESS){
                *error = result;
                FreeTree(root);
                FreeStackTree(stack);
                free(postfix);
                postfix = NULL;
                return RUNTIME_ERROR;
            }
        }
        else if (operator(symbol) == SUCCESS){
            variable1 = GetLastTree(stack);
            if (symbol == '~'){
                node = CreateTreeNode(symbol, NULL, variable1->node);
            }
            else{
                variable2 = GetPrevTree(stack);
                node = CreateTreeNode(symbol, variable2->node, variable1->node);
            }
            if (!node){
                FreeTree(root);
                free(postfix);
                FreeStackTree(stack);
                postfix = NULL;
                *error = ERROR_OF_MEMORY;
                return RUNTIME_ERROR;
            }
            if (symbol == '~'){
                PopBackTree(&stack);
            } 
            else{
                PopBackTree(&stack);
                PopBackTree(&stack);
            }
            result = PushBackTree(&stack, node);
            if (result != SUCCESS){
                *error = result;
                FreeStackTree(stack);
                return RUNTIME_ERROR;
            }
        }
    }
    FreeStackTree(stack);
    free(postfix);
    postfix = NULL;
    *tree = node;
    return SUCCESS;
}

int BuildTable(char * input_file){
    FILE * file = fopen(input_file, "r");
    if (!file){
        return ERROR_OF_FILE;
    } 
    char * line = NULL;
    size_t len = 0;
    getline(&line, &len, file);
    int length = strlen(line);
    if (!length){
        free(line);
        line = NULL;
        fclose(file);
        return ERROR_OF_FILE;
    }
    if (!feof(file)){
        free(line);
        line = NULL;
        fclose(file);
        return ERROR_OF_FILE;
    }
    fclose(file);
    int error;
    Node_ptr root = NULL;
    int result = BuildTree(&root, &error, line);
    if (result != SUCCESS){
        free(line);
        line = NULL;
        return error;
    }
    char * variables = NULL;
    int count = ReadVariables(&variables, line);
    if (count < 0){
        FreeTree(root);
        return ERROR_OF_MEMORY;
    }
    free(line);
    line = NULL;
    BuildTruthTable(root, count, variables);
    free(variables);
    variables = NULL;
    FreeTree(root);
    return SUCCESS;
}

int main(int argc, char * argv[]){
    if (argc != 2)
    {
        printf("Invalid number of arguments\n");
        return WRONG_INPUT;
    }
    int result = BuildTable(argv[1]);
    switch(result){
        case WRONG_NUMBER_OF_BRACKETS:
            printf("Wrong number of brackets.\n");
            return result;
        case WRONG_ARGUMENTS:
            printf("Invalid arguments in variables.\n");
            return result;
        case ERROR_OF_MEMORY:
            printf("Error of memory.\n");
            return result;
        case WRONG_OPERATOR:
            printf("Incorrect operator in variables.\n");
            return result;
        default: 
            break;
    }
    printf("Check output file.\n");
    return SUCCESS;
}

