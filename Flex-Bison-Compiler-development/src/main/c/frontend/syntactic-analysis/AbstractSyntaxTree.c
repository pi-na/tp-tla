#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntxTree");
}

void shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PUBLIC FUNCTIONS */

void releaseEntryList(EntryList * entryList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (entryList == NULL) {
		return;
	}
	
	// Liberar todos los nodos de la lista enlazada recursivamente
	if (entryList->next != NULL) {
		releaseEntryList(entryList->next);
	}
	
	// Liberar la entrada del nodo actual
	if (entryList->entry != NULL) {
		releaseEntry(entryList->entry);
	}
	
	// Liberar el nodo actual
	free(entryList);
}

void releaseEntry(Entry * entry) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (entry != NULL) {
		free(entry);
	}
}

void releaseObject(Object * object) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (object != NULL) {
		releaseEntryList(object->entries);
		free(object);
	}
}

void releaseProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		releaseObject(program->object);
		free(program);
	}
}
