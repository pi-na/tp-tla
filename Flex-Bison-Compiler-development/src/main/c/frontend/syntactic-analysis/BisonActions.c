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

// Lista de elementos HTML válidos
static const char* validElements[] = {
	"html", "head", "body", "title", "p", "div", "span", "a", "img", "ul", "ol", "li", "h1", "h2", "h3", "h4", "h5", "h6", NULL
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

// Función para validar compatibilidad de contenido
static boolean isContentCompatible(const char* elementType, Value* content) {
	if (content == NULL) return true;  // Contenido vacío es válido

	// Elementos que solo aceptan texto o referencias a variables
	const char* textOnlyElements[] = {"title", "p", "span", "h1", "div", NULL};
	boolean isTextOnly = false;
	
	for (int i = 0; textOnlyElements[i] != NULL; i++) {
		if (strcmp(elementType, textOnlyElements[i]) == 0) {
			isTextOnly = true;
			break;
		}
	}

	if (isTextOnly) {
		// Permitir string, referencia a variable, u objeto que representa una referencia a variable
		if (content->type == STRING_VALUE || content->type == VAR_REF_VALUE) {
			return true;
		}
		
		// Verificar si es un objeto que representa una referencia a variable
		if (content->type == OBJECT_VALUE) {
			Object* obj = content->data.objectValue;
			if (obj != NULL && obj->pairs != NULL && obj->pairs->pair != NULL &&
				strcmp(obj->pairs->pair->key, "var") == 0 && 
				obj->pairs->pair->value != NULL &&
				obj->pairs->pair->value->type == STRING_VALUE) {
				return true;
			}
		}
		
		logError(_logger, "Element '%s' can only contain text content or variable references", elementType);
		return false;
	}

	// Elementos que solo aceptan otros elementos
	const char* containerElements[] = {"html", "head", "body", "div", "ul", "ol", NULL};
	boolean isContainer = false;
	
	for (int i = 0; containerElements[i] != NULL; i++) {
		if (strcmp(elementType, containerElements[i]) == 0) {
			isContainer = true;
			break;
		}
	}

	if (isContainer && content->type == STRING_VALUE) {
		logError(_logger, "Element '%s' cannot contain direct text content", elementType);
		return false;
	}

	return true;
}

// Función auxiliar para verificar si un objeto es una referencia a variable
static boolean isVarRefObject(Object* object) {
	if (object == NULL || object->pairs == NULL || object->pairs->pair == NULL) {
		return false;
	}
	return strcmp(object->pairs->pair->key, "var") == 0;
}

// Modificar la función validateHtmlObject para incluir la validación de contenido
static boolean validateHtmlObject(Object* object) {
	if (object == NULL || object->pairs == NULL) return false;

	// Si es un objeto que representa una referencia a variable, no necesita validación HTML
	if (isVarRefObject(object)) {
		return true;
	}

	// Verificar que tenga un campo "type" válido
	PairList* pairs = object->pairs;
	boolean hasType = false;
	boolean hasValidType = false;
	const char* elementType = NULL;
	Value* contentValue = NULL;

	while (pairs != NULL) {
		if (strcmp(pairs->pair->key, "type") == 0) {
			hasType = true;
			if (pairs->pair->value->type == STRING_VALUE) {
				elementType = pairs->pair->value->data.stringValue;
				hasValidType = isValidElement(elementType);
				if (!hasValidType) {
					logError(_logger, "Invalid HTML element type: %s", elementType);
					return false;
				}
			}
		} else if (strcmp(pairs->pair->key, "content") == 0) {
			contentValue = pairs->pair->value;
		}
		pairs = pairs->next;
	}

	if (!hasType || !hasValidType) {
		logError(_logger, "Missing or invalid 'type' field in HTML element");
		return false;
	}

	// Validar compatibilidad de contenido
	if (!isContentCompatible(elementType, contentValue)) {
		return false;
	}

	// Validar recursivamente el contenido
	if (contentValue != NULL) {
		if (contentValue->type == OBJECT_VALUE) {
			if (!validateHtmlObject(contentValue->data.objectValue)) {
				return false;
			}
		} else if (contentValue->type == ARRAY_VALUE) {
			ValueList* values = contentValue->data.arrayValue->values;
			while (values != NULL) {
				if (values->value->type == OBJECT_VALUE) {
					if (!validateHtmlObject(values->value->data.objectValue)) {
						return false;
					}
				}
				values = values->next;
			}
		}
	}

	return true;
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

	// Validar la estructura HTML si este objeto es un elemento HTML
	if (isFirstPairType(pairs)) {
		if (!validateHtmlObject(object)) {
			_currentState->succeed = false;
			return NULL;
		}
	}
	
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

Loop * LoopSemanticAction(char * iteratorName, Object * iterable, Object * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	// Validar que el cuerpo tenga la estructura correcta
	if (body == NULL || !isFirstPairType(body->pairs)) {
		logError(_logger, "Loop body must be a valid HTML element with 'type' as first field");
		_currentState->succeed = false;
		return NULL;
	}
	
	// Validar que el iterable sea un objeto válido
	if (iterable == NULL) {
		logError(_logger, "Loop iterable must be a valid object");
		_currentState->succeed = false;
		return NULL;
	}
	
	// Definir la variable iteradora
	defineVariable(iteratorName);
	
	Loop * loop = calloc(1, sizeof(Loop));
	loop->iteratorName = iteratorName;
	loop->iterable = iterable;
	loop->body = body;
	return loop;
}

static boolean validateVariableRef(const char* name) {
	if (name == NULL) return false;
	
	// Verificar si la variable está siendo resuelta (referencia circular)
	for (int i = 0; i < variableCount; i++) {
		if (strcmp(variables[i].name, name) == 0) {
			if (variables[i].isBeingResolved) {
				logError(_logger, "Circular reference detected for variable '%s'", name);
				return false;
			}
			return true;
		}
	}
	
	logError(_logger, "Variable '%s' is not defined", name);
	return false;
}

VarRef * VariableRefSemanticAction(char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	
	if (!validateVariableRef(name)) {
		_currentState->succeed = false;
		return NULL;
	}
	
	VarRef * varRef = calloc(1, sizeof(VarRef));
	varRef->name = name;
	
	// Marcar la variable como en proceso de resolución
	setVariableResolutionStatus(name, true);
	
	return varRef;
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


