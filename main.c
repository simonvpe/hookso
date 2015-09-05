#include <stdlib.h>
#include <stdio.h>

int main(int argc, char * argv[]) {
  const int N = 10;
  int i;
  int *p = (int*)malloc(N*sizeof(int));
  printf("main: Putting %d random numbers in allocated memory\n", N);
  for(i = 0 ; i < N ; ++i) {
    p[i] = rand();
  }
  printf("main: The %d random numbers are ", N);
  for(i = 0 ; i < N ; ++i) {
    printf("%d,", p[i]);
  }
  printf("\n");
  return 0;
}
