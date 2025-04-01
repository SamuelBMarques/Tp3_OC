#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define QTD_INS 10000
#define QTD_INS2 100
#define TAM_FOR 3
#define TAM_MEM 1000
#define PROB_FOR 95

void GeradorInstrucoesINT() {
    int aleatorio;
    char separador = ':';
    char** ins = (char**)malloc(QTD_INS2 * sizeof(char*));

    if (!ins) {
        printf("Erro ao alocar memória para ins.\n");
        return;
    }

    int N = 3;
    int instruc[] = { 6, 6, 6, 6, 6, 6, 6, 6 };

    char** repeticao = (char**)malloc(TAM_FOR * sizeof(char*));
    if (!repeticao) {
        printf("Erro ao alocar memória para repeticao.\n");
        free(ins);
        return;
    }
    
    for (int i = 0; i < TAM_FOR; i++) {
        aleatorio = rand() % N;
        repeticao[i] = (char*)malloc(50 * sizeof(char));
        if (!repeticao[i]) {
            printf("Erro ao alocar memória para repeticao[%d].\n", i);
            for (int j = 0; j < i; j++) free(repeticao[j]);
            free(repeticao);
            free(ins);
            return;
        }

        sprintf(repeticao[i], "%d", aleatorio);
        for (int j = 0; j < instruc[aleatorio]; j++) {
            char temp[10];
            sprintf(temp, "%c%d", separador, rand() % TAM_MEM);
            strcat(repeticao[i], temp);
        }
    }

    for (int i = 0; i < QTD_INS2;) {
        aleatorio = rand() % 100 + 1;
        if (aleatorio <= PROB_FOR && i + TAM_FOR <= QTD_INS2) {
            for (int j = 0; j < TAM_FOR && i < QTD_INS2; j++) {
                ins[i++] = strdup(repeticao[j]);  // Criamos uma cópia da string
            }
        } else {
            aleatorio = rand() % N;
            ins[i] = (char*)malloc(50 * sizeof(char));
            if (!ins[i]) {
                printf("Erro ao alocar memória para ins[%d].\n", i);
                break;
            }

            sprintf(ins[i], "%d", aleatorio);
            for (int j = 0; j < instruc[aleatorio]; j++) {
                char temp[10];
                sprintf(temp, "%c%d", separador, rand() % TAM_MEM);
                strcat(ins[i], temp);
            }
            i++;
        }
    }

    FILE* file = fopen("interrupcao.txt", "w");
    if (!file) {
        printf("Erro ao abrir arquivo.\n");
        return;
    }

    for (int i = 0; i < QTD_INS2; i++) {
        fprintf(file, "%s\n", ins[i]);
        free(ins[i]);  // Liberamos cada string individualmente
    }
    
    fclose(file);
    free(ins);

    for (int i = 0; i < TAM_FOR; i++) {
        free(repeticao[i]);  // Agora podemos liberar as strings de repeticao
    }
    free(repeticao);  // Liberamos o array repeticao
}


void GeradorInstrucoes() {
    int aleatorio;
    char separador = ':';
    char** ins = (char**)malloc(QTD_INS * sizeof(char*));

    if (!ins) {
        printf("Erro ao alocar memória para ins.\n");
        return;
    }

    int N = 3;
    int instruc[] = { 6, 6, 6, 6, 6, 6, 6, 6 };

    char** repeticao = (char**)malloc(TAM_FOR * sizeof(char*));
    if (!repeticao) {
        printf("Erro ao alocar memória para repeticao.\n");
        free(ins);
        return;
    }

    for (int i = 0; i < TAM_FOR; i++) {
        aleatorio = rand() % N;
        repeticao[i] = (char*)malloc(50 * sizeof(char));
        if (!repeticao[i]) {
            printf("Erro ao alocar memória para repeticao[%d].\n", i);
            for (int j = 0; j < i; j++) free(repeticao[j]);
            free(repeticao);
            free(ins);
            return;
        }

        sprintf(repeticao[i], "%d", aleatorio);
        for (int j = 0; j < instruc[aleatorio]; j++) {
            char temp[10];
            sprintf(temp, "%c%d", separador, rand() % TAM_MEM);
            strcat(repeticao[i], temp);
        }
    }

    for (int i = 0; i < QTD_INS;) {
        aleatorio = rand() % 100 + 1;
        if (aleatorio <= PROB_FOR && i + TAM_FOR <= QTD_INS) {
            for (int j = 0; j < TAM_FOR && i < QTD_INS; j++) {
                ins[i++] = strdup(repeticao[j]);  // Criamos uma cópia da string
            }
        } else {
            aleatorio = rand() % N;
            ins[i] = (char*)malloc(50 * sizeof(char));
            if (!ins[i]) {
                printf("Erro ao alocar memória para ins[%d].\n", i);
                break;
            }

            sprintf(ins[i], "%d", aleatorio);
            for (int j = 0; j < instruc[aleatorio]; j++) {
                char temp[10];
                sprintf(temp, "%c%d", separador, rand() % TAM_MEM);
                strcat(ins[i], temp);
            }
            i++;
        }
    }

    FILE* file = fopen("programa.txt", "w");
    if (!file) {
        printf("Erro ao abrir arquivo.\n");
        return;
    }

    for (int i = 0; i < QTD_INS; i++) {
        fprintf(file, "%s\n", ins[i]);
        free(ins[i]);  // Liberamos cada string individualmente
    }
    
    fclose(file);
    free(ins);

    for (int i = 0; i < TAM_FOR; i++) {
        free(repeticao[i]);  // Agora podemos liberar as strings de repeticao
    }
    free(repeticao);  // Liberamos o array repeticao
}


