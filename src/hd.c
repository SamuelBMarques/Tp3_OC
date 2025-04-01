#include "../include/hd.h"
#include <stdio.h>


void criaHD(HD *hd) {
    hd->arquivo = fopen("HD.bin", "wb");
    if (hd->arquivo == NULL) {
        perror("Erro ao criar HD");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    BlocoMemoria bloco;

    for (int i = 0; i < hd->capacidade; i++) {
        // Preenche as palavras com valores aleatórios
        for (int j = 0; j < 4; j++) {
            bloco.palavras[j] = rand() % 100;
        }
        // Inicializa metadados
        bloco.endBloco = i;
        bloco.valido = 1;
        bloco.atualizado = 0;
        bloco.custo = 0;
        bloco.cacheHit = 0;
        bloco.ultimoAcesso = 0;

        // Escreve o bloco completo no HD
        fwrite(&bloco, sizeof(BlocoMemoria), 1, hd->arquivo);
    }

    fclose(hd->arquivo);
}


BlocoMemoria HD_getDado(HD *hd, int endBloco) {
    // Garante que o arquivo está aberto
    if (hd->arquivo == NULL) {
        hd->arquivo = fopen("HD.bin", "rb");
        if (hd->arquivo == NULL) {
            perror("Erro ao abrir HD");
            exit(EXIT_FAILURE);
        }
    }

    BlocoMemoria bloco;
    fseek(hd->arquivo, endBloco * sizeof(BlocoMemoria), SEEK_SET);
    fread(&bloco, sizeof(BlocoMemoria), 1, hd->arquivo);
    return bloco;
}