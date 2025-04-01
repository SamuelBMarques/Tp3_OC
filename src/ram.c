#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/ram.h"

void criarRAM_vazia(RAM* ram, int tamanho) {
    ram->memoria = (BlocoMemoria*)malloc(tamanho * sizeof(BlocoMemoria));
    ram->tamanho = tamanho;
    for (int i = 0; i < tamanho; i++) {
        ram->memoria[i].endBloco = -1; // Bloco inválido
        ram->memoria[i].valido = 0;     // Não válido
        ram->memoria[i].atualizado = 0; // Não modificado
        for (int j = 0; j < 4; j++) {
            ram->memoria[i].palavras[j] = 0;
        }
    }
}

void criarRAM_aleatoria(RAM* ram, int tamanho) {
    ram->memoria = (BlocoMemoria*)malloc(tamanho * sizeof(BlocoMemoria));
    srand(time(NULL));
    for (int i = 0; i < tamanho; i++) {
        ram->memoria[i].endBloco = i;
        for (int j = 0; j < 4; j++) {
            ram->memoria[i].palavras[j] = rand();
        }
    }
}

void RAM_setDado(RAM* ram, int endereco, BlocoMemoria *conteudo) {
    ram->memoria[endereco] = *conteudo;
}

BlocoMemoria* RAM_getDado(RAM* ram, int endereco) {
    return &ram->memoria[endereco];
}

void RAM_imprimir(RAM* ram) {
    printf("Conteudo da RAM\n");
    for (int i = 0; i < ram->memoria->endBloco; i++) {
        imprimirUmBloco(ram->memoria[i]);
    }
    printf("\n");
}

void imprimirUmBloco(BlocoMemoria bloco) {
    for (int i = 0; i < 4; i++) {
        printf("%d,", bloco.palavras[i]);
    }
}

