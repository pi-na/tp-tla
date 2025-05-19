#ifndef TOKENS_H
#define TOKENS_H

#include "../syntactic-analysis/BisonParser.h"

// Usamos los tokens definidos en BisonParser.h
typedef Token Token;

// Token desconocido
#define UNKNOWN 0

/**
 * Unión que almacena los valores semánticos de los tokens.
 */
typedef union {
    char* string;    /* Para strings e identificadores */
    int integer;     /* Para enteros */
    double real;     /* Para números reales */
    int boolean;     /* Para booleanos */
    Token token;     /* Para tokens que no tienen valor */
} SemanticValue;

#endif // TOKENS_H 