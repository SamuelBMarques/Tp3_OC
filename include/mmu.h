
#ifndef MMU_H
#define MMU_H

#include "blocoMemoria.h"
#include "ram.h"
#include "endereco.h"
#include "hd.h"
BlocoMemoria* MMU_buscarNasMemorias(Endereco *e, RAM* ram, Cache* L1, Cache* L2, Cache* L3);
BlocoMemoria* MMU_movRamCache3(int posicaoCache3, Cache* L3, RAM* ram, Endereco *e, int custo);
BlocoMemoria* MMU_movCache2Cache1(int posicaoCache1, int posicaoCache2, Cache* L1, Cache* L2, Cache* L3, int custo, int posicao);
BlocoMemoria* MMU_movCache3Cache2(int posicaoCache2, int posicaoCache3, Cache* L2, Cache* L3, int custo, int posicao);
BlocoMemoria* MMU_movCache1ParaCache2(int posicaoCache2, int posicaoCache1, Cache* L2, Cache* L1, int custo, Cache* L3);
BlocoMemoria* MMU_movCache2ParaCache3(int posicaoCache3, int posicaoCache2, Cache* L3, Cache* L2, int custo);
BlocoMemoria* MMU_movHDParaRAM(int endBloco, RAM* ram, int custo);
void atualizarUltimoAcesso(Cache *cache,int conjunto, int linha);
int encontrarLRU(Cache *cache, int conjunto);
int encontrarBlocoNaCache(Cache *cache, int conjunto, int endBloco);

#endif // MMU_H