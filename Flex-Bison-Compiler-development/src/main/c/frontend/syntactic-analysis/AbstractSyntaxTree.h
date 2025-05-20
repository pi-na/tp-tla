#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * This typedefs allows self-referencing types.
 */

typedef struct Program Program;
typedef struct Object Object;
typedef struct EntryList EntryList;
typedef struct Entry Entry;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

struct Program {
	Object * object;
};

struct Object {
	EntryList * entries;
};

struct EntryList {
	Entry * entry;
	EntryList * next;
};

struct Entry {
	/** TODO */
};

/**
 * Node recursive destructors.
 */
void releaseProgram(Program * program);
void releaseObject(Object * object);
void releaseEntryList(EntryList * entryList);
void releaseEntry(Entry * entry);

#endif
