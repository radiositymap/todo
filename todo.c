#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define COL_R "\x1b[31m"
#define COL_G "\x1b[32m"
#define COL_Y "\x1b[33m"
#define COL_B "\x1b[34m"
#define COL_M "\x1b[35m"
#define COL_C "\x1b[36m"
#define COL_X "\x1b[00m"

#define FILENAME "list.txt"
#define DELIM ","
#define MAX_BOARDS 10
#define MAX_ITEMS 50
#define MAX_LENGTH 60

const char colours[][6] = {
    COL_R,
    COL_Y,
    COL_G,
    COL_C,
    COL_M
};

enum ItemFields {
    DESC_I,
    DUE_I,
    NUM_FIELDS
};

enum Boards {
    BACKLOG,
    TODO,
    DOING,
    DONE
};

typedef struct ItemStruct {
    char desc[60];
    char due[10];
    int board;
} Item;


char** init_str_arr(size_t arr_size, size_t str_len);
void free_str_arr(char **str_arr, size_t arr_size);

void test_display(void) {
    printf(COL_R "Test" COL_X "\n");
    printf(COL_G "Test" COL_X "\n");
    printf(COL_Y "Test" COL_X "\n");
    printf(COL_B "Test" COL_X "\n");
    printf(COL_M "Test" COL_X "\n");
    printf(COL_X "Test" COL_X "\n");
}

void sort(Item* items, size_t items_i,
        Item* todo, size_t* todo_i, 
        Item* doing, size_t* doing_i,
        Item* done, size_t* done_i) {

    int i;
    for (i=0; i<items_i; i++) {
        switch (items[i].board) {
            case TODO:
                todo[(*todo_i)++] = items[i];
                break;
            case DOING:
                doing[(*doing_i)++] = items[i];
                break;
            case DONE:
                done[(*done_i)++] = items[i];
                break;
            default:
                printf("Invalid board!");
        }
    }
}

void print(Item* items, size_t size) {
    int i;
    for (i=0; i<size; i++) {
        printf("%-30s%-30s%20d\n", 
                items[i].desc, items[i].due, items[i].board);
    }
}

int max(size_t a, size_t b) {
    return a >= b ? a : b;
}



void write(FILE *f, char* header, Item* items, size_t size) {
    int i;
    fprintf(f, "%s\n", header);
    for (i=0; i<size; i++) {
        fprintf(f, "%s,%s\n", items[i].desc, items[i].due);
    }
}

int tokenise(char line[], char delim[], char **tokens) {
    char *token;
    char *pos;
    int count = 0;

    token = strtok(line, delim);

    while (token != NULL && !isspace(token[0])) {
        strcpy(tokens[count], token);
        token = strtok(NULL, delim);
        count++;
    }

    // trim \n off last token
    if (count > 0 && (pos=strchr(tokens[count-1], '\n')) != NULL)
        *pos = '\0';

    return count;
}

int read(Item boards[MAX_BOARDS][MAX_ITEMS], char **board_names) {
    FILE *f;

    if (f = fopen(FILENAME, "r")) {

        char line[500];
        int board_i = -1;
        int item_i = 0;
        int num_boards;
        char **tokens = init_str_arr(NUM_FIELDS, MAX_LENGTH);
        int num_fields;

        /* get number of boards and board names */
        fgets(line, sizeof(line), f);
        num_boards = tokenise(line, DELIM, board_names);

        /* populate boards with file data */
        while (fgets(line, sizeof(line), f)) {

            num_fields = tokenise(line, DELIM, tokens);

            if (num_fields <= 0) {
                board_i++;
                item_i=0;
            }

            if (num_fields == NUM_FIELDS) {
                strcpy(boards[board_i][item_i].desc, tokens[DESC_I]);
                strcpy(boards[board_i][item_i++].due, tokens[DUE_I]);
            }
        }
        free_str_arr(tokens, NUM_FIELDS);

        fclose(f);
        return num_boards;
    }

    else {
        printf("File doesn't exist\n");
        return 0;
    }
}

char ** init_str_arr(size_t arr_size, size_t str_len) {
    char **str_arr;
    int i;

    str_arr = malloc(arr_size * sizeof(char*));
    for (i=0; i<arr_size; i++)
        str_arr[i] = malloc(str_len * sizeof(char));

    return str_arr;
    
}

void free_str_arr(char **str_arr, size_t arr_size) {
    int i;
    for (i=0; i<arr_size; i++)
        free(str_arr[i]);
    free(str_arr);
}

void print_str_arr(char **str_arr, size_t len) {
    int i;
    for (i=0; i<len; i++)
        printf("%d: %s\n", i, str_arr[i]);
}

void print_items(Item boards[MAX_BOARDS][MAX_ITEMS]) {
    int i, j;
    for (i=0; i<MAX_BOARDS; i++) {
        printf("%d\n", i);
        for (j=0; j<MAX_ITEMS; j++)
            if (strlen(boards[i][j].desc) > 0)
                printf("%s %s\n", boards[i][j].desc, boards[i][j].due);
    }
}

void display(Item boards[MAX_BOARDS][MAX_ITEMS],
        char ** board_names, size_t num_boards) {
    int i, j;
    int empty_count = 0;

    // display header
    for (i=0; i<num_boards; i++)
        printf("%s%-25s" COL_X, colours[i], board_names[i]);
    printf("\n");

    for (j=0; j<10; j++) {
        empty_count = 0;
        for (i=0; i<num_boards; i++) {
            if (strlen(boards[i][j].desc) == 0) {
                empty_count++;
                if (empty_count >= num_boards) {
                    printf("\n");
                    return;
                }
            }
            printf("%-25s", boards[i][j].desc);
        }
        printf("\n");
        for (i=0; i<num_boards; i++) {
            printf(COL_B "%25s" COL_X, boards[i][j].due);
        }
        printf("\n");
    }
}

// add board
// delete board
// move item
int main() {
    Item boards[MAX_BOARDS][MAX_ITEMS] = {{{{0}}}};

    // read file for no of boards
    char **board_names = init_str_arr(MAX_BOARDS, MAX_LENGTH);

    int num_boards = read(boards, board_names);
    printf("no of boards: %d\n", num_boards);

    print_str_arr(board_names, num_boards);
    print_items(boards);

    display(boards, board_names, num_boards);

    free_str_arr(board_names, MAX_BOARDS);

    return 0;
}
