#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "sumar-str.h"

#pragma GCC diagnostic ignored "-Wunused-function"

// ----------------------------------------------------
// Funcion que entrega el tiempo transcurrido desde el lanzamiento del
// programa en milisegundos

static long long time0= 0;

static long long getTime0() {
#ifdef RISCV
    return 0;
#else
    struct rusage ru;
    int rc= getrusage(RUSAGE_SELF, &ru);
    if (rc!=0) {
      perror("getTime");
      exit(1);
    }
    return (long long)ru.ru_utime.tv_sec*1000000+ru.ru_utime.tv_usec;
#endif
}

static void resetTime() {
  time0= getTime0();
}

static int getTime() {
  return (getTime0()-time0+500)/1000;
}

// ----------------------------------------------------
// revisar_str: verifica que un string sea un valor especifico

static void revisar_str(char *n, char *nref) {
  if (strcmp(n, nref)!=0) {
    fprintf(stderr, "El valor retornado debio ser %s, no %s\n", nref, n);
    exit(1);
  }
}

// ----------------------------------------------------
// revisar_char: verifica que un char sea un valor especifico

static void revisar_char(char n, char nref) {
  if (n!=nref) {
    fprintf(stderr, "El valor retornado debio ser %c, no %c\n", nref, n);
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  printf("===============================\n");
  printf("Tests de la funcion sumaDecimal\n");
  printf("===============================\n\n");

  char c;

  printf("Test: 2+4 = 6\n");
  char dec6[]="2";
  c= sumarStr(dec6, "4");
  revisar_str(dec6, "6");
  revisar_char(c, '0');

  printf("Test: 25+14 = 39\n");
  char dec39[]="25";
  c= sumarStr(dec39, "14");
  revisar_str(dec39, "39");
  revisar_char(c, '0');

  printf("Test: 09+09 = 18\n");
  char dec18[]="09";
  c= sumarStr(dec18, "09");
  revisar_str(dec18, "18");
  revisar_char(c, '0');

  printf("Test: 9+9 = 8, acarreo = 1\n");
  char dec8[]="9";
  c= sumarStr(dec8, "9");
  revisar_str(dec8, "8");
  revisar_char(c, '1');

  printf("Test: 37+45 = 82\n");
  char dec82[]="37";
  c= sumarStr(dec82, "45");
  revisar_str(dec82, "82");
  revisar_char(c, '0');

  printf("Test: 068+045 = 113\n");
  char dec113[]="068";
  c= sumarStr(dec113, "045");
  revisar_str(dec113, "113");
  revisar_char(c, '0');

  printf("Test: 68+45 = 13, acarreo = 1\n");
  char dec13[]="68";
  c= sumarStr(dec13, "45");
  revisar_str(dec13, "13");
  revisar_char(c, '1');

  printf("Test: 09999+00001 = 10000\n");
  char dec10000[]="09999";
  c= sumarStr(dec10000, "00001");
  revisar_str(dec10000, "10000");
  revisar_char(c, '0');

  printf("Test: 9999+0001 = 0000, acarreo = 1\n");
  char dec0000[]="9999";
  // char *dec0000= malloc(5);
  // strcpy(dec0000, "9999");
  c= sumarStr(dec0000, "0001");
  revisar_str(dec0000, "0000");
  revisar_char(c, '1');

  printf("Test: \n"
   "38596871930585839127139305840012839471393048462829373474833612373738373 +\n"
   "83849483475066030018127839340018273482920387465483493938264849595959574\n"
   "= 22446355405651869145267145180031112954313435928312867413098461969697947\n"
   "acarreo = 1\n");
  char dec_big[]=
    "38596871930585839127139305840012839471393048462829373474833612373738373";
  c= sumarStr(dec_big,
    "83849483475066030018127839340018273482920387465483493938264849595959574");
  revisar_str(dec_big,
    "22446355405651869145267145180031112954313435928312867413098461969697947");
  revisar_char(c, '1');

  printf("Felicitaciones: su solucion es correcta\n\n");

  return 0;
}
