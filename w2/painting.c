#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>


#define NUM_ROWS 1000
#define NUM_COLS 1000

#define PAINT 1
#define QUERY 2

// memory keeps colors in cells;
// color should be in ['R', 'L'];
char memory[NUM_ROWS][NUM_COLS];


void memory_filling() {
  for (int i = 0; i < NUM_ROWS; i++) {
    for (int j = 0; j < NUM_COLS; j++) {
      memory[i][j] = 'N';
    }
  }
}

// paint command looks like: 1 1 R
void paint() {
  // reading x, y and color
  int x, y;
  char color;
  scanf("%d %d %c\n", &x, &y, &color);
  
  // exec command
  // rewrite memory cell
  memory[x][y] = color;
}

// query command looks like: 1 1
// print color in (x,y) cell
void query() {
  // reading command
  int x, y;
  scanf("%d %d", &x, &y);

  // executing
  printf("%c\n", memory[x][y]);
}

int main() {
  memory_filling();

  int num_ops = 0;
  scanf("%d", &num_ops);
  
  for (int index = 0; index < num_ops; index++) {
    int index_of_operation;
    scanf("%d", &index_of_operation);

    switch (index_of_operation) {
      case PAINT:
        paint();
        break;
      case QUERY:
        query();
        break;
      default:
        fprintf(stderr, "command not found\n");
        exit(-1);
    }
  }

  return 0;
}
