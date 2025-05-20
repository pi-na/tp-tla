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

Object * ObjectSemanticAction(PairList * pairs) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Object * object = calloc(1, sizeof(Object));
	object->pairs = pairs;
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

// ----- Implementación de las nuevas acciones semánticas ----- //

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

Pair * PairSemanticAction(char * key, Value * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Pair * pair = calloc(1, sizeof(Pair));
	pair->key = key;
	pair->value = value;
	return pair;
}

Value * StringValueSemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value * val = calloc(1, sizeof(Value));
	val->type = STRING_VALUE;
	val->data.stringValue = value;
	return val;
}

Value * IntegerValueSemanticAction(int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value * val = calloc(1, sizeof(Value));
	val->type = INTEGER_VALUE;
	val->data.integerValue = value;
	return val;
}

Value * FloatValueSemanticAction(double value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value * val = calloc(1, sizeof(Value));
	val->type = FLOAT_VALUE;
	val->data.floatValue = value;
	return val;
}

Value * BooleanValueSemanticAction(boolean value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value * val = calloc(1, sizeof(Value));
	val->type = BOOLEAN_VALUE;
	val->data.booleanValue = value;
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

Value * ConditionalValueSemanticAction(Conditional * conditional) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value * val = calloc(1, sizeof(Value));
	val->type = CONDITIONAL_VALUE;
	val->data.conditionalValue = conditional;
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

Conditional * ConditionalSemanticAction(Expression * condition, Object * thenBranch, Object * elseBranch) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Conditional * conditional = calloc(1, sizeof(Conditional));
	conditional->condition = condition;
	conditional->thenBranch = thenBranch;
	conditional->elseBranch = elseBranch;
	return conditional;
}

Loop * LoopSemanticAction(Expression * initialization, Expression * condition, Expression * increment, Object * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Loop * loop = calloc(1, sizeof(Loop));
	loop->initialization = initialization;
	loop->condition = condition;
	loop->increment = increment;
	loop->body = body;
	return loop;
}

VarRef * VariableRefSemanticAction(char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	VarRef * varRef = calloc(1, sizeof(VarRef));
	varRef->name = name;
	return varRef;
}

Expression * IntegerExpressionSemanticAction(int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type = INTEGER_EXPR;
	expr->data.integerValue = value;
	return expr;
}

Expression * FloatExpressionSemanticAction(double value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type = FLOAT_EXPR;
	expr->data.floatValue = value;
	return expr;
}

Expression * StringExpressionSemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type = STRING_EXPR;
	expr->data.stringValue = value;
	return expr;
}

Expression * BooleanExpressionSemanticAction(boolean value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type = BOOLEAN_EXPR;
	expr->data.booleanValue = value;
	return expr;
}

Expression * VariableRefExpressionSemanticAction(VarRef * varRef) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type = VAR_REF_EXPR;
	expr->data.varRefValue = varRef;
	return expr;
}

Expression * BinaryExpressionSemanticAction(OperatorType op, Expression * left, Expression * right) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type = BINARY_EXPR;
	expr->data.binaryExpr.op = op;
	expr->data.binaryExpr.left = left;
	expr->data.binaryExpr.right = right;
	return expr;
}

Expression * UnaryExpressionSemanticAction(OperatorType op, Expression * operand) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expr = calloc(1, sizeof(Expression));
	expr->type = UNARY_EXPR;
	expr->data.unaryExpr.op = op;
	expr->data.unaryExpr.operand = operand;
	return expr;
}

AttributeList * emptyAttributeListAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return NULL;
}

AttributeList * singleAttributeListSemanticAction(Attribute * attribute) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AttributeList * newAttributeList = calloc(1, sizeof(AttributeList));
	newAttributeList->attribute = attribute;
	newAttributeList->next = NULL;
	return newAttributeList;
}

AttributeList * attributeListSemanticAction(AttributeList * attributeList, Attribute * newAttribute) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (attributeList == NULL) {
		return singleAttributeListSemanticAction(newAttribute);
	}
	
	AttributeList * current = attributeList;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = singleAttributeListSemanticAction(newAttribute);
	return attributeList;
}

Attribute * AttributeSemanticAction(char * name, Value * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Attribute * attribute = calloc(1, sizeof(Attribute));
	attribute->name = name;
	attribute->value = value;
	return attribute;
}

Element * ElementSemanticAction(char * tag, AttributeList * attributes, ValueList * children) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Element * element = calloc(1, sizeof(Element));
	element->tag = tag;
	element->attributes = attributes;
	element->children = children;
	return element;
}


