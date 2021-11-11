#include <string.h>
#include <stdlib.h>

#include "desplazar.h"

void desplazar(char *str, int from, int nbytes, int by) {
  int k= strlen(str); // Definimos una variable para strlen(str)
  int b= nbytes; //Definimos una variable para nbytes
  if (by <= 0){
    char *i = str + from; // Puntero al inicio
    char *f = i + by; // Puntero al final
    while (b > 0){ 
        if (f - str >= 0) {
            *f = *i; // Ahora el puntero final será el puntero del inicio
        }
        i++; // Se le suma a i
        f++; // Se le suma a f
        b--; // Se le suma a b
    }
  }
  else{
    char *i = str + from + b - 1; // Puntero al inicio
    char *f = i + by; // Puntero al final
    while (b > 0){
        // Si el final menos el str son menores que el largo del string, entonces
        if (f - str < k) {
            *f = *i; // Ahora el puntero final será el puntero del inicio
        }
        i--; // Se le resta a i
        f--; // Se le resta a f
        b--; // Se le resta a b
    }
  }  
}

char *desplazamiento(char *str, int from, int nbytes, int by) {
  // Creamos una variable dinámica utilizando malloc de
  // el tamaño de char * el largo de str y a eso le sumamos 1
  char *nuevodes = malloc(sizeof(char)*strlen(str) + 1);
  // Copiamos el str en nuevodes
  strcpy(nuevodes,str);
  // Hacemos uso de la función desplazamiento
  desplazar(nuevodes,from,nbytes,by);
  // Retornamos nuevodes
  return nuevodes;
}
