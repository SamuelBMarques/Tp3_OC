#include "../include/hd.h"
#include <stdio.h>

#define NUM_BLOCOS 10000
#define PALAVRAS_POR_BLOCO 4

void criarHD() {
    FILE *file = fopen("hd.bin", "wb");
    if (!file) {
        printf("Erro ao criar o HD!\n");
        return;
    }

    srand(time(NULL));
    int linha[PALAVRAS_POR_BLOCO];

    for (int i = 0; i < NUM_BLOCOS; i++) {
        for (int j = 0; j < PALAVRAS_POR_BLOCO; j++) {
            linha[j] = rand() % 100;  // Gera valores entre 0 e 99
        }
        fwrite(linha, sizeof(int), PALAVRAS_POR_BLOCO, file);
    }

    fclose(file);
}

int HD_getDado(int endBloco, int *buffer) {
    FILE *file = fopen("hd.bin", "rb");
    if (!file) {
        printf("Erro ao abrir o HD!\n");
        return 0;  // Retorna falha
    }

    // Cada bloco ocupa 4 inteiros no arquivo, então buscamos na posição correta
    fseek(file, endBloco * 4 * sizeof(int), SEEK_SET);
    fread(buffer, sizeof(int), 4, file);

    fclose(file);
    return 1;  // Retorna sucesso
}