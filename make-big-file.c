#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc!=2) {
    fprintf(stderr, "Uso: %s <archivo-de-salida>\n", argv[0]);
    exit(1);
  }
  char *fname= argv[1];
  FILE *big= fopen(fname, "w");
  if (big==NULL) {
    perror(fname);
    exit(1);
  }
  if (fseek(big, 0x1000000000, SEEK_SET)<0) {
    perror(fname);
    exit(1);
  }
  if (fprintf(big, "hello world!\n")<0) {
    perror(fname);
    exit(1);
  }
  if (fclose(big)<0) {
    perror(fname);
    exit(1);
  }
  return 0;
}
