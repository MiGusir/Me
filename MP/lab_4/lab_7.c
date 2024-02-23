#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MIN_BUF 200

typedef enum
{
    correct,
    runtime_error,
    invalid_input,
    allocate_error,
    invalid_file,
    end_of,
    not_found,
    nulls, empty,
    invalid_notation
} statements;

typedef struct MemoryCell
{
    char *name;
    int val;
} MemoryCell;

typedef struct Cells
{
    MemoryCell **cells;
    int len;
    int buf;
} Cells;

MemoryCell *create_cell(const char *name, int value)
{
    if (name == NULL)
    {
        return NULL;
    }
    int l = strlen(name);
    MemoryCell *cell = (MemoryCell *)malloc(sizeof(MemoryCell));
    if (cell == NULL)
    {
        return NULL;
    }
    cell->name = (char *)malloc(sizeof(char) * (l + 1));
    if (cell->name == NULL)
    {
        free(cell);
        return NULL;
    }
    strcpy(cell->name, name);
    cell->val = value;
    return cell;
}

bool is_latin(const char *s)
{
    int l = strlen(s);
    for (int i = 0; i < l; i++)
    {
        if (!isalpha(s[i])){
            return false;
        }
    }
    return true;
    
}

bool is_number(const char *s)
{
    int l = strlen(s);
    for (int i = 0; i < l; i++)
    {
        if (!isdigit(s[i])){
            return false;
        }
    }
    return true;
    
}
void delete_cell(MemoryCell **cell)
{
    if (*cell == NULL)
    {
        return;
    }
    free((*cell)->name);
    free(*(cell));
    *cell = NULL;
}

void delete_cells(Cells *cells)
{
    for (int i = 0; i < cells->len; i++)
    {
        delete_cell(&(cells->cells[i]));
    }
    free(cells->cells);
}

int comparator(const void *a, const void *b)
{
    MemoryCell *left = *(MemoryCell **)a;
    MemoryCell *right = *(MemoryCell **)b;
    return strcmp(left->name, right->name);
}

statements add_cell(Cells *cells, MemoryCell *cell)
{
    if (cells->len + 1 > cells->buf)
    {
        cells->buf = cells->buf * 2;
        MemoryCell **temp = (MemoryCell **)realloc(cells->cells, (sizeof(MemoryCell *) * cells->buf));
        if (temp == NULL)
        {
            delete_cells(cells);
            return allocate_error;
        }
        cells->cells = temp;
    }
    (cells->cells)[(cells->len++)] = cell;
    qsort(cells->cells, cells->len, sizeof(MemoryCell *), comparator);
    return correct;
}

int bin_search(Cells *cells, const char *name)
{
    int a = -1, b = cells->len;
    while (b - a > 1)
    {
        int mid = (a + b) / 2;
        int mval = strcmp((cells->cells[mid])->name, name);
        //printf("%d, %d\n", mval, mid);
        if (mval < 0)
        {
            a = mid;
        }
        else if (mval > 0)
        {
            b = mid;
        }
        else
        {
            return mid;
        }
    }
    return -1;
}

void print_memory_cell(MemoryCell *cell)
{
    printf("%s = %d\n", cell->name, cell->val);
}

void print_vals(Cells *cells)
{
    printf("\nAll values of cells (%d) :\n", cells->len);
    for (int i = 0; i < cells->len; i++)
    {
        print_memory_cell(cells->cells[i]);
    }
    printf("\n");
}

bool ending_symb(char s)
{
    return (s == '=' || s == ';' || s == '\0' || s == '+' || s == '-' || s == '%' || s == '/' || s == ' ' || s == '*');
}

char *scan_lexema(FILE *file, char *ending)
{
    char *string;
    int buf = 128;
    int lent = 0;

    string = (char *)malloc(sizeof(char) * buf);

    if (string == NULL)
    {
        return NULL;
    }
    char symb;

    while ((symb = getc(file)) != EOF && !ending_symb(symb))
    {
        if (symb == '\n' || symb == ' ')
        {
            continue;
        }
        if (lent + 1 > buf)
        {
            buf *= 2;
            char *temp = (char *)realloc(string, sizeof(char) * buf);
            if (temp == NULL)
            {
                free(string);
                return NULL;
            }
            string = temp;
        }
        string[lent++] = symb;
    }
    *ending = symb;

    if (lent == 0)
    {
        free(string);
        return NULL;
    }
    if (lent != buf)
    {
        char *temp = (char *)realloc(string, sizeof(char) * (lent + 1));
        if (temp == NULL)
        {
            free(string);
            return NULL;
        }
        string = temp;
    }
    string[lent] = '\0';
    return string;
}

statements run(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        return invalid_file;
    }

    char *in;
    char ending;
    Cells cells;
    cells.buf = 1;
    cells.len = 0;
    cells.cells = (MemoryCell **)malloc(sizeof(MemoryCell *));
    
    if (cells.cells == NULL)
    {
        fclose(file);
        return allocate_error;
    }

    while ((in = scan_lexema(file, &ending)) != NULL)
    {
        //printf("%s\n", in);
        if (ending == '=')
        {
            if (!is_latin(in))
            {
                free(in);
                fclose(file);
                return invalid_input;
            }
            MemoryCell *cl;
            bool created = false;
            int position = bin_search(&cells, in);
            if (position == -1)
            {
                cl = create_cell(in, 0);
                created = !created;
            }
            else
            {
                cl = cells.cells[position];
            }

            free(in);
            in = NULL;
            char new_ending;
            do
            {
                in = scan_lexema(file, &new_ending);

                int delta = 0;
                if (is_latin(in))
                {
                    int position = bin_search(&cells, in);
                    if (position == -1)
                    {
                        free(in);
                        fclose(file);
                        return invalid_input;
                    }
                    delta = cells.cells[position]->val;
                }
                else if (is_number(in))
                {
                    delta = atoi(in);
                }
                else
                {
                    free(in);
                    fclose(file);
                    return invalid_input;
                }

                if (ending == '+')
                {
                    cl->val += delta;
                }
                else if (ending == '*')
                {
                    cl->val *= delta;
                }
                else if (ending == '-')
                {
                    cl->val -= delta;
                }
                else if (ending == '%')
                {
                    cl->val %= delta;
                }
                else if (ending == '/')
                {
                    cl->val /= delta;
                }
                else if (ending == '=')
                {
                    cl->val = delta;
                }
                ending = new_ending;
                free(in);
                in = NULL;
            } while (ending != ';');

            //print_memory_cell(cl);
            if (created)
            {
                add_cell(&cells, cl);
            }
        }
        else if (strcmp(in, "print") == 0)
        {
            free(in);
            in = NULL;
            if (ending == ' ')
            {
                in = scan_lexema(file, &ending);
                if (ending != ';' || !is_latin(in))
                {
                    free(in);
                    in = NULL;
                    delete_cells(&cells);
                    fclose(file);
                    return invalid_input;
                }
                int position = bin_search(&cells, in);

                if (position == -1)
                {
                    free(in);
                    in = NULL;
                    fclose(file);
                    delete_cells(&cells);
                    return invalid_input;
                }
                
                print_memory_cell(cells.cells[position]);
            }
            else
            {
                print_vals(&cells);
            }
        }

        if (in != NULL)
        {
            free(in);
        }
    }
    // printf("%s %c\n", in, ending);

    // print_vals(&cells);
    fclose(file);
    delete_cells(&cells);
    return correct;
}

int main(int argc, char *argv[])
{
    if (argc != 2){
        printf("Usage <%s> <filepath>\n", argv[0]);
        return 1;
    }
     //print_memory_cell(cells.cells[bin_search(&cells, "af")]);
    printf("exit code = %d\n", run(argv[1]));
}