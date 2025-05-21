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
typedef struct Value Value;
typedef struct Array Array;
typedef struct Pair Pair;
typedef struct PairList PairList;
typedef struct Loop Loop;
typedef struct VarRef VarRef;
typedef struct AttributeList AttributeList;
typedef struct Attribute Attribute;
typedef struct Element Element;

/**
 * Enumeraciones para tipos de valor en JSON-HTML
 */
typedef enum {
    STRING_VALUE,
    NULL_VALUE,
    OBJECT_VALUE,
    ARRAY_VALUE,
    LOOP_VALUE,
    VAR_REF_VALUE,
    ELEMENT_VALUE
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

struct EntryList {
	Entry * entry;
	EntryList * next;
};

struct Entry {
	/** Para compatibilidad con código existente */
};

struct PairList {
	Pair * pair;
	PairList * next;
};

struct Pair {
	char * key;
	Value * value;
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
		Loop * loopValue;
		VarRef * varRefValue;
		Element * elementValue;
	} data;
};

struct Array {
	ValueList * values;
};

struct ValueList {
	Value * value;
	ValueList * next;
};

struct Loop {
	char * iteratorName;
	Object * iterable;
	Object * body;
};

struct VarRef {
	char * name;
};

struct AttributeList {
	Attribute * attribute;
	AttributeList * next;
};

struct Attribute {
	char * name;
	Value * value;
};

struct Element {
	char * tag;
	AttributeList * attributes;
	ValueList * children;
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
void releaseValue(Value * value);
void releaseArray(Array * array);
void releaseValueList(ValueList * valueList);
void releaseLoop(Loop * loop);
void releaseVarRef(VarRef * varRef);
void releaseAttributeList(AttributeList * attributeList);
void releaseAttribute(Attribute * attribute);
void releaseElement(Element * element);

#endif
