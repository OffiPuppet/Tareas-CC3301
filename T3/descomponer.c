#include <stdlib.h>

#include "descomponer.h"

void descomponer(Nodo* t, int z, Nodo **pinf, Nodo **psup) {
  if(t == NULL){
    *pinf = t;
    *psup = t;
    return;
  }
  if(t->x <= z){
    descomponer(t->der, z, pinf, psup);
    t->der = *pinf;
    *pinf = t;
  }
  else{
    descomponer(t->izq, z, pinf, psup);
    t->izq = *psup;
    *psup = t;
  }
}


void descomposicion(Nodo *t, int z, Nodo **pinf, Nodo **psup) {
  if (t == NULL){
    *pinf = t;
    *psup = t;
    return;
  }
  if(t->x <= z){
    Nodo *e = malloc(sizeof(Nodo));
    e->x = t->x;
    e->der = t->der;
    descomposicion(e->der, z, pinf, psup);
    e->izq = t->izq;
    e->der = *pinf;
    *pinf = e;
  }
  else{
    Nodo *f = malloc(sizeof(Nodo));
    f->x = t->x;
    f->izq = t->izq;
    descomposicion(f->izq, z, pinf, psup);
    f->der = t->der;
    f->izq = *psup;
    *psup = f;
  }
}
