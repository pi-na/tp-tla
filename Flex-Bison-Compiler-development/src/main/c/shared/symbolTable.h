#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Symbol {
    char          *name;   
    char          *value;   
    struct Symbol *next;     
} Symbol;

typedef struct SymbolTable {
    Symbol *head;
} SymbolTable;

SymbolTable* createSymbolTable(void);
void         destroySymbolTable(SymbolTable *table);

Symbol*      symbolTableLookup(SymbolTable *table, const char *name);
Symbol*      symbolTableInsert(SymbolTable *table, const char *name, const char *initialValue);

bool symbolTableSetValue(SymbolTable *table, const char *name, const char *newValue);

bool symbolTableGetValue(SymbolTable *table, const char *name, char **outValue);

bool symbolTablePop(SymbolTable *table, char **outValue);

#endif // SYMBOL_TABLE_H

