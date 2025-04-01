#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../include/ram.h"
#include "../include/cpu.h"
#include "../include/instrucao.h"
#include "../include/geradorInstrucoes.h"
#include "../include/hd.h"

void programaAleatorioRepeticoes(RAM* ram, CPU* cpu) {
    Instrucao* trecho1 = (Instrucao*)malloc(10001 * sizeof(Instrucao));
    
    FILE* file = fopen("programa.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char linha[256];
    int index = 0;
    while (fgets(linha, sizeof(linha), file)) {
        Instrucao* umaInstrucao = (Instrucao*)malloc(sizeof(Instrucao));
        char* palavras[7];
        char* token = strtok(linha, ":");
        int i = 0;
        while (token != NULL) {
            palavras[i++] = token;
            token = strtok(NULL, ":");
        }

        umaInstrucao->opcode = atoi(palavras[0]);

        Endereco* e1 = (Endereco*)malloc(sizeof(Endereco));
        e1->endBloco = atoi(palavras[1]);
        e1->endPalavra = atoi(palavras[2]) % 4;
        umaInstrucao->add1 = e1;

        Endereco* e2 = (Endereco*)malloc(sizeof(Endereco));
        e2->endBloco = atoi(palavras[3]);
        e2->endPalavra = atoi(palavras[4]) % 4;
        umaInstrucao->add2 = e2;

        Endereco* e3 = (Endereco*)malloc(sizeof(Endereco));
        e3->endBloco = atoi(palavras[5]);
        e3->endPalavra = atoi(palavras[6]) % 4;
        umaInstrucao->add3 = e3;

        trecho1[index++] = *umaInstrucao;
    }
    fclose(file);

    // Add halt instruction
    Instrucao* umaInstrucao = (Instrucao*)malloc(sizeof(Instrucao));
    umaInstrucao->opcode = -1;
    trecho1[10000] = *umaInstrucao;

    criarRAM_aleatoria(ram, 1000);
    CPU_setPrograma(cpu, trecho1);
    CPU_iniciar(cpu, ram, 16, 32, 64);
}

void programaAleatorio(RAM* ram, CPU* cpu, int qdeInstrucoes, int tamanhoRam) {
    Instrucao* trecho1 = (Instrucao*)malloc(qdeInstrucoes * sizeof(Instrucao));
    srand(time(NULL));

    for (int i = 0; i < qdeInstrucoes - 1; i++) {
        Instrucao* umaInstrucao = (Instrucao*)malloc(sizeof(Instrucao));
        umaInstrucao->opcode = rand() % 2;

        Endereco* add1 = (Endereco*)malloc(sizeof(Endereco));
        add1->endBloco = rand() % tamanhoRam;
        add1->endPalavra = rand() % 4;
        umaInstrucao->add1 = add1;

        Endereco* add2 = (Endereco*)malloc(sizeof(Endereco));
        add2->endBloco = rand() % tamanhoRam;
        add2->endPalavra = rand() % 4;
        umaInstrucao->add2 = add2;

        Endereco* add3 = (Endereco*)malloc(sizeof(Endereco));
        add3->endBloco = rand() % tamanhoRam;
        add3->endPalavra = rand() % 4;
        umaInstrucao->add3 = add3;

        trecho1[i] = *umaInstrucao;
    }

    // Add halt instruction
    Instrucao* umaInstrucao = (Instrucao*)malloc(sizeof(Instrucao));
    umaInstrucao->opcode = -1;
    trecho1[qdeInstrucoes - 1] = *umaInstrucao;

    criarRAM_vazia(ram, tamanhoRam);
    CPU_setPrograma(cpu, trecho1);
    CPU_iniciar(cpu,ram, 16, 32, 64, "principal", 1);
}

int main() {
    srand(time(NULL));
    GeradorInstrucoes();
    GeradorInstrucoesINT();
    RAM* ram = (RAM*)malloc(sizeof(RAM));
    CPU* cpu = (CPU*)malloc(sizeof(CPU));
    CPU* tratar = (CPU*)malloc(sizeof(CPU));
    //programaAleatorio(ram, cpu, 1000, 100);
    programaAleatorioRepeticoes(ram, cpu);
    return 0;
}