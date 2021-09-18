#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

void change_array(int* array, int len) {
  for (int i = 0; i < len; i++) {
    array[i] = (array[i] % 2 == 0) ? (array[i] * 2) : (array[i] + 2);
  } 
} 

void print_array(int* array, int len) {
  for (size_t i = 0; i < len; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");
}

int main() {
  int len_array = 0;
  int* array = (int*)malloc(len_array * sizeof(int));
  scanf("%d", &len_array);
  
  for (int i = 0; i < len_array; i++) {
    int number; 
    scanf("%d", &number);
    array[i] = number;
 } 
 
  change_array(array, len_array);
  print_array(array, len_array);
  
  free(array);
  return 0;
}
