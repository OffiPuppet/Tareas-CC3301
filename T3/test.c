#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "descomponer.h"

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
// revisar_int: verifica que un entero es un valor especifico

void revisar_int(int n, int nref) {
  if (n!=nref) {
    fprintf(stderr, "El valor retornado debio ser %d, no %d\n", nref, n);
    exit(1);
  }
}

// ----------------------------------------------------
// Funcion que construye un arbol

static Nodo *abb(int x, Nodo *izq, Nodo *der) {
  Nodo *a= malloc(sizeof(*a));
  a->x= x;
  a->izq= izq;
  a->der= der;
  return a;
}

// ----------------------------------------------------
// Funcion que verifica que 2 arboles binarios son iguales

static void revisar_abb(Nodo *a, Nodo *b) {
  if (a==NULL) {
    if (b==NULL)
      return;
    fprintf(stderr, "Arboles distintos: a es NULL, b tiene etiqueta %d\n",
                    b->x);
    exit(1);
  }
  else if (b==NULL) {
    fprintf(stderr, "Arboles distintos: b es NULL, a tiene etiqueta %d\n",
                    a->x);
    exit(1);
  }
  else {
    if (a->x!=b->x) {
      fprintf(stderr, "Etiquetas de arboles son distintas: %d != %d\n",
                      a->x, b->x);
      exit(1);
    }
    revisar_abb(a->izq, b->izq);
    revisar_abb(b->der, b->der);
  }
}

// ----------------------------------------------------
// Funcion que verifica que 2 arboles binarios sean el mismo

static void revisar_mismo_nodo(Nodo *a, Nodo *b) {
  if (a!=b) {
    if (a==NULL || b==NULL)
      fprintf(stderr, "No son el mismo arbol.  Uno es NULL, el otro no\n");
    else
      fprintf(stderr, "No son el mismo arbol.  Etiquetas son %d %d\n",
                      a->x, b->x);
    exit(1);
  }
}

// ----------------------------------------------------
// Funcion que verifica que los campos x de un arbol binario
// desbalanceado por la derecha sean i, i+1, ..., j

static void revisar_des_der(Nodo *a, int i, int j) {
  int ref= i;
  while (ref<=j) {
    if (a==NULL) {
      fprintf(stderr, "El arbol esta incorrectamente vacio\n");
      exit(1);
    }
    if (a->x!=ref) {
      fprintf(stderr, "x es %d, debio ser %d\n", a->x, ref);
      exit(1);
    }
    if (a->izq!=NULL) {
      fprintf(stderr, "Subarbol izquierdo debio ser NULL en nodo %d\n", a->x);
      exit(1);
    }
    a= a->der;
    ref++;
  }
  if (a!=NULL) {
    fprintf(stderr, "El arbol tiene mas nodos de lo que deberia\n");
    exit(1);
  }
}

// ----------------------------------------------------
// Funcion que verifica que los campos x de un arbol binario
// desbalanceado por la derecha sean i, i+1, ...,j

static void revisar_des_izq(Nodo *a, int i, int j) {
  int ref= j;
  while (i<=ref) {
    if (a==NULL) {
      fprintf(stderr, "El arbol esta incorrectamente vacio\n");
      exit(1);
    }
    if (a->x!=ref) {
      fprintf(stderr, "x es %d, debio ser %d\n", a->x, ref);
      exit(1);
    }
    if (a->der!=NULL) {
      fprintf(stderr, "Subarbol derecho debio ser NULL en nodo %d\n", a->x);
      exit(1);
    }
    a= a->izq;
    ref--;
  }
  if (a!=NULL) {
    fprintf(stderr, "El arbol tiene mas nodos de los que deberia\n");
    exit(1);
  }
}

// ----------------------------------------------------
// Revisa que los campos de un arbol sean i, i+1, i+2, etc.

static int revisar_campos(Nodo *a, int i) {
  if (a==NULL) {
    return i;
  }
  int j= revisar_campos(a->izq, i);
  if (a->x!=j) {
    fprintf(stderr, "El nodo con x=%d debio ser %d\n", a->x, j);
    exit(1);
  }
  j= revisar_campos(a->der, j+1);
  return j;
}

// ----------------------------------------------------
// Libera un arbol binario

static void liberar(Nodo *a) {
  if (a!=NULL) {
    liberar(a->izq);
    liberar(a->der);
    free(a);
  }
}

// ----------------------------------------------------
// Crea una copia de un arbol binario

static Nodo *copia(Nodo *a) {
  if (a==NULL)
    return NULL;
 
  Nodo *an= malloc(sizeof(Nodo));
  an->x= a->x;
  an->izq= copia(a->izq);
  an->der= copia(a->der);
  return an;
}

// ----------------------------------------------------
// Crea una copia condensada de un arbol binario con un solo malloc

static int contar_nodos(Nodo *a) {
  if (a==NULL)
    return 0;
  else
    return 1+contar_nodos(a->izq)+contar_nodos(a->der);
}

// ----------------------------------------------------
// Crea una copia condensada de un arbol binario con un solo malloc

static void duplicar(Nodo *a, Nodo **pdest) {
  Nodo *dest= *pdest;
  (*pdest)++;
  dest->x= a->x;
  if (a->izq==NULL)
    dest->izq=NULL;
  else {
    dest->izq= *pdest;
    duplicar(a->izq, pdest);
  }
  if (a->der==NULL)
    dest->der= NULL;
  else {
    dest->der= *pdest;
    duplicar(a->der, pdest);
  }
}

static Nodo *condensar(Nodo *a, Nodo *dest, Nodo **pfin) {
   Nodo *res= dest;
   duplicar(a, &dest);
   if (pfin!=NULL)
     *pfin= dest;
   return res;
}

static Nodo *compacto(Nodo *a, Nodo **pfin) {
   int n= contar_nodos(a);
   Nodo *dest= malloc(n*sizeof(Nodo));
   *pfin= dest;
   duplicar(a, pfin);
   return dest;
}

static void liberar_afuera(Nodo *a, Nodo *ini, Nodo *fin) {
  if ( a==NULL  ||  (ini<=a && a<fin) )
    return;
  else {
    liberar_afuera(a->izq, ini, fin);
    liberar_afuera(a->der, ini, fin);
    free(a);
  }
}

// ----------------------------------------------------
// Crea un arbol binario de busqueda bien equilibrado en donde 
// los x van de i a j

static Nodo *equilibrado(int i, int j) {
  if (i>j)
    return NULL;
  int k= (i+j+1)/2;
  Nodo *a= malloc(sizeof(Nodo));
  a->x= k;
  a->izq= equilibrado(i, k-1);
  a->der= equilibrado(k+1, j);
  return a;
}

// ----------------------------------------------------
// Crea un arbol binario desequilibrado por la izquierda en donde 
// los x van de i a j

static Nodo *desequilibrado_izq(int i, int j) {
  if (i>j) {
    return NULL;
  }
  Nodo *a= malloc(sizeof(Nodo));
  a->x= j;
  a->izq= desequilibrado_izq(i, j-1);
  a->der= NULL;
  return a;
}

// ----------------------------------------------------
// Crea un arbol binario desequilibrado por la derecha en donde 
// los x van de i a j

static Nodo *desequilibrado_der(int i, int j) {
  if (i>j) {
    return NULL;
  }
  Nodo *a= malloc(sizeof(Nodo));
  a->x= i;
  a->izq= NULL;
  a->der= desequilibrado_der(i+1, j);
  return a;
}

int main(int argc, char *argv[]) {
  printf("===============================\n");
  printf("Tests de la funcion descomponer\n");
  printf("===============================\n\n");
  {
    printf("Caso arbol nulo: debe continuar vacio\n");
    Nodo *a= NULL;
    Nodo dummy= {-100, NULL, NULL};
    Nodo *inf= &dummy, *sup= &dummy;
    descomponer(a, 1000, &inf, &sup);
    revisar_abb(inf, NULL);
    revisar_abb(sup, NULL);
    printf("Caso arbol de un nodo con x=5\n");
    a= abb(5, NULL, NULL);
    Nodo *ref= copia(a);
    inf= sup= &dummy;
    printf("Se descompone con z=5: inf=nodo con x=5, sup=NULL\n");
    descomponer(a, 5, &inf, &sup);
    revisar_abb(inf, ref);
    revisar_abb(sup, NULL);
    revisar_mismo_nodo(a, inf);
    printf("Se descompone con z=4: inf=NULL, sup= nodo con x=5\n");
    a= inf;
    inf= sup= &dummy;
    descomponer(a, 4, &inf, &sup);
    revisar_abb(inf, NULL);
    revisar_abb(sup, ref);
    revisar_mismo_nodo(a, sup);
    liberar(sup);
    liberar(ref);
  }

  {
    printf("Caso abb de un nodo x=1 con subarbol izquierdo x=2\n");
    Nodo dummy= {-200, NULL, &dummy};
    Nodo *inf= &dummy, *sup= &dummy;
    Nodo *n2= abb(2, NULL, NULL);
    Nodo *n1= abb(1, NULL, n2);
    Nodo *ref= copia(n1);
    Nodo *ref_n1= abb(1, NULL, NULL);
    Nodo *ref_n2= abb(2, NULL, NULL);
    printf("Se descompone con z=0: inf=NULL, sup=el mismo abb\n");
    descomponer(n1, 0, &inf, &sup);
    revisar_abb(inf, NULL);
    revisar_abb(sup, ref);
    revisar_mismo_nodo(n1, sup);
    printf("Se descompone con z=1: inf=nodo con x=1, sup=nodo con x=2\n");
    inf= sup= &dummy;
    descomponer(n1, 1, &inf, &sup);
    revisar_abb(inf, ref_n1);
    revisar_abb(sup, ref_n2);
    revisar_mismo_nodo(n1, inf);
    revisar_mismo_nodo(n2, sup);
    liberar(inf);
    liberar(sup);
    printf("Se descompone con z=2: inf=el mismo abb, sup=NULL\n");
    inf= sup= &dummy;
    n1= copia(ref);
    descomponer(n1, 2, &inf, &sup);
    revisar_abb(inf, ref);
    revisar_abb(sup, NULL);
    revisar_mismo_nodo(n1, inf);
    liberar(n1);
    liberar(ref);
    liberar(ref_n1);
    liberar(ref_n2);
  }

  {
    printf("Caso abb de un nodo x=2 con subarboles izquierdo y derecho\n");
    Nodo dummy= {-200, NULL, &dummy};
    Nodo *inf= &dummy, *sup= &dummy;
    Nodo *n1= abb(1, NULL, NULL);
    Nodo *n3= abb(3, NULL, NULL);
    Nodo *n123= abb(2, n1, n3);
    Nodo *ref_n123= copia(n123);
    Nodo *ref_n1= copia(n1);
    Nodo *ref_n3= copia(n3);
    Nodo *ref_n12= abb(2, copia(ref_n1), NULL);
    Nodo *ref_n23= abb(2, NULL, copia(ref_n3));
    printf("Se descompone con z=0: inf=NULL, sup=el mismo abb\n");
    descomponer(n123, 0, &inf, &sup);
    revisar_abb(inf, NULL);
    revisar_abb(sup, ref_n123);
    revisar_mismo_nodo(n123, sup);
    printf("Se descompone con z=1: inf=nodo con x=1, sup=nodos 2 y 3\n");
    inf= sup= &dummy;
    descomponer(n123, 1, &inf, &sup);
    revisar_abb(inf, ref_n1);
    revisar_abb(sup, ref_n23);
    revisar_mismo_nodo(n1, inf);
    revisar_mismo_nodo(n123, sup);
    liberar(inf);
    liberar(sup);
    printf("Se descompone con z=2: inf=nodos 1 y 2, sup=nodo con x=3\n");
    inf= sup= &dummy;
    n123= copia(ref_n123);
    descomponer(n123, 2, &inf, &sup);
    revisar_abb(inf, ref_n12);
    revisar_abb(sup, ref_n3);
    revisar_mismo_nodo(n123, inf);
    liberar(inf);
    liberar(sup);
    printf("Se descompone con z=3: inf=el mismo abb, sup=NULL\n");
    inf= sup= &dummy;
    n123= copia(ref_n123);
    descomponer(n123, 3, &inf, &sup);
    revisar_abb(inf, ref_n123);
    revisar_abb(sup, NULL);
    revisar_mismo_nodo(n123, inf);
    liberar(n123);
    liberar(ref_n123);
    liberar(ref_n1);
    liberar(ref_n3);
    liberar(ref_n12);
    liberar(ref_n23);
  }

  {
    printf("Caso abb del enunciado\n");
    Nodo dummy= {-200, NULL, &dummy};
    Nodo *inf= &dummy, *sup= &dummy;
    Nodo *n1= abb(1, NULL, NULL);
    Nodo *n3= abb(3, NULL, NULL);
    Nodo *n34= abb(4, n3, NULL);
    Nodo *n1234= abb(2, n1, n34);
    Nodo *n6= abb(6, NULL, NULL);
    Nodo *n67= abb(7, n6, NULL);
    Nodo *n1234567= abb(5, n1234, n67);
    Nodo *ref_n123= abb(2, copia(n1), copia(n3));
    Nodo *ref_n4567= abb(5, abb(4, NULL, NULL), copia(n67));
    printf("Se descompone con z=3\n");
    descomponer(n1234567, 3, &inf, &sup);
    revisar_abb(inf, ref_n123);
    revisar_abb(sup, ref_n4567);
    revisar_mismo_nodo(inf->izq, n1);
    revisar_mismo_nodo(inf, n1234);
    revisar_mismo_nodo(inf->der, n3);
    revisar_mismo_nodo(sup->izq, n34);
    revisar_mismo_nodo(sup, n1234567);
    revisar_mismo_nodo(sup->der, n67);
    revisar_mismo_nodo(sup->der->izq, n6);
    liberar(inf);
    liberar(sup);
    liberar(ref_n123);
    liberar(ref_n4567);
  }

  {
    int n= 100;
    printf("Caso abbs de %d nodos, equilibrado, desequilibrado por izquierda "
           "y derecha\n", n);
    Nodo *equilib= equilibrado(1, n);
    Nodo *deseq_izq= desequilibrado_izq(1, n);
    Nodo *deseq_der= desequilibrado_der(1, n);
    for (int z=0; z<=n; z++) {
      Nodo *a= copia(equilib);
      Nodo *inf, *sup;
      descomponer(a, z, &inf, &sup);
      revisar_int(revisar_campos(inf, 1), z+1);
      revisar_int(revisar_campos(sup, z+1), n+1);
      liberar(inf);
      liberar(sup);
      a= copia(deseq_izq);
      descomponer(a, z, &inf, &sup);
      revisar_int(revisar_campos(inf, 1), z+1);
      revisar_int(revisar_campos(sup, z+1), n+1);
      liberar(inf);
      liberar(sup);
      a= copia(deseq_der);
      descomponer(a, z, &inf, &sup);
      revisar_int(revisar_campos(inf, 1), z+1);
      revisar_int(revisar_campos(sup, z+1), n+1);
      liberar(inf);
      liberar(sup);

    }
    liberar(equilib);
    liberar(deseq_izq);
    liberar(deseq_der);
  }

  printf("==================================\n");
  printf("Tests de la funcion descomposicion\n");
  printf("==================================\n\n");

  {
    printf("Caso arbol nulo: debe continuar vacio\n");
    Nodo *a= NULL;
    Nodo dummy= {-300, NULL, NULL};
    Nodo *inf= &dummy, *sup= &dummy;
    descomposicion(a, 1000, &inf, &sup);
    revisar_abb(inf, NULL);
    revisar_abb(sup, NULL);
    printf("Caso arbol de un nodo con x=5\n");
    a= abb(5, NULL, NULL);
    Nodo *ref= copia(a);
    inf= sup= &dummy;
    printf("Se descompone con z=5: inf=nodo con x=5, sup=NULL\n");
    descomposicion(a, 5, &inf, &sup);
    revisar_abb(inf, ref);
    revisar_abb(sup, NULL);
    revisar_abb(a, ref);
    if (inf!=a)
      liberar(inf);
    printf("Se descompone con z=4: inf=NULL, sup= nodo con x=5\n");
    inf= sup= &dummy;
    descomposicion(a, 4, &inf, &sup);
    revisar_abb(inf, NULL);
    revisar_abb(sup, ref);
    revisar_abb(a, ref);
    if (sup!=a)
      liberar(sup);
    liberar(a);
    liberar(ref);
  }

  {
    printf("Caso abb de un nodo x=1 con subarbol izquierdo x=2\n");
    Nodo dummy= {-400, NULL, &dummy};
    Nodo *inf= &dummy, *sup= &dummy;
    Nodo *n1= abb(1, NULL, NULL);
    Nodo *n2= abb(2, NULL, NULL);
    Nodo *n12= abb(1, NULL, n2);
    printf("Se descompone con z=0: inf=NULL, sup=el mismo abb\n");
    Nodo *fin, *ini= compacto(n12, &fin);
    descomposicion(ini, 0, &inf, &sup);
    revisar_abb(inf, NULL);
    revisar_abb(sup, n12);
    revisar_abb(ini, n12);
    liberar_afuera(inf, ini, fin);
    liberar_afuera(sup, ini, fin);
    free(ini);
    printf("Se descompone con z=1: inf=nodo con x=1, sup=nodo con x=2\n");
    inf= sup= &dummy;
    ini= compacto(n12, &fin);
    descomposicion(ini, 1, &inf, &sup);
    revisar_abb(inf, n1);
    revisar_abb(sup, n2);
    revisar_abb(ini, n12);
    liberar_afuera(inf, ini, fin);
    liberar_afuera(sup, ini, fin);
    free(ini);
    printf("Se descompone con z=2: inf=el mismo abb, sup=NULL\n");
    inf= sup= &dummy;
    ini= compacto(n12, &fin);
    descomposicion(ini, 2, &inf, &sup);
    revisar_abb(inf, n12);
    revisar_abb(sup, NULL);
    revisar_abb(ini, n12);
    liberar_afuera(inf, ini, fin);
    liberar_afuera(sup, ini, fin);
    free(ini);
    free(n1);
    free(n2);
    free(n12);
  }

  {
    printf("Caso abb de un nodo x=2 con subarboles izquierdo y derecho\n");
    Nodo dummy= {-200, NULL, &dummy};
    Nodo *inf= &dummy, *sup= &dummy;
    Nodo *n1= abb(1, NULL, NULL);
    Nodo *n3= abb(3, NULL, NULL);
    Nodo *n123= abb(2, n1, n3);
    Nodo *n12= abb(2, n1, NULL);
    Nodo *n23= abb(2, NULL, n3);
    printf("Se descompone con z=0: inf=NULL, sup=el mismo abb\n");
    Nodo *fin, *ini= compacto(n123, &fin);
    descomposicion(ini, 0, &inf, &sup);
    revisar_abb(inf, NULL);
    revisar_abb(sup, n123);
    revisar_abb(ini, n123);
    liberar_afuera(inf, ini, fin);
    liberar_afuera(sup, ini, fin);
    free(ini);
    printf("Se descompone con z=1: inf=nodo con x=1, sup=nodos 2 y 3\n");
    inf= sup= &dummy;
    ini= compacto(n123, &fin);
    descomposicion(ini, 1, &inf, &sup);
    revisar_abb(inf, n1);
    revisar_abb(sup, n23);
    revisar_abb(ini, n123);
    liberar_afuera(inf, ini, fin);
    liberar_afuera(sup, ini, fin);
    free(ini);
    printf("Se descompone con z=2: inf=nodos 1 y 2, sup=nodo con x=3\n");
    inf= sup= &dummy;
    ini= compacto(n123, &fin);
    descomposicion(ini, 2, &inf, &sup);
    revisar_abb(inf, n12);
    revisar_abb(sup, n3);
    revisar_abb(ini, n123);
    liberar_afuera(inf, ini, fin);
    liberar_afuera(sup, ini, fin);
    free(ini);
    printf("Se descompone con z=3: inf=el mismo abb, sup=NULL\n");
    inf= sup= &dummy;
    ini= compacto(n123, &fin);
    descomposicion(ini, 3, &inf, &sup);
    revisar_abb(inf, n123);
    revisar_abb(sup, NULL);
    revisar_abb(ini, n123);
    liberar_afuera(inf, ini, fin);
    liberar_afuera(sup, ini, fin);
    free(ini);
    free(n1);
    free(n3);
    free(n123);
    free(n12);
    free(n23);
  }

  {
    printf("Caso abb del enunciado\n");
    Nodo dummy= {-200, NULL, &dummy};
    Nodo *inf= &dummy, *sup= &dummy;
    Nodo *n1= abb(1, NULL, NULL);
    Nodo *n3= abb(3, NULL, NULL);
    Nodo *n34= abb(4, n3, NULL);
    Nodo *n1234= abb(2, n1, n34);
    Nodo *n6= abb(6, NULL, NULL);
    Nodo *n67= abb(7, n6, NULL);
    Nodo *n1234567= abb(5, n1234, n67);
    Nodo *n123= abb(2, n1, n3);
    Nodo *n4= abb(4, NULL, NULL);
    Nodo *n4567= abb(5, n4, n67);
    printf("Se descompone con z=3\n");
    Nodo *fin, *ini= compacto(n1234567, &fin);
    descomposicion(ini, 3, &inf, &sup);
    revisar_abb(inf, n123);
    revisar_abb(sup, n4567);
    revisar_abb(ini, n1234567);
    liberar_afuera(inf, ini, fin);
    liberar_afuera(sup, ini, fin);
    free(ini);
    free(n1);
    free(n3);
    free(n34);
    free(n1234);
    free(n6);
    free(n67);
    free(n1234567);
    free(n123);
    free(n4);
    free(n4567);
  }

  {
    int n= 100;
    printf("Caso abbs de %d nodos, equilibrado, desequilibrado por izquierda "
           "y derecha\n", n);
    Nodo *eq= equilibrado(1, n);
    Nodo *fin_eq, *equilib= compacto(eq, &fin_eq);
    Nodo *izq= desequilibrado_izq(1, n);
    Nodo *fin_izq, *deseq_izq= compacto(izq, &fin_izq);
    Nodo *der= desequilibrado_der(1, n);
    Nodo *fin_der, *deseq_der= compacto(der, &fin_der);
    for (int z=0; z<=n; z++) {
      Nodo *inf, *sup;
      descomposicion(equilib, z, &inf, &sup);
      revisar_int(revisar_campos(inf, 1), z+1);
      revisar_int(revisar_campos(sup, z+1), n+1);
      revisar_abb(equilib, eq);
      liberar_afuera(inf, equilib, fin_eq);
      liberar_afuera(sup, equilib, fin_eq);

      descomposicion(deseq_izq, z, &inf, &sup);
      revisar_int(revisar_campos(inf, 1), z+1);
      revisar_int(revisar_campos(sup, z+1), n+1);
      revisar_abb(deseq_izq, izq);
      liberar_afuera(inf, deseq_izq, fin_izq);
      liberar_afuera(sup, deseq_izq, fin_izq);

      descomposicion(deseq_der, z, &inf, &sup);
      revisar_int(revisar_campos(inf, 1), z+1);
      revisar_int(revisar_campos(sup, z+1), n+1);
      revisar_abb(deseq_der, der);
      liberar_afuera(inf, deseq_der, fin_der);
      liberar_afuera(sup, deseq_der, fin_der);
    }
    liberar(eq); free(equilib);
    liberar(izq); free(deseq_izq);
    liberar(der); free(deseq_der);
  }

#ifdef BENCH
#define N1 20000
#define N2 14000

  printf("\n==================================\n");
  printf(  "Benchmark de descomponer\n");
  printf(  "==================================\n\n");

#else

#define N1 2000
#define N2 1400

#endif

  int tiempo_descomponer;

  {
    int n= N1;
    printf("arbol de %d nodos\n", n);
    Nodo *equilib= equilibrado(1, n);
    Nodo *deseq_izq= desequilibrado_izq(1, n);
    Nodo *deseq_der= desequilibrado_der(1, n);
    resetTime();
    for (int z=0; z<=n; z++) {
      Nodo *inf, *sup;
      Nodo *a= condensar(equilib, malloc(n*sizeof(Nodo)), NULL);
      descomponer(a, z, &inf, &sup);
      free(a);

      a=  condensar(deseq_izq, malloc(n*sizeof(Nodo)), NULL);
      descomponer(a, z, &inf, &sup);
      free(a);

      a=  condensar(deseq_der, malloc(n*sizeof(Nodo)), NULL);
      descomponer(a, z, &inf, &sup);
      free(a);

    }
    tiempo_descomponer= getTime();
    if (tiempo_descomponer!=0) {
      printf("Tiempo descomponer = %d milisegundos\n", tiempo_descomponer);
    }
    liberar(equilib);
    liberar(deseq_izq);
    liberar(deseq_der);
  }

#ifdef BENCH
  printf("\n==================================\n");
  printf(  "Benchmark de descomposicion\n");
  printf(  "==================================\n\n");
#endif

  int tiempo_descomposicion;

  {
    int n= N2;
    printf("arbol de %d nodos\n", n);
    Nodo *eq= equilibrado(1, n);
    Nodo *fin_eq, *equilib= compacto(eq, &fin_eq);
    liberar(eq);
    Nodo *izq= desequilibrado_izq(1, n);
    Nodo *fin_izq, *deseq_izq= compacto(izq, &fin_izq);
    liberar(izq);
    Nodo *der= desequilibrado_der(1, n);
    Nodo *fin_der, *deseq_der= compacto(der, &fin_der);
    liberar(der);
    resetTime();
    for (int z=0; z<=n; z++) {
      Nodo *inf, *sup;
      descomposicion(equilib, z, &inf, &sup);
      liberar_afuera(inf, equilib, fin_eq);
      liberar_afuera(sup, equilib, fin_eq);

      descomposicion(deseq_izq, z, &inf, &sup);
      liberar_afuera(inf, deseq_izq, fin_izq);
      liberar_afuera(sup, deseq_izq, fin_izq);

      descomposicion(deseq_der, z, &inf, &sup);
      liberar_afuera(inf, deseq_der, fin_der);
      liberar_afuera(sup, deseq_der, fin_der);
    }
    tiempo_descomposicion= getTime();
    if (tiempo_descomposicion!=0) {
      printf("Tiempo descomposicion = %d milisegundos\n",
             tiempo_descomposicion);
    }
    free(equilib);
    free(deseq_izq);
    free(deseq_der);
  }

 if (argc>=2) {
    if (strcmp(argv[1], "ref")==0) {
      FILE *out= fopen("tiempo-ref.txt", "w");
      if (out==NULL) {
        perror("tiempo-ref.txt");
        exit(1);
      }
      fprintf(out, "%d %d\n", tiempo_descomponer, tiempo_descomposicion);
      fclose(out);
    }
    else if (argc>=3) {
      int tiempo_descomponer_ref= atoi(argv[1]);
      int tiempo_descomposicion_ref= atoi(argv[2]);
      double q= (double)tiempo_descomponer/(double)tiempo_descomponer_ref;
      int porciento= (q-1.)*100+0.5;
      printf("Tiempo de referencia para descomponer = %d milisegundos\n",
             tiempo_descomponer_ref);
      printf("Porcentaje de sobrecosto de descomponer: %d %%\n", porciento);
      if (porciento>80) {
        fflush(stdout);
        fprintf(stderr, "Lo siento: su solucion es demasiado lenta\n");
        exit(1);
      }
      q= (double)tiempo_descomposicion/(double)tiempo_descomposicion_ref;
      porciento= (q-1.)*100+0.5;
      printf("Tiempo de referencia para descomposicion = %d milisegundos\n",
             tiempo_descomposicion_ref);
      printf("Porcentaje de sobrecosto de descomposicion: %d %%\n", porciento);
      if (porciento>80) {
        fflush(stdout);
        fprintf(stderr, "Lo siento: su solucion es demasiado lenta\n");
        exit(1);
      }
    }
  }

#ifdef BENCH
  printf("Felicitaciones: su solucion es correcta y eficiente\n");
#else
  printf("Felicitaciones: su solucion es correcta\n");
#endif

  return 0;
}
