#include "BisonActions.h"
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;
static CompilerState * _currentState = NULL;  // Para mantener el estado actual

// Sistema de seguimiento de variables
#define MAX_VARIABLES 100
static struct {
	char* name;
	boolean isDefined;
	boolean isBeingResolved;
	ValueType type;
	union {
		int intValue;
		double floatValue;
		char* stringValue;
		boolean booleanValue;
	} value;
} variables[MAX_VARIABLES];
static int variableCount = 0;

static void defineVariable(const char* name) {
	for (int i = 0; i < variableCount; i++) {
		if (strcmp(variables[i].name, name) == 0) {
			variables[i].isDefined = true;
			return;
		}
	}
	if (variableCount < MAX_VARIABLES) {
		variables[variableCount].name = strdup(name);
		variables[variableCount].isDefined = true;
		variables[variableCount].isBeingResolved = false;
		variableCount++;
	}
}

static boolean isVariableDefined(const char* name) {
	for (int i = 0; i < variableCount; i++) {
		if (strcmp(variables[i].name, name) == 0) {
			return variables[i].isDefined;
		}
	}
	return false;
}

static boolean isVariableBeingResolved(const char* name) {
	for (int i = 0; i < variableCount; i++) {
		if (strcmp(variables[i].name, name) == 0) {
			return variables[i].isBeingResolved;
		}
	}
	return false;
}

static void setVariableResolutionStatus(const char* name, boolean status) {
	for (int i = 0; i < variableCount; i++) {
		if (strcmp(variables[i].name, name) == 0) {
			variables[i].isBeingResolved = status;
			return;
		}
	}
}

static void clearVariables() {
	for (int i = 0; i < variableCount; i++) {
		free(variables[i].name);
	}
	variableCount = 0;
}

void initializeBisonActionsModule() {
	_logger = createLogger("BisonActions");
	clearVariables();
}

void shutdownBisonActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
	clearVariables();
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
	_currentState = compilerState;
	Program * program = calloc(1, sizeof(Program));
	program->object = object;
	compilerState->abstractSyntaxtTree = program;
	compilerState->succeed = true;
	return program;
}

Object * ObjectSemanticAction(PairList * pairs) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Object * object = calloc(1, sizeof(Object));
	object->pairs = pairs;
	return object;
}

PairList * emptyPairListAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return NULL;
}

PairList * singlePairListSemanticAction(Pair * pair) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	PairList * newPairList = calloc(1, sizeof(PairList));
	newPairList->pair = pair;
	newPairList->next = NULL;
	return newPairList;
}

PairList * pairListSemanticAction(PairList * pairList, Pair * newPair) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (pairList == NULL) {
		return singlePairListSemanticAction(newPair);
	}
	PairList * current = pairList;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = singlePairListSemanticAction(newPair);
	return pairList;
}

Pair * PairSemanticAction(Keyword * key, Value * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Pair * pair = calloc(1, sizeof(Pair));
	pair->key = key;
	pair->value = value;
	return pair;
}

Keyword * KeywordSemanticAction(Token type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Keyword * keyword = calloc(1, sizeof(Keyword));
	keyword->type = type;
	return keyword;
}

/**por que value es char???????????????? */
Value * StringValueSemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value * val = calloc(1, sizeof(Value));
	val->type = STRING_VALUE;
	val->data.stringValue = value;
	return val;
}

Value * NullValueSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value * val = calloc(1, sizeof(Value));
	val->type = NULL_VALUE;
	return val;
}

Value * ObjectValueSemanticAction(Object * object) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value * val = calloc(1, sizeof(Value));
	val->type = OBJECT_VALUE;
	val->data.objectValue = object;
	return val;
}

Value * ArrayValueSemanticAction(Array * array) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value * val = calloc(1, sizeof(Value));
	val->type = ARRAY_VALUE;
	val->data.arrayValue = array;
	return val;
}

Value * LoopValueSemanticAction(Loop * loop) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value * val = calloc(1, sizeof(Value));
	val->type = LOOP_VALUE;
	val->data.loopValue = loop;
	return val;
}

Value * VariableRefValueSemanticAction(VarRef * varRef) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value * val = calloc(1, sizeof(Value));
	val->type = VAR_REF_VALUE;
	val->data.varRefValue = varRef;
	return val;
}

Array * ArraySemanticAction(ValueList * values) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Array * array = calloc(1, sizeof(Array));
	array->values = values;
	return array;
}

ValueList * emptyValueListAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return NULL;
}

ValueList * singleValueListSemanticAction(Value * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ValueList * newValueList = calloc(1, sizeof(ValueList));
	newValueList->value = value;
	newValueList->next = NULL;
	return newValueList;
}

ValueList * valueListSemanticAction(ValueList * valueList, Value * newValue) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (valueList == NULL) {
		return singleValueListSemanticAction(newValue);
	}
	ValueList * current = valueList;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = singleValueListSemanticAction(newValue);
	return valueList;
}

Loop * LoopSemanticAction(char * iteratorName, Object * iterable, Object * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Loop * loop = calloc(1, sizeof(Loop));
	loop->iteratorName = iteratorName;
	loop->iterable = iterable;
	loop->body = body;
	return loop;
}

VarRef * VariableRefSemanticAction(char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	VarRef * varRef = calloc(1, sizeof(VarRef));
	varRef->name = name;
	return varRef;
}



