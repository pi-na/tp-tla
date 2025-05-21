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
} variables[MAX_VARIABLES];
static int variableCount = 0;

// Lista de elementos HTML válidos
static const char* validElements[] = {
	"html", "head", "body", "title", "p", "div", "span", "a", "img", "ul", "ol", "li", NULL
};

// Funciones de validación
static boolean isValidElement(const char* type) {
	if (type == NULL) return false;
	for (int i = 0; validElements[i] != NULL; i++) {
		if (strcmp(type, validElements[i]) == 0) {
			return true;
		}
	}
	return false;
}

static boolean isFirstPairType(PairList* pairs) {
	if (pairs == NULL || pairs->pair == NULL || pairs->pair->key == NULL) {
		return false;
	}
	return strcmp(pairs->pair->key, "type") == 0;
}

static boolean hasDuplicateKeys(PairList* pairs) {
	for (PairList* outer = pairs; outer != NULL; outer = outer->next) {
		for (PairList* inner = outer->next; inner != NULL; inner = inner->next) {
			if (strcmp(outer->pair->key, inner->pair->key) == 0) {
				return true;
			}
		}
	}
	return false;
}

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
	_currentState = compilerState;  // Guardar el estado actual
	
	// Validar que el objeto raíz tenga 'type' como primer campo
	if (!isFirstPairType(object->pairs)) {
		logError(_logger, "The 'type' field must be the first field in the object");
		compilerState->succeed = false;
		return NULL;
	}
	
	// Validar que no haya campos duplicados
	if (hasDuplicateKeys(object->pairs)) {
		logError(_logger, "Duplicate keys found in object");
		compilerState->succeed = false;
		return NULL;
	}
	
	// Validar que el tipo sea un elemento HTML válido
	if (object->pairs && object->pairs->pair && object->pairs->pair->value->type == STRING_VALUE) {
		if (!isValidElement(object->pairs->pair->value->data.stringValue)) {
			logError(_logger, "Invalid HTML element type: %s", object->pairs->pair->value->data.stringValue);
			compilerState->succeed = false;
			return NULL;
		}
	}
	
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
	
	// Validar que no haya campos duplicados
	if (hasDuplicateKeys(pairs)) {
		logError(_logger, "Duplicate keys found in object");
		_currentState->succeed = false;
		return NULL;
	}
	
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
	
	// Si la referencia es NULL (debido a un error), propagar el error
	if (varRef == NULL) {
		_currentState->succeed = false;
		return NULL;
	}
	
	Value * val = calloc(1, sizeof(Value));
	val->type = VAR_REF_VALUE;
	val->data.varRefValue = varRef;
	return val;
}

Value * ElementValueSemanticAction(Element * element) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value * val = calloc(1, sizeof(Value));
	val->type = ELEMENT_VALUE;
	val->data.elementValue = element;
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
	
	// Validar que la condición sea una expresión booleana
	if (condition == NULL || 
		(condition->type != BOOLEAN_EXPR && 
		 condition->type != BINARY_EXPR && 
		 condition->type != UNARY_EXPR)) {
		logError(_logger, "Invalid condition in if statement - must be a boolean expression");
		_currentState->succeed = false;
		return NULL;
	}
	
	// Validar que las ramas tengan la estructura correcta
	if (thenBranch == NULL || !isFirstPairType(thenBranch->pairs)) {
		logError(_logger, "Then branch must be a valid HTML element with 'type' as first field");
		_currentState->succeed = false;
		return NULL;
	}
	
	if (elseBranch != NULL && !isFirstPairType(elseBranch->pairs)) {
		logError(_logger, "Else branch must be a valid HTML element with 'type' as first field");
		_currentState->succeed = false;
		return NULL;
	}
	
	Conditional * conditional = calloc(1, sizeof(Conditional));
	conditional->condition = condition;
	conditional->thenBranch = thenBranch;
	conditional->elseBranch = elseBranch;
	return conditional;
}

Conditional * ObjectConditionalSemanticAction(Object * condObject) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	Conditional * conditional = calloc(1, sizeof(Conditional));
	
	// Extract condition, then and else from the object
	PairList * currentPair = condObject->pairs;
	Object * thenObj = NULL;
	Object * elseObj = NULL;
	Expression * condExpr = NULL;
	
	while (currentPair != NULL) {
		if (strcmp(currentPair->pair->key, "condition") == 0) {
			// Convert value to expression
			if (currentPair->pair->value->type == BOOLEAN_VALUE) {
				condExpr = BooleanExpressionSemanticAction(currentPair->pair->value->data.booleanValue);
			}
			// Handle other types if needed
		} else if (strcmp(currentPair->pair->key, "then") == 0 && currentPair->pair->value->type == OBJECT_VALUE) {
			thenObj = currentPair->pair->value->data.objectValue;
		} else if (strcmp(currentPair->pair->key, "else") == 0 && currentPair->pair->value->type == OBJECT_VALUE) {
			elseObj = currentPair->pair->value->data.objectValue;
		}
		currentPair = currentPair->next;
	}
	
	conditional->condition = condExpr;
	conditional->thenBranch = thenObj;
	conditional->elseBranch = elseObj;
	
	return conditional;
}

Loop * LoopSemanticAction(Expression * initialization, Expression * condition, Expression * increment, Object * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	// Validar que la condición sea una expresión booleana
	if (condition == NULL || 
		(condition->type != BOOLEAN_EXPR && 
		 condition->type != BINARY_EXPR && 
		 condition->type != UNARY_EXPR)) {
		logError(_logger, "Invalid condition in for loop - must be a boolean expression");
		_currentState->succeed = false;
		return NULL;
	}
	
	// Validar que el cuerpo tenga la estructura correcta
	if (body == NULL || !isFirstPairType(body->pairs)) {
		logError(_logger, "Loop body must be a valid HTML element with 'type' as first field");
		_currentState->succeed = false;
		return NULL;
	}
	
	Loop * loop = calloc(1, sizeof(Loop));
	loop->initialization = initialization;
	loop->condition = condition;
	loop->increment = increment;
	loop->body = body;
	return loop;
}

VarRef * VariableRefSemanticAction(char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	// Validar que la variable esté definida
	if (!isVariableDefined(name)) {
		logError(_logger, "Variable '%s' is not defined", name);
		_currentState->succeed = false;
		return NULL;
	}
	
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

Element * ElementSemanticAction(Object * object) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Element * element = calloc(1, sizeof(Element));
	
	// Extract the type and content from the object
	PairList * currentPair = object->pairs;
	char * type = NULL;
	Value * content = NULL;
	AttributeList * attributes = NULL;
	
	while (currentPair != NULL) {
		if (strcmp(currentPair->pair->key, "type") == 0 && currentPair->pair->value->type == STRING_VALUE) {
			type = currentPair->pair->value->data.stringValue;
		} else if (strcmp(currentPair->pair->key, "content") == 0) {
			content = currentPair->pair->value;
		} else {
			// Other attributes can be processed here
			attributes = attributeListSemanticAction(attributes, 
				AttributeSemanticAction(currentPair->pair->key, currentPair->pair->value));
		}
		currentPair = currentPair->next;
	}
	
	element->tag = type;
	element->attributes = attributes;
	
	// Handle content value - could be a single value or an array
	if (content != NULL) {
		if (content->type == ARRAY_VALUE) {
			// Content is an array, use its values directly
			ValueList * contentValues = content->data.arrayValue->values;
			element->children = contentValues;
		} else {
			// Content is a single value, create a value list with just this value
			element->children = singleValueListSemanticAction(content);
		}
	} else {
		element->children = NULL;
	}
	
	return element;
}


