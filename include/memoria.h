#ifndef MEMORIA_H
#define MEMORIA_H
#include "instrucao.h"
#include <stdbool.h>

#define TAM_BLOCO 4
#define TAM_L1 16
#define TAM_L2 32
#define TAM_L3 64
#define TAM_RAM 500

typedef struct memoria{
	int endBloco;
	int palavras[4];
	int nAcessos;
	bool atualizado;
	
} Memoria;

typedef struct taxas{
	int Miss_c1, Hit_c1;
	int Miss_c2, Hit_c2;
	int Miss_c3, Hit_c3;
	int Miss_ram, Hit_ram;
	int Hit_hd;
	int custo;
} Taxas;

Memoria* alocaMemoria(int);
void liberaMemoria(Memoria *);

void preencheRam(Memoria *);
void iniciaTaxas(Taxas *);
void criaHD();

int buscaHD(int endBloco, int endPalavra);
int buscarMemoria(Endereco e, Memoria* cache, int tam_cache);
void insereCache(int valor, Memoria* memoria, Endereco e);
void trocarCache(int endBloco, Memoria* cOrigem, int tam_cOrigem, Memoria* cDestino, int tam_cDestino);
void trocarRam(int endBloco, Memoria* ram, int tam_ram, Memoria* cDestino, int tam_cDestino);

Memoria MMU(Memoria* c1, Memoria* c2, Memoria* c3, Memoria* ram, Endereco e, Taxas* taxas);

//GET 
int getEndBlocoM(Memoria* memoria);
int getPalavra(Memoria memoria, int indicePalavra);
int getAcesso(Memoria* memoria);
int getAtualizado(Memoria* memoria);

//SET
void setEndBlocoM(Memoria* memoria, int endBloco);
void setPalavra(Memoria* memoria, int indicePalavra, int palavra);
void setAcesso(Memoria* memoria, int nAcessos);
void setAtualizado(Memoria* memoria, int atualizado);

void setHitC1(Taxas* taxas);
void setMissC1(Taxas* taxas);
void setHitC2(Taxas* taxas);
void setMissC2(Taxas* taxas);
void setHitC3(Taxas* taxas);
void setMissC3(Taxas* taxas);
void setHitRam (Taxas* taxas);
void setMissRam (Taxas* taxas);
void setHitHD (Taxas* taxas);
void setCusto(Taxas* taxas, int custo);
#endif