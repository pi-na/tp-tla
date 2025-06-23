#include "Logger.h"
#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>   // PATH_MAX
#include <unistd.h>   // realpath

static Logger * _logger = NULL;

void initializeSymbolTableModule() {
    _logger = createLogger("SymbolTable");
    _logger->loggingLevel = DEBUGGING;
    logDebugging(_logger, "SymbolTable module initialized.");
}

void shutdownSymbolTableModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

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

bool symbolTableLoadFromFile(SymbolTable *table, const char *filename) {
    logDebugging(_logger, "symbolTableLoadFromFile for file %s", filename);

    if (!table || !filename || filename[0] == '\0') {
        logError(_logger, "Invalid parameters for symbolTableLoadFromFile!");
        return false;
    }

    // Canonizar a ruta absoluta
    char abs_path[PATH_MAX];
    if (!realpath(filename, abs_path)) {
        logError(_logger, "Failed to resolve absolute path for file: %s", filename);
        return false;
    }

    logDebugging(_logger, "File: %s", abs_path);

    // Validar permisos de acceso
    if (access(abs_path, R_OK) != 0) {
        logError(_logger, "Cannot read file: %s - check file permissions", abs_path);
        return false;
    }

    // abrir el archivo ya con ruta absoluta validada
    FILE *file = fopen(abs_path, "r");
    if (!file) {
        logError(_logger, "Failed to open file: %s", abs_path);
        return false;
    }

    logDebugging(_logger, "File: %s opened succesfully. Now loading symbols...", abs_path);
    char name[256];
    char value[256];
    while (fscanf(file, "%255s %255s", name, value) == 2) {
        Symbol *sym = symbolTableInsert(table, name, value);
        if (!sym) {
            fclose(file);
            return false;
        }
        logDebugging(_logger, "Loaded symbol: %s = %s", name, value);
    }

    fclose(file);
    return true;
}

Symbol* symbolTableLookup(SymbolTable *table, const char *name) {
    if (!table || !name || strlen(name) == 0 || table->head == NULL) return NULL;

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

Symbol* symbolTablePush(SymbolTable *table, const char *name, const char *initialValue){
    if (!table || !name || strlen(name) == 0) return NULL;

    logDebugging(_logger, "Pushing symbol: %s = %s", name,
                 initialValue ? initialValue : "(null)");

    Symbol *sym = calloc(1, sizeof(Symbol));
    if (!sym) return NULL;

    sym->name  = strdup(name);
    sym->value = initialValue ? strdup(initialValue) : NULL;
    sym->next  = table->head;
    table->head = sym;
    return sym;
}

bool symbolTableGetValue(SymbolTable *table, const char *name, char **outValue) {
    if (!table || !name || strlen(name) == 0) return false;
    Symbol *sym = symbolTableLookup(table, name);
    if (!sym || !sym->value) return false;
    *outValue = strdup(sym->value);
    return true;
}

bool symbolTablePop(SymbolTable *table, char **outValue) {
    if (!table || !outValue || !table->head) return false;

    Symbol *sym = table->head;
    table->head = sym->next;

    if (sym->value) {
        *outValue = strdup(sym->value);
    } else {
        *outValue = NULL;
    }

    free(sym->name);
    free(sym->value);
    free(sym);

    return true;
}