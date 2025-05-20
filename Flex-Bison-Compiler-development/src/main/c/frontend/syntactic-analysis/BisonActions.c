#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeBisonActionsModule() {
	_logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Program * ObjectProgramSemanticAction(CompilerState * compilerState, Object * object) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->object = object;
	compilerState->abstractSyntaxtTree = program;
	if (0 < flexCurrentContext()) {
		logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
		compilerState->succeed = false;
	}
	else {
		compilerState->succeed = true;
	}
	return program;
}

Object * ObjectSemanticAction(EntryList * entries) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Object * object = calloc(1, sizeof(Object));
	object->entries = entries;
	return object;
}

EntryList * entryListSemanticAction(EntryList * entryList, Entry * newEntry) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	while(entryList->next != NULL) {
		entryList = entryList->next;
	}
	entryList->next = singleEntryListSemanticAction(newEntry);
	return entryList;
}

EntryList * singleEntryListSemanticAction(Entry * entry) {
	EntryList * newEntryList = calloc(1, sizeof(EntryList));
	newEntryList->entry = entry;
	newEntryList->next = NULL;
	return newEntryList;
}

Entry * emptyEntryAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Entry * entry = calloc(1, sizeof(Entry));
	// No hay campos para inicializar en Entry ya que está vacío
	return entry;
}


