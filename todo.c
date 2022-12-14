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
#define DELIM "`"
#define MAX_BOARDS 10
#define MAX_ITEMS 50
#define MAX_LENGTH 60
#define WIDTH 25

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

void display_help() {
    printf("Commands:\n");
    printf("- a: add item.              format: <description> <time> a\n");
    printf("- d: delete item.           format: <boardId> <itemId> d\n");
    printf("- n: move to next board.    format: <boardId> <itemId> n\n");
    printf("- b: move to prev board.    format: <boardId> <itemId> b\n");
    printf("- h: help\n\n");
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
        fprintf(f, "%s" DELIM "%s\n", items[i].desc, items[i].due);
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
    int i, j, k;
    int empty_count = 0;

    // display header
    for (i=0; i<num_boards; i++) {
        if (i==0)
            printf(" ");
        printf(" %s%*s" COL_X, colours[i], -WIDTH, board_names[i]);
    }
    printf("\n ");
    for (j=0; j<num_boards; j++) {
        printf(" ");
        for (i=0; i<25; i++)
            printf("???");
    }
    printf("\n");
    for (j=0; j<10; j++) {
        //empty_count = 0;

        // print index if row is not empty
        for (i=0; i<num_boards; i++) {
            if (strlen(boards[i][j].desc) != 0) {
                printf("%d ", j);
                break;
            }
        }
        // print desc in multiple rows
        for (k=0; k<3; k++) {
            empty_count=0;
            for (i=0; i<num_boards; i++) {
                if (strlen(boards[i][j].desc) == 0) {
                    empty_count++;
                    if (empty_count >= num_boards) {
                        printf("\n");
                        return;
                    }
                }
                if (i>0)
                    printf(" ");
                else if (k>0)
                    printf("  ");
                printf("%-*.*s", WIDTH, WIDTH, boards[i][j].desc+WIDTH*k);
            }
            printf("\n");
        }
        // print due date
        printf(" ");
        for (i=0; i<num_boards; i++) {
            printf(COL_B " %*s" COL_X, WIDTH, boards[i][j].due);
        }
        printf("\n ");
        for (k=0; k<num_boards; k++) {
            printf(" ");
            for (i=0; i<25; i++)
                printf("???");
        }
        printf("\n");
    }
}

void write_boards(Item boards[MAX_BOARDS][MAX_ITEMS],
        char **board_names, int num_boards) {
    FILE *f;
    int i, j;
    if (f = fopen(FILENAME, "w")) {
        for (i=0; i<num_boards-1; i++)
            fprintf(f, "%s" DELIM, board_names[i]);
        fprintf(f, "%s\n\n", board_names[i]);

        for (i=0; i<num_boards; i++) {
            fprintf(f, "%s\n", board_names[i]);
            for (j=0; j<MAX_ITEMS; j++)
                if (strlen(boards[i][j].desc) > 0) {
                    fprintf(f, "%s" DELIM "%s\n",
                        boards[i][j].desc, boards[i][j].due);
                } else {
                    fprintf(f, "\n");
                    break;
                }
        }

        fclose(f);
    }
}

int add_board(char **board_names, int num_boards, char board_name[]) {
    strcpy(board_names[num_boards], board_name);
    return num_boards + 1;
}

int delete_board(Item boards[MAX_BOARDS][MAX_ITEMS],
        char **board_names, int num_boards, char board_name[]) {
    int match_i, i, j;
    Item temp_item;
    char temp_str[MAX_LENGTH];

    for (match_i=0; match_i<num_boards; match_i++)
        if (strcmp(board_name, board_names[match_i]) == 0)
            break;
        else
            printf("not same: %s\n", board_names[match_i]);

    if (match_i >= num_boards) /* no match, return */
        return num_boards;

    /* shift all valid boards thereafter left */
    printf("match: %d\n", i);
    for (i=match_i; i<num_boards-1; i++) {
        for (j=0; j<MAX_ITEMS; j++) {
            if (strlen(boards[i+1][j].desc) > 0)
                boards[i][j] = boards[i+1][j];
        }
        strcpy(board_names[i], board_names[i+1]);
    }

    if (i<num_boards) {
        for (j=0; j<MAX_ITEMS; j++)
            boards[i][j] = (const Item){0};
        board_names[i] = malloc(MAX_LENGTH * sizeof(char));
    }

    return num_boards-1;
}

/* insert into correct place next time */
void add_item(Item boards[MAX_BOARDS][MAX_ITEMS],
        int boardId, char desc[], char due[]) {
    int i;
    for (i=0; i<MAX_ITEMS; i++) {
        if (strlen(boards[boardId][i].desc) <= 0) {
            strcpy(boards[boardId][i].desc, desc);
            strcpy(boards[boardId][i].due, due);
            break;
        }
    }
}

void delete_item(Item boards[MAX_BOARDS][MAX_ITEMS],
        int boardId, int itemId) {
    int i;
    for (i=itemId; i<MAX_ITEMS-1; i++) {
        if (strlen(boards[boardId][i+1].desc) > 0) {
            printf("deleting\n");
            strcpy(boards[boardId][i].desc, boards[boardId][i+1].desc);
            strcpy(boards[boardId][i].due, boards[boardId][i+1].due);
        }
        else
            break;
    }
    memset(boards[boardId][i].desc, 0, strlen(boards[boardId][i].desc));
    memset(boards[boardId][i].due, 0, strlen(boards[boardId][i].due));
}

// move item by n boards
void move_item(Item boards[MAX_BOARDS][MAX_ITEMS],
        int boardId, int itemId, int n) {
    Item item = boards[boardId][itemId];
    add_item(boards, boardId + n, item.desc, item.due);
    delete_item(boards, boardId, itemId);
}

// add item
// delete item
// move item to next board
int main(int argc, char **argv) {
    Item boards[MAX_BOARDS][MAX_ITEMS] = {{{{0}}}};
    char **board_names = init_str_arr(MAX_BOARDS, MAX_LENGTH);

    // read file for no of boards
    int num_boards = read(boards, board_names);
    printf("no of boards: %d\n", num_boards);

    if (num_boards <= 0) {
        printf("No boards. Creating boards.\n");
        num_boards = add_board(board_names, num_boards, "TODO");
        num_boards = add_board(board_names, num_boards, "DOING");
        num_boards = add_board(board_names, num_boards, "DONE");
        write_boards(boards, board_names, num_boards);
        return 0;
    }

    //print_str_arr(board_names, num_boards);
    //print_items(boards);

    //display(boards, board_names, num_boards);

    num_boards = read(boards, board_names);

    // read args
    if (argc == 2 && argv[1][0] == 'h') {
        display_help();
    }
    else {
        if (argc >= 4) {
            switch (argv[3][0]) {
                case 'a':
                    add_item(boards, 0, argv[1], argv[2]);
                    break;
                case 'd':
                    delete_item(boards, atoi(argv[1]), atoi(argv[2]));
                    break;
                case 'n':
                    move_item(boards, atoi(argv[1]), atoi(argv[2]), 1);
                    break;
                case 'b':
                    move_item(boards, atoi(argv[1]), atoi(argv[2]), -1);
            }
        }
    }

    //num_boards = delete_board(boards, board_names, num_boards, "Backlog");
    //add_item(boards, "Testing", "randtime");
    //delete_item(boards, 2, 0);
    //move_item(boards, 0, 1);

    write_boards(boards, board_names, num_boards);
    display(boards, board_names, num_boards);

    free_str_arr(board_names, MAX_BOARDS);

    return 0;
}
