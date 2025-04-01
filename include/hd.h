#ifndef HD_H
#define HD_H

#include "../include/blocoMemoria.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    FILE *arquivo;      // Ponteiro para o arquivo binário
    int capacidade;     // Número total de blocos no HD (ex: 10000)
} HD;

BlocoMemoria HD_getDado(HD* hd, int endBloco);
void criaHD();

#endif