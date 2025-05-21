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
typedef struct Conditional Conditional;
typedef struct Loop Loop;
typedef struct Expression Expression;
typedef struct VarRef VarRef;
typedef struct AttributeList AttributeList;
typedef struct Attribute Attribute;
typedef struct Element Element;

/**
 * Enumeraciones para tipos de valor en JSON-HTML
 */
typedef enum {
    STRING_VALUE,
    INTEGER_VALUE,
    FLOAT_VALUE,
    BOOLEAN_VALUE,
    NULL_VALUE,
    OBJECT_VALUE,
    ARRAY_VALUE,
    CONDITIONAL_VALUE,
    LOOP_VALUE,
    VAR_REF_VALUE,
    ELEMENT_VALUE
} ValueType;

/**
 * Enumeraciones para tipos de expresión
 */
typedef enum {
    INTEGER_EXPR,
    FLOAT_EXPR,
    STRING_EXPR,
    BOOLEAN_EXPR,
    VAR_REF_EXPR,
    BINARY_EXPR,
    UNARY_EXPR
} ExpressionType;

/**
 * Enumeraciones para operadores en expresiones
 */
typedef enum {
    EQUALS_OP,
    NOT_EQUALS_OP,
    GREATER_THAN_OP,
    LESS_THAN_OP,
    GREATER_EQUAL_OP,
    LESS_EQUAL_OP,
    AND_OP,
    OR_OP,
    NOT_OP,
    ADD_OP,
    SUB_OP,
    MUL_OP,
    DIV_OP
} OperatorType;

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
		Conditional * conditionalValue;
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

struct Conditional {
	Expression * condition;
	Object * thenBranch;
	Object * elseBranch;  // Puede ser NULL
};

struct Loop {
	char * iteratorName;
	Object * iterable;
	Object * body;
};

struct Expression {
	ExpressionType type;
	union {
		int integerValue;
		double floatValue;
		char * stringValue;
		boolean booleanValue;
		VarRef * varRefValue;
		struct {
			OperatorType op;
			Expression * left;
			Expression * right;
		} binaryExpr;
		struct {
			OperatorType op;
			Expression * operand;
		} unaryExpr;
	} data;
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
void releaseConditional(Conditional * conditional);
void releaseLoop(Loop * loop);
void releaseExpression(Expression * expression);
void releaseVarRef(VarRef * varRef);
void releaseAttributeList(AttributeList * attributeList);
void releaseAttribute(Attribute * attribute);
void releaseElement(Element * element);

#endif
