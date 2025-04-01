#ifndef BLOCONMEMORIA_H
#define BLOCONMEMORIA_H

#include <time.h>  // Para usar a função time()

typedef struct {
    int valido;  // 1 = válido, 0 = vazio
    int endBloco;
    int atualizado;
    int custo;
    int cacheHit;
    int palavras[4];
    int ultimoAcesso;
} BlocoMemoria;

typedef struct {
    BlocoMemoria * lines;
    int numLinhas;
}MemorySet;

typedef struct{
    int numConjuntos;
    MemorySet * memorySet;
}Cache;

#endif // BLOCONMEMORIA_H