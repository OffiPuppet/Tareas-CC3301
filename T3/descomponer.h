typedef struct nodo {
  int x;
  struct nodo *izq, *der;
} Nodo;

void descomponer(Nodo* t, int z, Nodo **pinf, Nodo **psup);
void descomposicion(Nodo *t, int z, Nodo **pinf, Nodo **psup);
