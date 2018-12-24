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
    COL_C,
    COL_R,
    COL_Y,
    COL_G,
    COL_M
};

enum ItemFields {
    DESC_I,
    DUE_I,
    BOARD_I,
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

void test_display(void) {
    printf(COL_R "Test" COL_X "\n");
    printf(COL_G "Test" COL_X "\n");
    printf(COL_Y "Test" COL_X "\n");
    printf(COL_B "Test" COL_X "\n");
    printf(COL_M "Test" COL_X "\n");
    printf(COL_X "Test" COL_X "\n");
}

void display_header(void) { // colour codes are counted as extra chars
    printf("%-30s%-30s%-30s\n", 
            COL_R "TODO", COL_Y "DOING", COL_G "DONE" COL_X);
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

void display(Item* todo, size_t todo_i,
        Item* doing, size_t doing_i,
        Item* done, size_t done_i) {
    int i;
    int max_i = max(todo_i, max(doing_i, done_i));
    for (i=0; i<max_i; i++) {
        printf("%-25s%-25s%-25s\n",
                todo[i].desc, doing[i].desc, done[i].desc);
        printf(COL_B "%25s%25s%25s\n" COL_X, 
                todo[i].due, doing[i].due, done[i].due);
    }
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

        // get no of boards and board names
        char line[500];
        // read to boards[][]
        int board_i = 0;
        int item_i = 0;
        int num_boards;

        if (fgets(line, sizeof(line), f))
            printf("%s", line);
        num_boards = tokenise(line, DELIM, board_names);


        while (fgets(line, sizeof(line), f)) {
            //char *tokens[MAX_BOARDS];
            char **tokens = init_str_arr(NUM_FIELDS, MAX_LENGTH);
            int num_fields;

            printf("line: %s\n", line);
            num_fields = tokenise(line, DELIM, tokens);

            printf("fields: %d\n", num_fields);
            if (num_fields <= 0) {
                board_i++;
                item_i=0;
            }

            if (num_fields >= 2) {
                strcpy(boards[board_i][item_i].desc, tokens[DESC_I]);
                strcpy(boards[board_i][item_i++].due, tokens[DUE_I]);
            }
        }

        fclose(f);

        return num_boards;
    }

    else {
        printf("File doesn't exist\n");
        return 0;
    }
}

char** init_str_arr(size_t arr_size, size_t str_len) {
    char **str_arr;
    int i;

    str_arr = malloc(arr_size * sizeof(char*));
    for (i=0; i<arr_size; i++)
        str_arr[i] = malloc(str_len * sizeof(char));

    return str_arr;
    
}

void print_str_arr(char **str_arr, size_t len) {
    int i;
    for (i=0; i<len; i++)
        printf("%d: %s\n", i, str_arr[i]);
}

void print_items(Item boards[MAX_BOARDS][MAX_ITEMS]) {
    int i, j;
    for (i=0; i<MAX_BOARDS; i++)
        for (j=0; j<MAX_ITEMS; j++)
            printf("%s %s\n", boards[i][j].desc, boards[i][j].due);
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

    //print_str_arr(board_names, 10);
    //print_items(boards);

    return 0;
}
