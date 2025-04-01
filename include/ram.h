#ifndef RAM_H
#define RAM_H

#include "blocoMemoria.h"

typedef struct {
    BlocoMemoria* memoria;
    int tamanho;
} RAM;

BlocoMemoria RAM_getDado(RAM* ram, int endBloco);
void RAM_setDado(RAM* ram, int endBloco, BlocoMemoria* bloco);
void RAM_imprimir(RAM* ram);
void criarRAM_aleatoria(RAM* ram, int tamanho);
void criarRAM_vazia(RAM* ram, int tamanho);
void imprimirUmBloco(BlocoMemoria bloco);

#endif // RAM_H