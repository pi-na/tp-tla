#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>

SymbolTable* createSymbolTable(void) {
    SymbolTable *table = calloc(1, sizeof(SymbolTable));
    table->head = NULL;
    return table;
}

void destroySymbolTable(SymbolTable *table) {
    if (!table) return;
    Symbol *cur = table->head;
    while (cur) {
        Symbol *next = cur->next;
        free(cur->name);
        free(cur->value);
        free(cur);
        cur = next;
    }
    free(table);
}

Symbol* symbolTableLookup(SymbolTable *table, const char *name) {
    for (Symbol *s = table->head; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    return NULL;
}

Symbol* symbolTableInsert(SymbolTable *table, const char *name, const char *initialValue) {
    if (!table || !name || strlen(name) == 0) return NULL;
    Symbol *existing = symbolTableLookup(table, name);
    if (existing) return existing;

    Symbol *sym = calloc(1, sizeof(Symbol));
    sym->name  = strdup(name);
    sym->value = initialValue ? strdup(initialValue) : NULL;
    sym->next  = table->head;
    table->head = sym;
    return sym;
}

bool symbolTableSetValue(SymbolTable *table,
                         const char *name,
                         const char *newValue) {
    Symbol *sym = lookup(table, name);
    if (!sym) return false;
    free(sym->value);
    sym->value = newValue ? strdup(newValue) : NULL;
    return sym->value != NULL || newValue == NULL;
}

bool symbolTableGetValue(SymbolTable *table, const char *name, char **outValue) {
    if (!table || !name || strlen(name) == 0) return NULL;
    Symbol *sym = symbolTableLookup(table, name);
    if (!sym || !sym->value) return false;
    *outValue = strdup(sym->value);
    return true;
}

#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>

bool symbolTablePop(SymbolTable *table, char **outValue) {
    if (!table || !outValue || !table->head) return false;

    // Tomo el símbolo de la cabeza
    Symbol *sym = table->head;
    table->head = sym->next;

    // Copio el valor al buffer de salida
    if (sym->value) {
        *outValue = strdup(sym->value);
    } else {
        *outValue = NULL;
    }

    // Libero todos los campos de la entrada
    free(sym->name);
    free(sym->value);
    free(sym);

    return true;
}
