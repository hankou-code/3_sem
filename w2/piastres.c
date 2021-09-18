#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

int main() {
  int num_ships = 0;
  int* ships = (int*)malloc(num_ships * sizeof(int));
  int sum_chests = 0;
  
  scanf("%d", &num_ships);

  for (int index_ship = 0; index_ship < num_ships; index_ship++) {
    int num_chests_in_ship;
    scanf("%d", &num_chests_in_ship);
    ships[index_ship] = num_chests_in_ship;

    sum_chests += num_chests_in_ship;
  }

  int avg_num_chests = sum_chests / num_ships;

  for (int index_ship = 0; index_ship < num_ships; index_ship++) {
    printf("%d ", avg_num_chests - ships[index_ship]);
  }

  free(ships);
  return 0;
}
