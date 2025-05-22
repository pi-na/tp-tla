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

// Nuevas estructuras para JSON-HTML
typedef struct ValueList ValueList;
typedef struct Keyword Keyword;
typedef struct Value Value;
typedef struct Array Array;
typedef struct Pair Pair;
typedef struct PairList PairList;
typedef struct VarRef VarRef;

/**
 * Enumeraciones para tipos de valor en JSON-HTML
 */
typedef enum {
    STRING_VALUE,
	TOKEN_VALUE,
    NULL_VALUE,
    OBJECT_VALUE,
    ARRAY_VALUE,
    VAR_REF_VALUE
} ValueType;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

struct Program {
	Object * object;
};

struct Object {
	PairList * pairs;
};

struct PairList {
	Pair * pair;
	PairList * next;
};

struct Pair {
	Keyword * key;
	Value * value;
};

struct Keyword {
	Token type;
};

struct Value {
	ValueType type;
	union {
		char * stringValue;
		int integerValue;
		double floatValue;
		boolean booleanValue;
		Object * objectValue;
		Array * arrayValue;
		VarRef * varRefValue;
		Token tokenValue;
	} data;
};

struct Array {
	ValueList * values;
};

struct ValueList {
	Value * value;
	ValueList * next;
};

struct VarRef {
	char * name;
};

/**
 * Node recursive destructors.
 */
void releaseProgram(Program * program);
void releaseObject(Object * object);
void releaseEntryList(EntryList * entryList);
void releaseEntry(Entry * entry);

// Destructores para nuevas estructuras
void releasePairList(PairList * pairList);
void releasePair(Pair * pair);
void releaseKeyword(Keyword * keyword);
void releaseValue(Value * value);
void releaseArray(Array * array);
void releaseValueList(ValueList * valueList);
void releaseVarRef(VarRef * varRef);

#endif
