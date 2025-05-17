#ifndef TOKENS_HEADER
#define TOKENS_HEADER

#include <stdlib.h>

/**
 * Tipos de tokens reconocidos por el analizador léxico.
 */
typedef enum {
    /* Delimitadores */
    OPEN_BRACE,      /* { */
    CLOSE_BRACE,     /* } */
    OPEN_BRACKET,    /* [ */
    CLOSE_BRACKET,   /* ] */
    COLON,           /* : */
    COMMA,           /* , */
    OPEN_PARENTHESIS,/* ( */
    CLOSE_PARENTHESIS,/* ) */

    /* Literales */
    STRING,          /* "texto" */
    INTEGER,         /* 123 */
    REAL,            /* 123.45 */
    TRUE,            /* true */
    FALSE,           /* false */
    NULL_TOKEN,      /* null */

    /* Palabras clave */
    TYPE,            /* type */
    CONTENT,         /* content */
    STYLE,           /* style */
    SRC,             /* src */
    ALT,             /* alt */
    ID,              /* id */
    CLASS,           /* class */
    HREF,            /* href */
    REF,             /* ref */
    EXT_REF,         /* ext-ref */
    VAR,             /* var */
    IF,              /* if */
    THEN,            /* then */
    ELSE,            /* else */
    FOR,             /* for */
    WHILE,           /* while */

    /* Tags HTML */
    HTML_TAG,        /* html */
    HEAD_TAG,        /* head */
    TITLE_TAG,       /* title */
    BODY_TAG,        /* body */
    DIV_TAG,         /* div */
    P_TAG,           /* p */
    H1_TAG,          /* h1 */
    IMG_TAG,         /* img */
    A_TAG,           /* a */
    B_TAG,           /* b */
    I_TAG,           /* i */
    BR_TAG,          /* br */
    HR_TAG,          /* hr */
    CENTER_TAG,      /* center */

    /* Operadores */
    AND,             /* && */
    OR,              /* || */
    NOT,             /* ! */
    EQ,              /* == */
    NEQ,             /* != */
    LE,              /* <= */
    GE,              /* >= */
    LT,              /* < */
    GT,              /* > */
    ADD,             /* + */
    SUB,             /* - */
    MUL,             /* * */
    DIV,             /* / */

    /* Identificadores */
    IDENTIFIER,      /* variable o nombre de clave */

    /* Token desconocido */
    UNKNOWN          /* cualquier otro carácter */
} Token;

/**
 * Unión que almacena los valores semánticos de los tokens.
 */
typedef union {
    char* string;    /* Para strings e identificadores */
    int integer;     /* Para enteros */
    double real;     /* Para números reales */
    int boolean;     /* Para booleanos */
    int null;        /* Para null */
    Token token;     /* Para tokens que no tienen valor */
} SemanticValue;

#endif 