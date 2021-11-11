// _DEFAULT_SOURCE se necesita para usar random
#define _DEFAULT_SOURCE 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "recortar.h"

// ----------------------------------------------------
// Funcion que entrega el tiempo de CPY usado desde
// que se invoco resetTime()

static long long time0= 0;

static long long getTime0() {
#ifndef RISCV
    struct rusage ru;
    int rc= getrusage(RUSAGE_SELF, &ru);
    if (rc!=0) {
      perror("getTime");
      exit(1);
    }
    return (long long)ru.ru_utime.tv_sec*1000000+ru.ru_utime.tv_usec;
#else
    return 0;
#endif
}

static void resetTime() {
  time0= getTime0();
}

static int getTime() {
  return (getTime0()-time0+500)/1000;
}

// ----------------------------------------------------
// La misma funcionalidad de recortar pero con
// multiplicaciones, divisiones y modulo

static uint recortar_lento(unsigned int x, int i, int j) {
  uint dosEi= pow(2, i);
  uint dosEj= pow(2, j);
  uint der= x % dosEj;
  uint izq= x / dosEi / 2;
  return izq*dosEj + der;
}

// ----------------------------------------------------
// Benchmark

typedef struct {
  uint x;
  int i, j;
} Param;
 
int main(int argc, char *argv[]) {
  int tiempo_ref= 0;
  int verbose= 1;     // si se despliegan los mensajes de las etapas
  int bench= 0;       // si se hace o no el benchmark
  int ntests= 100000; // numero de tests para cada i y j

#ifdef RISCV
  ntests /= 10;
#endif

  if (argc==2) {
    bench= 1;
    ntests /= 10;
    if (strcmp(argv[1], "ref")==0)
      verbose= 0;
    else
      tiempo_ref= atoi(argv[1]);
  }
    
  if (verbose)
    printf("Tests unitarios\n");

  // Test a.1
  // recortar(x, 0, 0)==r1, con x== 'puros unos' y r1== '31 unos'
  uint r1= recortar(0xffffffff, 0, 0);
  if (r1!=0x7fffffff) {
    fprintf(stderr, "test a.1 falla: dio 0x%x en vez de 0x7fffffff\n", r1);
    exit(1);
  }

  // Test a.2
  // recortar(x, 31, 0)==r2, con x== 'puros unos' y r2== 0
  // ¡Cuidado! Recuerde que no puede desplazar x en 32 o mas bits.
  uint r2= recortar(0xffffffff, 31, 0);
  if (r2!=0) {
    fprintf(stderr, "test a.2 falla: dio 0x%x en vez de 0\n", r2);
    exit(1);
  }

  // Test a.3
  // recortar(0b0110 1010 1111 0101 0010 1100, 19, 8)==0b0110 0010 1100)
  // debe borrar          ^^^^ ^^^^
  uint r3= recortar(0x006af52c, 19, 8);
  if (r3!=0x62c) {
    fprintf(stderr, "test a.3 falla: dio 0x%x en vez de 0x62c\n", r3);
    exit(1);
  }

  const int bits_uint= sizeof(uint)*8;
  srandom(1000);
  int tot_tests= 0;

  for (int i= 1; i<bits_uint; i++) {
    for (int j= 0; j<i; j++) {
      tot_tests += ntests;
    }
  }

  if (verbose)
    printf("Aprobado\n");

  if (verbose)
    printf("Test exhaustivo con %d invocaciones de recortar\n", tot_tests);

  Param *params= NULL, *p= NULL;
  uint crc_ref= 0;
  if (bench) {
    params= malloc(tot_tests*sizeof(Param));
    p= params;
  }

  for (int i= 1; i<bits_uint; i++) {
    for (int j= 0; j<i; j++) {
      for (int k= 0; k<ntests; k++) {
        uint r= random();
        uint x = r*2 + (k&1); // El ultimo bit no es realmente aleatorio

        uint test= recortar(x, i, j);
        uint ref= recortar_lento(x, i, j);
        crc_ref += ref;
        if (test!=ref) {
          fflush(stdout);
          fprintf(stderr, "Valor incorrecto: para x=%x i=%d j=%d\n",
                  x, i, j);
          fprintf(stderr, "recortar retorna 0x%x cuando debio ser 0x%x\n",
                  test, ref);
          exit(1);
        }

        if (bench) {
          p->x= x;
          p->i= i;
          p->j= j;
          p++;
        }
      }
    }
  }
  if (verbose)
    printf("Aprobado\n");

  if (bench) {
    if (verbose) {
      printf("Corriendo benchmark\n");
      printf("Tiempo de cpu usado por la version del profesor: "
             "%d milisegundos\n", tiempo_ref);
    }
    Param *params_top= &params[tot_tests];
    resetTime();
    for (int k= 0; k<100; k++) {
      uint crc= 0;
      for (p= params; p<params_top; p++) {
        crc += recortar(p->x, p->i, p->j);
      }
      if (crc!=crc_ref) {
        fflush(stdout);
        fprintf(stderr, "Los resultados son inconsistentes: con los mismos\n");
        fprintf(stderr, "argumentos se entregan resultados distintos\n");
        exit(1);
      }
    }
    int tiempo= getTime();
    free(params);
    if (verbose) {
      printf("Tiempo de cpu usado por su version: %d milisegundos\n", tiempo);
      double q= (double)tiempo/(double)tiempo_ref;
      int porciento= (q-1.)*100;
      printf("Porcentaje de sobrecosto: %d %%\n", porciento);
      if (porciento>80) {
        fflush(stdout);
        fprintf(stderr, "Lo siento: su solucion es demasiado lenta\n");
        exit(1);
      }
    }
    else
      printf("%d\n", tiempo);
    if (verbose)
      printf("Aprobado\n");
  }

  if (verbose)
    printf("Felicitaciones, todos los tests funcionan correctamente\n");
  return 0;
}
