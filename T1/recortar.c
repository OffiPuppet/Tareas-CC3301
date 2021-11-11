#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "recortar.h"

uint recortar(unsigned int x, int i, int j) {
  uint der= x - ((x >> j) << j);
  uint izq= x >> i >> 1;
  return (izq << j) + der;
}
