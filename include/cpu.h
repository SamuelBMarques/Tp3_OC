#ifndef CPU_H
#define CPU_H

#include "blocoMemoria.h"
#include "instrucao.h"
#include "ram.h"
#include "hd.h"

typedef struct Contexto {
    int PC;
    struct Contexto* proximo;
} Contexto;

typedef struct {
    BlocoMemoria* registrador1;
    BlocoMemoria* registrador2;
    BlocoMemoria* registrador3;
    int PC;
    Instrucao* programa;
    Instrucao* tratar;
    int opcode;
    int custo;
    int missC1;
    int hitC1;
    int missC2;
    int hitC2;
    int missC3;
    int hitC3;
    int hitRAM;
    int missRAM;
    int hitHD;
    Cache* L1;
    Cache* L2;
    Cache* L3;
    int interrupcao;
} CPU;

void CPU_setPrograma(CPU* cpu, Instrucao* programaAux);
Cache* CPU_iniciarCache(int tamanho);
void CPU_setTratar(CPU* cpu, Instrucao* interrupcao);
void CPU_iniciar(CPU* cpu,RAM* ram, int lengthL1, int lenghtL2, int lengthL3, Contexto** pilhaContexto);
#endif // CPU_H