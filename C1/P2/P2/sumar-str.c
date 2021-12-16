#include <stdio.h>
#include <string.h>

#include "sumar-str.h"

char sumarStr(char *a, char *b) {
    int len = strlen(a);
    char *c = a;
    char *d = a + len - 1;
    char *e = b + len - 1;
    int reserva = 0;
    int aux = 0;
    while(c <= d){
        int i = (*d - '0' + *e - '0')+ reserva; // creamos un int que sume todo
        reserva = 0; //seteamos a 0 reserva al igual que aux
        aux = 0;
        if(i > 9){
            reserva += (i%100)/10; //la reserva sera igual a si misma más (i%100)/10
            i = i%10; //mod 10
            aux +=1; //sumamos 1 a la variable aux
        }
        *d = i + '0'; //el entero d
        d--; //restamos 1 a d
        e--; //restamos 1 a e
    } 
    if(reserva != 0){
        return '1';
    }
    return '0';
}
