#include <stdio.h>
#include "handler.h"
#include "calculator.h"
#include "explorer.h"
#include "config.h"

int main(int argc, char* argv[]) {
  for (int i = MIN_THREADS; i <= MAX_THREADS; i++) {
    write_data_to_the_file(i, FLAG_ACCUMULATE | FLAG_MULTIPLY);
  }

  build_graph();
  return 0;
}