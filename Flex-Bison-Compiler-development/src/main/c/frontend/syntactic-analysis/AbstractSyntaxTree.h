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
typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;
typedef enum ValueType ValueType;
typedef enum StyleValueType StyleValueType;
typedef enum HTMLElementType HTMLElementType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Program Program;
typedef struct Value Value;
typedef struct StyleValue StyleValue;
typedef struct StyleProperty StyleProperty;
typedef struct StyleProperties StyleProperties;
typedef struct Entry Entry;
typedef struct Entries Entries;
typedef struct Array Array;
typedef struct Object Object;
typedef struct HTMLElement HTMLElement;

/* Nuevos typedefs para elementos HTML específicos */
typedef struct HTMLElementHTML HTMLElementHTML;
typedef struct HTMLElementHead HTMLElementHead;
typedef struct HTMLElementTitle HTMLElementTitle;
typedef struct HTMLElementBody HTMLElementBody;
typedef struct HTMLElementDiv HTMLElementDiv;
typedef struct HTMLElementP HTMLElementP;
typedef struct HTMLElementH1 HTMLElementH1;
typedef struct HTMLElementImg HTMLElementImg;
typedef struct HTMLElementA HTMLElementA;
typedef struct HTMLElementCenter HTMLElementCenter;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

enum ValueType {
	STRING_VALUE,
	INTEGER_VALUE,
	REAL_VALUE,
	BOOLEAN_VALUE,
	NULL_VALUE,
	OBJECT_VALUE,
	ARRAY_VALUE
};

enum StyleValueType {
	STRING_STYLE,
	INTEGER_STYLE,
	REAL_STYLE,
	CSS_KEYWORD_STYLE
};

enum HTMLElementType {
	HTML_ELEMENT,
	HEAD_ELEMENT,
	TITLE_ELEMENT,
	BODY_ELEMENT,
	DIV_ELEMENT,
	P_ELEMENT,
	H1_ELEMENT,
	IMG_ELEMENT,
	A_ELEMENT,
	CENTER_ELEMENT,
	B_ELEMENT,
	I_ELEMENT,
	BR_ELEMENT,
	HR_ELEMENT
};

struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

struct Value {
	union {
		char * string;
		int integer;
		double real;
		int boolean;
		Object * object;
		Array * array;
	};
	ValueType type;
};

struct StyleValue {
	union {
		char * string;
		int integer;
		double real;
		char * cssKeyword;
	};
	StyleValueType type;
};

struct StyleProperty {
	char * key;
	StyleValue * value;
};

struct StyleProperties {
	StyleProperty ** properties;
	int count;
};

struct Entry {
	char * key;
	Value * value;
};

struct Entries {
	Entry ** entries;
	int count;
};

struct Array {
	Value ** elements;
	int count;
};

struct Object {
	char * type;
	Entries * entries;
};

/* Estructura base para todos los elementos HTML */
struct HTMLElement {
	HTMLElementType type;
	Object* attributes;  // Para style, class, id, etc.
};

/* Estructuras específicas para cada tipo de elemento */
struct HTMLElementHTML {
	HTMLElement base;
	Object* head;
	Object* body;
};

struct HTMLElementHead {
	HTMLElement base;
	Object* title;
};

struct HTMLElementTitle {
	HTMLElement base;
	char* content;
};

struct HTMLElementBody {
	HTMLElement base;
	Array* content;
};

struct HTMLElementDiv {
	HTMLElement base;
	Array* content;
};

struct HTMLElementP {
	HTMLElement base;
	Value* content;
};

struct HTMLElementH1 {
	HTMLElement base;
	Value* content;
};

struct HTMLElementImg {
	HTMLElement base;
	char* src;
	char* alt;
};

struct HTMLElementA {
	HTMLElement base;
	char* href;
	char* ref;
	char* extRef;
	Value* content;
};

struct HTMLElementCenter {
	HTMLElement base;
	Array* content;
};

struct Program {
	HTMLElement * root;
};

/**
 * Node recursive destructors.
 */
void releaseConstant(Constant * constant);
void releaseExpression(Expression * expression);
void releaseFactor(Factor * factor);
void releaseProgram(Program * program);
void releaseValue(Value * value);
void releaseStyleValue(StyleValue * value);
void releaseStyleProperty(StyleProperty * property);
void releaseStyleProperties(StyleProperties * properties);
void releaseEntry(Entry * entry);
void releaseEntries(Entries * entries);
void releaseArray(Array * array);
void releaseObject(Object * object);
void releaseHTMLElement(HTMLElement * element);

#endif
