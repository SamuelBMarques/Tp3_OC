
#include "../include/mmu.h"
#include "../include/hd.h"
#include <stdlib.h>
#include <stdio.h>

static int contadorLRU = 0; // Contador global para rastrear o último acesso

// Função para atualizar o tempo de acesso de um bloco
void atualizarUltimoAcesso(Cache *cache, int conjunto, int linha) {
    cache->memorySet[conjunto].lines[linha].ultimoAcesso = contadorLRU++;
}

// Função para encontrar o conjunto usado a menos tempo
int encontrarLRU(Cache *cache, int conjunto) {
    int lruIndex = 0;
    int menorTempo = cache->memorySet[conjunto].lines[0].ultimoAcesso;

    for (int i = 0; i < 2; i++) { // Como são 2 vias, percorremos apenas as 2 linhas
        if (cache->memorySet[conjunto].lines[i].ultimoAcesso < menorTempo) {
            menorTempo = cache->memorySet[conjunto].lines[i].ultimoAcesso;
            lruIndex = i;
        }
    }
    return lruIndex;
}

int encontrarBlocoNaCache(Cache *cache, int conjunto, int endBloco) {
    for (int i = 0; i < 2; i++) {
        if (cache->memorySet[conjunto].lines[i].endBloco == endBloco) {
            return i;
        }
    }
    return -1;
}

BlocoMemoria* MMU_buscarNasMemorias(Endereco *e, RAM* ram, Cache* L1, Cache* L2, Cache* L3) {
    int numConjuntosL1 = L1->numConjuntos;
    int numConjuntosL2 = L2->numConjuntos;
    int numConjuntosL3 = L3->numConjuntos;
    int posicaoCache1 = e->endBloco % numConjuntosL1;
    int posicaoCache2 = e->endBloco % numConjuntosL2;
    int posicaoCache3 = e->endBloco % numConjuntosL3;
    int custo = 0;

    // Verifica na Cache L1 (2 posições por conjunto)
    for (int i = 0; i < 2; i++) {
        if (L1->memorySet[posicaoCache1].lines[i].endBloco == e->endBloco) {
            custo = 10;
            L1->memorySet[posicaoCache1].lines[i].custo = custo;
            L1->memorySet[posicaoCache1].lines[i].cacheHit = 1;
            return &L1->memorySet[posicaoCache1].lines[i];
        }
    }

    // Verifica na Cache L2 (2 posições por conjunto)
    for (int i = 0; i < 2; i++) {
        if (L2->memorySet[posicaoCache2].lines[i].endBloco == e->endBloco) {
            custo = 110;
            L2->memorySet[posicaoCache2].lines[i].cacheHit = 2;
            BlocoMemoria* bloco =  MMU_movCache2Cache1(posicaoCache1, posicaoCache2, L1, L2,L3, custo,i);
            return bloco;
        }
    }

    // Verifica na Cache L3 (2 posições por conjunto)
    for (int i = 0; i < 2; i++) {
        if (L3->memorySet[posicaoCache3].lines[i].endBloco == e->endBloco) {
            custo = 1110;
            L3->memorySet[posicaoCache3].lines[i].cacheHit = 3;
            BlocoMemoria* bloco = MMU_movCache3Cache2(posicaoCache2, posicaoCache3, L2, L3, custo,i);
            for (int i = 0; i < 2; i++) {
                if (L2->memorySet[posicaoCache2].lines[i].endBloco == e->endBloco) {
                    MMU_movCache2Cache1(posicaoCache1, posicaoCache2, L1, L2,L3, custo,i);
                }
            }
            return bloco;
        }
    }

    BlocoMemoria* bloco_ram = RAM_getDado(ram, e->endBloco);
    if (bloco_ram->endBloco == e->endBloco) {
        custo = 11110;
        // Move para L3 e propagar
        BlocoMemoria* bloco = MMU_movRamCache3(posicaoCache3, L3, ram, e, custo);
        for (int i = 0; i < 2; i++) {
            if (L3->memorySet[posicaoCache3].lines[i].endBloco == e->endBloco) {
                MMU_movCache3Cache2(posicaoCache2, posicaoCache3, L2, L3, custo, i);
                for (int j = 0; j < 2; j++) {
                    if (L2->memorySet[posicaoCache2].lines[j].endBloco == e->endBloco) {
                        MMU_movCache2Cache1(posicaoCache1, posicaoCache2, L1, L2, L3, custo, j);
                    }
                }
            }
        }
        return bloco;
    }


    // Se não encontrou em nenhuma cache, busca na RAM e move para a Cache L3
    custo = 111110;
    BlocoMemoria* bloco = MMU_movHDParaRAM(e->endBloco, ram, custo);
    
    // Agora move para L3
    bloco = MMU_movRamCache3(posicaoCache3, L3, ram, e, custo);
    bloco->cacheHit = 5;
    for (int i = 0; i < 2; i++) {
        if (L3->memorySet[posicaoCache3].lines[i].endBloco == e->endBloco) {
            MMU_movCache3Cache2(posicaoCache2, posicaoCache3, L2, L3, custo, i);
            for (int j = 0; j < 2; j++) {
                if (L2->memorySet[posicaoCache2].lines[j].endBloco == e->endBloco) {
                    MMU_movCache2Cache1(posicaoCache1, posicaoCache2, L1, L2, L3, custo, j);
                }
            }
        }
    }
    return bloco;
}

BlocoMemoria* MMU_movCache2Cache1(int posicaoCache1, int posicaoCache2, Cache* L1, Cache* L2, Cache* L3, int custo, int posicao) {
    int lruIndex = 0;
    int linhaFonte = encontrarBlocoNaCache(L2, posicaoCache2, L2->memorySet[posicaoCache2].lines[posicao].endBloco);
    // Encontra uma posição livre na Cache L1
    for (int i = 0; i < 2; i++) {
        if (!L1->memorySet[posicaoCache1].lines[i].valido) { // Verifica se a linha está vazia
            L1->memorySet[posicaoCache1].lines[i] = L2->memorySet[posicaoCache2].lines[linhaFonte];
            L1->memorySet[posicaoCache1].lines[i].valido = 1; // Marca como válida
            L1->memorySet[posicaoCache1].lines[i].custo = custo;
            L1->memorySet[posicaoCache1].lines[i].cacheHit = 2;
            atualizarUltimoAcesso(L1, posicaoCache1, i);
            return &L1->memorySet[posicaoCache1].lines[i];
        }
    }

    // Substitui o LRU do conjunto correto
    lruIndex = encontrarLRU(L1, posicaoCache1);
    // Verifica se o bloco substituído foi atualizado e precisa ser escrito de volta na L2
    if (L1->memorySet[posicaoCache1].lines[lruIndex].atualizado) {
        MMU_movCache1ParaCache2(posicaoCache2, posicaoCache1, L2, L1, custo, L3);
    }
    L1->memorySet[posicaoCache1].lines[lruIndex] = L2->memorySet[posicaoCache2].lines[linhaFonte];
    L1->memorySet[posicaoCache1].lines[lruIndex].valido = 1; // Adicione
    L1->memorySet[posicaoCache1].lines[lruIndex].atualizado = 0;
    L1->memorySet[posicaoCache1].lines[lruIndex].custo = custo;
    L1->memorySet[posicaoCache1].lines[lruIndex].cacheHit = 2; // Encontrado na L2
    atualizarUltimoAcesso(L1, posicaoCache1, lruIndex);

    return &L1->memorySet[posicaoCache1].lines[lruIndex];
}

BlocoMemoria* MMU_movCache3Cache2(int posicaoCache2, int posicaoCache3, Cache* L2, Cache* L3, int custo, int posicao) {
    int lruIndex = 0;
    int linhaFonte = encontrarBlocoNaCache(L3, posicaoCache3, L3->memorySet[posicaoCache3].lines[posicao].endBloco);
    // Encontra posição livre na Cache L2
    for (int i = 0; i < 2; i++) {
        if (L2->memorySet[posicaoCache2].lines[i].endBloco == -1) {
            L2->memorySet[posicaoCache2].lines[i] = L3->memorySet[posicaoCache3].lines[linhaFonte];
            L2->memorySet[posicaoCache2].lines[i].custo = custo;
            L2->memorySet[posicaoCache2].lines[i].cacheHit = 3; // Encontrado na L3
            atualizarUltimoAcesso(L2, posicaoCache2, i);
            return &L2->memorySet[posicaoCache2].lines[i];
        }
    }

    // Substitui LRU na L2
    lruIndex = encontrarLRU(L2, posicaoCache2);
    // Verifica se o bloco substituído foi atualizado e precisa ser escrito de volta na L3
    if (L2->memorySet[posicaoCache2].lines[lruIndex].atualizado) {
        MMU_movCache2ParaCache3(posicaoCache3, posicaoCache2, L3, L2, custo);
    }
    L2->memorySet[posicaoCache2].lines[lruIndex] = L3->memorySet[posicaoCache3].lines[linhaFonte];
    L2->memorySet[posicaoCache2].lines[lruIndex].custo = custo;
    L2->memorySet[posicaoCache2].lines[lruIndex].valido = 1; // Adicione
    L2->memorySet[posicaoCache2].lines[lruIndex].atualizado = 0; 
    L2->memorySet[posicaoCache2].lines[lruIndex].cacheHit = 3; // Encontrado na L3
    atualizarUltimoAcesso(L2, posicaoCache2, lruIndex);

    return &L2->memorySet[posicaoCache2].lines[lruIndex];
}

BlocoMemoria* MMU_movRamCache3(int posicaoCache3, Cache* L3, RAM* ram, Endereco *e, int custo) {
    int lruIndex = 0;
    // Tenta encontrar posição livre na L3
    for (int i = 0; i < 2; i++) {
        if (L3->memorySet[posicaoCache3].lines[i].endBloco == -1) {
            BlocoMemoria* blocoRAM = RAM_getDado(ram, e->endBloco);
            L3->memorySet[posicaoCache3].lines[i] = *blocoRAM;
            L3->memorySet[posicaoCache3].lines[i].cacheHit = 4;
            L3->memorySet[posicaoCache3].lines[i].custo = custo;
            atualizarUltimoAcesso(L3, posicaoCache3, i);
            return &L3->memorySet[posicaoCache3].lines[i];
        }
    }

    // Substitui LRU na L3
    lruIndex = encontrarLRU(L3, posicaoCache3);
    // Move o bloco substituído da L3 para a RAM (se atualizado)
    if (L3->memorySet[posicaoCache3].lines[lruIndex].atualizado) {
        RAM_setDado(ram, L3->memorySet[posicaoCache3].lines[lruIndex].endBloco,
                    &L3->memorySet[posicaoCache3].lines[lruIndex]);
    }

    // Insere novo bloco da RAM na L3
    BlocoMemoria* novoBlocoRAM = RAM_getDado(ram, e->endBloco);
    L3->memorySet[posicaoCache3].lines[lruIndex] = *novoBlocoRAM; // Atribuição por valor
    L3->memorySet[posicaoCache3].lines[lruIndex].cacheHit = 4;
    L3->memorySet[posicaoCache3].lines[lruIndex].custo = custo;
    atualizarUltimoAcesso(L3, posicaoCache3, lruIndex);

    return &L3->memorySet[posicaoCache3].lines[lruIndex];
}

BlocoMemoria* MMU_movHDParaRAM(int endBloco, RAM* ram, int custo) {
    int buffer[4]; // Buffer para armazenar as 4 palavras do HD
    if (!HD_getDado(endBloco, buffer)) { // Usar a nova HD_getDado
        printf("Erro ao ler do HD!\n");
        exit(EXIT_FAILURE);
    }

    int ram_index = -1;

    // Busca por espaço vazio ou substituição LRU
    for (int i = 0; i < ram->tamanho; i++) {
        if (ram->memoria[i].endBloco == -1) {
            ram_index = i;
            break;
        }
    }

    if (ram_index == -1) {
        // Substituição LRU
        int menorAcesso = ram->memoria[0].ultimoAcesso;
        ram_index = 0;
        for (int i = 1; i < ram->tamanho; i++) {
            if (ram->memoria[i].ultimoAcesso < menorAcesso) {
                menorAcesso = ram->memoria[i].ultimoAcesso;
                ram_index = i;
            }
        }

        // Escreve o bloco substituído de volta no HD (se atualizado)
        if (ram->memoria[ram_index].atualizado) {
            FILE *file = fopen("hd.bin", "rb+");
            fseek(file, ram->memoria[ram_index].endBloco * sizeof(BlocoMemoria), SEEK_SET);
            fwrite(&ram->memoria[ram_index], sizeof(BlocoMemoria), 1, file);
            fclose(file);
        }
    }

    // Carrega o bloco do HD para a RAMs
    ram->memoria[ram_index].endBloco = endBloco; // Define o endBloco manualmente
    ram->memoria[ram_index].valido = 1;
    ram->memoria[ram_index].atualizado = 0;
    ram->memoria[ram_index].cacheHit = 5;
    ram->memoria[ram_index].custo = custo;
    ram->memoria[ram_index].ultimoAcesso = contadorLRU++;
    for (int i = 0; i < 4; i++) {
        ram->memoria[ram_index].palavras[i] = buffer[i]; // Copia as palavras
    }

    return &ram->memoria[ram_index];
}

BlocoMemoria* MMU_movCache2ParaCache3(int posicaoCache3, int posicaoCache2, Cache* L3, Cache* L2, int custo) {
    int lruIndex = 0;
    int linhaFonte = encontrarBlocoNaCache(L2, posicaoCache2, L2->memorySet[posicaoCache2].lines[0].endBloco);
    // Encontra posição livre na L3
    for (int i = 0; i < 2; i++) {
        if (L3->memorySet[posicaoCache3].lines[i].endBloco == -1) {
            L3->memorySet[posicaoCache3].lines[i] = L2->memorySet[posicaoCache2].lines[0];
            L3->memorySet[posicaoCache3].lines[i].custo = custo;
            atualizarUltimoAcesso(L3, posicaoCache3, i);
            return &L3->memorySet[posicaoCache3].lines[i];
        }
    }

    // Substitui LRU na L3
    lruIndex = encontrarLRU(L3, posicaoCache3);
    L3->memorySet[posicaoCache3].lines[lruIndex] = L2->memorySet[posicaoCache2].lines[linhaFonte];
    atualizarUltimoAcesso(L3, posicaoCache3, lruIndex);

    return &L3->memorySet[posicaoCache3].lines[lruIndex];
}

BlocoMemoria* MMU_movCache1ParaCache2(int posicaoCache2, int posicaoCache1, Cache* L2, Cache* L1, int custo, Cache* L3){
    int lruIndex = 0;
    int linhaFonte = encontrarBlocoNaCache(L1, posicaoCache1, L1->memorySet[posicaoCache1].lines[0].endBloco);
    // Encontra posição livre na L2
    for (int i = 0; i < 2; i++) {
        if (L2->memorySet[posicaoCache2].lines[i].endBloco == -1) {
            L2->memorySet[posicaoCache2].lines[i] = L1->memorySet[posicaoCache1].lines[0];
            L2->memorySet[posicaoCache2].lines[i].custo = custo;
            atualizarUltimoAcesso(L2, posicaoCache2, i);
            return &L2->memorySet[posicaoCache2].lines[i];
        }
    }

    // Substitui LRU na L2
    lruIndex = encontrarLRU(L2, posicaoCache2);
    // Verifica se o bloco substituído foi atualizado e precisa ser escrito de volta na L3
    if (L2->memorySet[posicaoCache2].lines[lruIndex].atualizado) {
        MMU_movCache2ParaCache3(posicaoCache2, posicaoCache2, L3, L2, custo);
    }
    L2->memorySet[posicaoCache2].lines[lruIndex] = L1->memorySet[posicaoCache1].lines[linhaFonte];
    atualizarUltimoAcesso(L2, posicaoCache2, lruIndex);

    int endBloco = L1->memorySet[posicaoCache1].lines[0].endBloco;
    int posicaoCache3 = endBloco % L3->numConjuntos;
    // Move o bloco substituído da L2 para L3
    MMU_movCache2ParaCache3(posicaoCache3, posicaoCache2, L3, L2, custo);

    return &L2->memorySet[posicaoCache2].lines[lruIndex];
}