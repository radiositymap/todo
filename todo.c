#include <stdio.h>
#include <string.h>

#define COL_R "\x1b[31m"
#define COL_G "\x1b[32m"
#define COL_Y "\x1b[33m"
#define COL_B "\x1b[34m"
#define COL_M "\x1b[35m"
#define COL_C "\x1b[36m"
#define COL_X "\x1b[00m"

enum Board {
    BACKLOG,
    TODO,
    DOING,
    DONE
};

typedef struct itemStruct {
    char desc[60];
    char due[10];
    int board;
} Item;

void testDisplay(void) {
    printf(COL_R "Test" COL_X "\n");
    printf(COL_G "Test" COL_X "\n");
    printf(COL_Y "Test" COL_X "\n");
    printf(COL_B "Test" COL_X "\n");
    printf(COL_M "Test" COL_X "\n");
    printf(COL_X "Test" COL_X "\n");
}

void displayHeader(void) { // colour codes are counted as extra chars
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
        printf("%-25s%-25s%-25s\n", todo[i].desc, doing[i].desc, done[i].desc);
        printf(COL_B "%25s%25s%25s\n" COL_X, todo[i].due, doing[i].due, done[i].due);
    }
}


int main() {
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

    return 0;
}
