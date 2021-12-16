#include <stddef.h>

#include "prev.h"

void asignarPrev(Nodo *t, Nodo **pprev) {
    if(t == NULL){ //caso base NULL
        return;
    }
    else{
        asignarPrev(t->izq, pprev); //recursión izquierda
        t->prev = *pprev; // nodo previo de t prev es **prev
        t->prox = NULL; // asignamos NULL al proximo de momento
        if(t->prev != NULL){ //si el previo de t no es NULL
            t->prev->prox = t; // t es el nodo proximo del previo de t
        }    
        *pprev = t; // asignamos t a *pprev
        asignarPrev(t->der, pprev); //recursión derecha
    }    
}
