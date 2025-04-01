#ifndef INSTRUCAO_H
#define INSTRUCAO_H

#include "endereco.h"

typedef struct {
    Endereco *add1;
    Endereco *add2;
    Endereco *add3;
    int opcode;
} Instrucao;

#endif // INSTRUCAO_H