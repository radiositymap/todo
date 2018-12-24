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
    BOARD_I
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
    int count = 0;
    char *pos;

    token = strtok(line, delim);
    while (token != NULL && !isspace(token[0])) {
        strcpy(tokens[count], token);
        token = strtok(NULL, delim);
        count++;
    }

    // trim \n off last token
    if ((pos=strchr(tokens[count-1], '\n')) != NULL)
        *pos = '\0';

    return count;
}

int read(Item boards[MAX_BOARDS][MAX_ITEMS], char **board_names) {

        // get no of boards and board names
        char line[500];
        // read to boards[][]
        int board_i = 0;
        int item_i = 0;
        int num_boards;

        if (fgets(line, sizeof(line), f))
            printf("%s", line);
        //(*board_names)[MAX_BOARDS];
        num_boards = tokenise(line, DELIM, board_names);

        fgets(line, sizeof(line), f);

        //while (fgets(line, sizeof(line), f)) {
            //char *tokens[MAX_BOARDS];
            //int num_field;

            //num_field = tokenise(line, DELIM, tokens);
            /*

            if (num_field <= 0) {
                board_i++;
                item_i=0;
            }

            if (num_field >= 2) {
                strcpy(boards[board_i][item_i].desc, tokens[DESC_I]);
                strcpy(boards[board_i][item_i++].due, tokens[DUE_I]);
            }
            */
        //}

        fclose(f);


        int i;
        for (i=0; i<MAX_BOARDS; i++) {
            if (board_names[i] == NULL)
                break;
            printf("a %s ", board_names[i]);
        }
        printf("\n");



        // print boards to test
        //int i, j;
        //for (i=0; i<num_boards; i++) {
        //    printf("%d\n", i);
        //    for (j=0; j<2; j++) {
        //        printf("desc: %s, due: %s \n",
        //                boards[i][j].desc, boards[i][j].due);
        //    }
        //}

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

// add board
// delete board
// move item
int main() {
    Item boards[MAX_BOARDS][MAX_ITEMS] = {{{{0}}}};

    // read file for no of boards
    char **board_names = init_str_arr(MAX_BOARDS, MAX_LENGTH);

    int num_boards = read(boards, board_names);
    printf("no of boards: %d\n", num_boards);

    int i;
    for (i=0; i<MAX_BOARDS; i++) {
        if (board_names[i] == NULL)
            break;
        printf("%s ", board_names[i]);
    }
    printf("\n");

    /*
    Item todo[10] = {{0}};
    Item doing[10] = {{0}};
    Item done[10] = {{0}};
    size_t todo_i=0;
    size_t doing_i=0;
    size_t done_i=0;

    Item item1 = {"Include time lib", "20181214", TODO};
    Item item2 = {"Time sorting", "20181215", TODO};
    Item item3 = {"Display", "20181213", DONE};
    Item item4 = {"Sort by Board", "20181214", DOING};
    Item items[] = {item1, item2, item3, item4};

    sort(items, 4, todo, &todo_i, doing, &doing_i, done, &done_i);

    displayHeader();
    display(todo, todo_i, doing, doing_i, done, done_i);

    // write to file
    FILE *f;
    f = fopen("list.txt", "w");
    if (f == NULL) {
    printf("Can't open file\n");
    exit(1);
    }
    write(f, "TODO", todo, todo_i);
    write(f, "DOING", doing, doing_i);
    write(f, "DONE", done, done_i);
    fclose(f);
     */

    return 0;
}
