#include "memoria.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

Memoria* alocaMemoria(int tamanho){
    Memoria *memoria = malloc(tamanho * sizeof(Memoria));

    for(int i = 0; i<tamanho; i++){
        memoria[i].endBloco = -1;
        for(int j = 0; j < TAM_BLOCO; j++)
            memoria[i].palavras[j] = 0;
        memoria[i].nAcessos = 0;
        memoria[i].atualizado = false;
    }

    return memoria;
}

void liberaMemoria(Memoria *memoria){
    free(memoria);
}

//Preenchendo a ram com numeros aleatorias de 0 a 99
void preencheRam(Memoria *RAM){
    srand(time(NULL));
    for(int i = 0; i < TAM_RAM; i++)
        for(int j = 0; j < TAM_BLOCO; j++)
            RAM[i].palavras[j] = rand() % 100;
}

void criaHD(){

    FILE *file = fopen("HD.bin", "wb");

    srand(time(NULL));

    int linha[4];

    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < 4; j++) {
            linha[j] = rand() % 100;
        }

        fwrite(linha, sizeof(int), 4, file);
    }

    fclose(file);
    //printf("Arquivo binário '%s' criado com sucesso com %d linhas.\n", filename, numLines);
}




void iniciaTaxas(Taxas *taxas){
	taxas->Miss_c1 = 0;
	taxas->Miss_c2 = 0;
	taxas->Miss_c3 = 0;
	taxas->Miss_ram = 0;
	taxas->Hit_c1 = 0;
	taxas->Hit_c2 = 0;
	taxas->Hit_c3 = 0;
	taxas->Hit_ram = 0;
	taxas->Hit_hd = 0;
	taxas->custo = 0;
}


int buscaHD(int endBloco, int endPalavra) {
    FILE *file = fopen("HD.bin", "rb");

	long posicao = (endBloco * 4 + endPalavra) * sizeof(int);
    
    fseek(file, posicao, SEEK_SET);

    int value;
  	fread(&value, sizeof(int), 1, file);

    fclose(file);
    return value;
}


int buscarMemoria(Endereco e, Memoria* cache, int tam_cache){ 
	//verifica a existencia do endereco para cada bloco dentro de uma cache
	for(int i = 0; i < tam_cache; i++) {
		if(cache[i].endBloco == getEndBloco(e)){
			setAcesso(&cache[i], 1);
			return i;
		}
	}
	return -1;
}


void insereCache(int valor, Memoria* memoria, Endereco e){

	for(int i = 0; i < TAM_L1; i++){
		if(memoria[i].endBloco == getEndBloco(e)){
			memoria[i].palavras[getEndPalavra(e)] = valor;
			memoria[i].nAcessos = 0;
			memoria[i].atualizado = 1;
			break;
		}
	}
}


void trocarCache(int endBloco, Memoria* cOrigem, int tam_cOrigem, Memoria* cDestino, int tam_cDestino){
	int indice = 0; 
	int menorAcesso = cDestino[0].nAcessos;
	
	for(int i = 0; i < tam_cDestino; i++){
		if(cDestino[i].endBloco == -1){
			indice = i;
			break;
		} 
		else if(cDestino[i].nAcessos < menorAcesso){
			menorAcesso = cDestino[i].nAcessos;
			indice = i;
		} 
	}

	for(int i = 0; i < tam_cOrigem; i++){
		if(cOrigem[i].endBloco == endBloco){
			Memoria aux;
			aux = cDestino[indice];
			cDestino[indice] = cOrigem[i];
			cOrigem[i] = aux;
			cDestino[indice].nAcessos = 0;
			cDestino[indice].atualizado = 0;
			break;
		}
	}
}

//Essa função quando chamada, pega um conteudo especifico da ram e coloca na L3, se o conteudo da L3 que vai ser sobrescristo estiver atualizado, salva esse conteudo na Ram.
void trocarRam(int endBloco, Memoria* ram, int tam_ram, Memoria* cDestino, int tam_cDestino){
	int indice = 0; 
	int menorAcesso = cDestino[0].nAcessos;
	
	for(int i = 0; i < tam_cDestino; i++){
		if(cDestino[i].endBloco == -1){
			indice = i;
			break;
		} 
		else if(cDestino[i].nAcessos < menorAcesso){
			menorAcesso = cDestino[i].nAcessos;
			indice = i;
		} 
	}

	for(int i = 0; i < tam_ram; i++){
		if(ram[i].endBloco == endBloco){
			Memoria aux;
			aux = cDestino[indice];
			cDestino[indice] = ram[i];
			ram[i] = aux;
			cDestino[indice].nAcessos = 0;
			cDestino[indice].atualizado = 0;
			break;
		}
	}
	
}

void trocarHD(int endBloco, Memoria* ram, int tam_ram){
	int indice = 0;
	int menorAcesso = ram[0].nAcessos;

	for(int i = 0; i < tam_ram; i++){
		if(ram[i].endBloco == -1){
			indice = i;
			break;
		} 
		else if(ram[i].nAcessos < menorAcesso){
			menorAcesso = ram[i].nAcessos;
			indice = i;
		} 
	}
	if(ram[indice].atualizado == 1){
		FILE *file = fopen("HD.bin", "ab");
    	fwrite(ram[indice].palavras, sizeof(int), 4, file);
    	fclose(file);
	}


	FILE *file = fopen("HD.bin", "rb");
    long pos = endBloco * 4 * sizeof(int);
    
    fseek(file, pos, SEEK_SET);
    fread(ram[indice].palavras, sizeof(int), 4, file);
    fclose(file);
	ram[indice].atualizado = 0;
	ram[indice].nAcessos = 0;
	ram[indice].endBloco = endBloco;

}

//Uma das funções principais do codigo
//vai buscar nas caches de acordo com um endereço de bloco especifico e já da set nos hits e miss necessarios
Memoria MMU(Memoria* L1, Memoria* L2, Memoria* L3, Memoria* ram, Endereco e, Taxas* taxas){
	inidice = buscarMemoria(e, L1, TAM_L1);
	if(indice != -1){
		setHitC1(taxas);
		setCusto(taxas, 10);
		return L1[indice];
	} else if(buscarMemoria(e, L2, TAM_L2) != -1){
		setMissC1(taxas);
		setHitC2(taxas);
		setCusto(taxas, 100);
		trocarCache(getEndBloco(e), L2, TAM_L2, L1, TAM_L1);
		return L1[buscarMemoria(e, L1, TAM_L1)];
	} else if(buscarMemoria(e, L3, TAM_L3) != -1){
		setMissC1(taxas);
		setMissC2(taxas);
		setHitC3(taxas);
		setCusto(taxas, 1000);
		trocarCache(getEndBloco(e), L3, TAM_L3, L2, TAM_L2);
		trocarCache(getEndBloco(e), L2, TAM_L2, L1, TAM_L1);
		return L1[buscarMemoria(e, L1, TAM_L1)];
	} else if(buscarMemoria(e, ram, TAM_RAM) != -1){
		setMissC1(taxas);
		setMissC2(taxas);
		setMissC3(taxas);
		setHitRam (taxas);
		setCusto(taxas, 10000);
		trocarRam(getEndBloco(e), ram, TAM_RAM, L3, TAM_L3);
		trocarCache(getEndBloco(e), L3, TAM_L3, L2, TAM_L2);
		trocarCache(getEndBloco(e), L2, TAM_L2, L1, TAM_L1);
		return L1[buscarMemoria(e, L1, TAM_L1)];
	} else {
		setMissC1(taxas);
		setMissC2(taxas);
		setMissC3(taxas);
		setMissRam (taxas);
		setHitHD(taxas);
		trocarHD(getEndBloco(e), ram, TAM_RAM);
		trocarRam(getEndBloco(e), ram, TAM_RAM, L3, TAM_L3);
		trocarCache(getEndBloco(e), L3, TAM_L3, L2, TAM_L2);
		trocarCache(getEndBloco(e), L2, TAM_L2, L1, TAM_L1);
	}
	return L1[buscarMemoria(e, L1, TAM_L1)];
}


// GET 
int getEndBlocoM (Memoria* memoria){
	return memoria->endBloco;
}

int getPalavra(Memoria memoria, int indicePalavra){
	return memoria.palavras[indicePalavra];
}

int getAcesso(Memoria* memoria){
	return memoria->nAcessos;
}

int getAtualizado(Memoria* memoria){
	return memoria->atualizado;
}


// SET
void setEndBlocoM (Memoria* memoria, int endBloco){
	memoria->endBloco = endBloco;
}

void setPalavra(Memoria* memoria, int indicePalavra, int palavra){
	memoria->palavras[indicePalavra] = palavra;
}

void setAcesso(Memoria* memoria, int nAcessos){
	memoria->nAcessos += nAcessos;
}

void setAtualizado(Memoria* memoria, int atualizado){
	memoria->atualizado = atualizado;
}

void setHitC1(Taxas* taxas){
	taxas->Hit_c1++;
}

void setMissC1(Taxas* taxas){
	taxas->Miss_c1++;
}

void setHitC2(Taxas* taxas){
	taxas->Hit_c2++;
}

void setMissC2(Taxas* taxas){
	taxas->Miss_c2++;
}

void setHitC3(Taxas* taxas){
	taxas->Hit_c3++;
}

void setMissC3(Taxas* taxas){
	taxas->Miss_c3++;
}

void setHitRam (Taxas* taxas) {
	taxas->Hit_ram++;
} 

void setMissRam (Taxas* taxas) {
	taxas->Miss_ram++;
} 

void setHitHD (Taxas* taxas) {
	taxas->Hit_hd++;
} 

void setCusto(Taxas* taxas, int custo){
	taxas->custo += custo;
}