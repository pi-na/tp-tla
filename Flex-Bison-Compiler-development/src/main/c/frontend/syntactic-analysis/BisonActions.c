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

// Acciones para valores
Value* createStringValue(char* string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value* value = calloc(1, sizeof(Value));
	value->type = STRING_VALUE;
	value->string = string;
	return value;
}

Value* createIntegerValue(int integer) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value* value = calloc(1, sizeof(Value));
	value->type = INTEGER_VALUE;
	value->integer = integer;
	return value;
}

Value* createRealValue(double real) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value* value = calloc(1, sizeof(Value));
	value->type = REAL_VALUE;
	value->real = real;
	return value;
}

Value* createBooleanValue(int boolean) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value* value = calloc(1, sizeof(Value));
	value->type = BOOLEAN_VALUE;
	value->boolean = boolean;
	return value;
}

Value* createNullValue() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value* value = calloc(1, sizeof(Value));
	value->type = NULL_VALUE;
	return value;
}

Value* createObjectValue(Object* object) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value* value = calloc(1, sizeof(Value));
	value->type = OBJECT_VALUE;
	value->object = object;
	return value;
}

Value* createArrayValue(Array* array) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Value* value = calloc(1, sizeof(Value));
	value->type = ARRAY_VALUE;
	value->array = array;
	return value;
}

// Acciones para estilos
StyleValue* createStringStyleValue(char* string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StyleValue* value = calloc(1, sizeof(StyleValue));
	value->type = STRING_STYLE;
	value->string = string;
	return value;
}

StyleValue* createIntegerStyleValue(int integer) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StyleValue* value = calloc(1, sizeof(StyleValue));
	value->type = INTEGER_STYLE;
	value->integer = integer;
	return value;
}

StyleValue* createRealStyleValue(double real) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StyleValue* value = calloc(1, sizeof(StyleValue));
	value->type = REAL_STYLE;
	value->real = real;
	return value;
}

StyleValue* createCSSKeywordStyleValue(char* keyword) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StyleValue* value = calloc(1, sizeof(StyleValue));
	value->type = CSS_KEYWORD_STYLE;
	value->cssKeyword = keyword;
	return value;
}

StyleProperty* createStyleProperty(char* key, StyleValue* value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StyleProperty* property = calloc(1, sizeof(StyleProperty));
	property->key = key;
	property->value = value;
	return property;
}

StyleProperties* createStyleProperties() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StyleProperties* properties = calloc(1, sizeof(StyleProperties));
	properties->properties = NULL;
	properties->count = 0;
	return properties;
}

void addStyleProperty(StyleProperties* properties, StyleProperty* property) {
	properties->count++;
	properties->properties = realloc(properties->properties, 
								  sizeof(StyleProperty*) * properties->count);
	properties->properties[properties->count - 1] = property;
}

// Acciones para entradas y objetos
Entry* createEntry(char* key, Value* value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Entry* entry = calloc(1, sizeof(Entry));
	entry->key = key;
	entry->value = value;
	return entry;
}

Entries* createEntries() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Entries* entries = calloc(1, sizeof(Entries));
	entries->entries = NULL;
	entries->count = 0;
	return entries;
}

void addEntry(Entries* entries, Entry* entry) {
	entries->count++;
	entries->entries = realloc(entries->entries, 
							 sizeof(Entry*) * entries->count);
	entries->entries[entries->count - 1] = entry;
}

// Acciones para arrays
Array* createArray() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Array* array = calloc(1, sizeof(Array));
	array->elements = NULL;
	array->count = 0;
	return array;
}

void addArrayElement(Array* array, Value* element) {
	array->count++;
	array->elements = realloc(array->elements, 
							sizeof(Value*) * array->count);
	array->elements[array->count - 1] = element;
}

// Acciones para objetos
Object* createObject(char* type, Entries* entries) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Object* object = calloc(1, sizeof(Object));
	object->type = type;
	object->entries = entries;
	return object;
}

// Acciones para elementos HTML
HTMLElement* createHTMLElement(HTMLElementType type, Object* attributes) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	HTMLElement* element = calloc(1, sizeof(HTMLElement));
	element->type = type;
	element->attributes = attributes;
	return element;
}

HTMLElementHTML* createHTMLElementHTML(Object* head, Object* body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	HTMLElementHTML* element = calloc(1, sizeof(HTMLElementHTML));
	element->base.type = HTML_ELEMENT;
	element->head = head;
	element->body = body;
	return element;
}

HTMLElementHead* createHTMLElementHead(Object* title) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	HTMLElementHead* element = calloc(1, sizeof(HTMLElementHead));
	element->base.type = HEAD_ELEMENT;
	element->title = title;
	return element;
}

HTMLElementTitle* createHTMLElementTitle(char* content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	HTMLElementTitle* element = calloc(1, sizeof(HTMLElementTitle));
	element->base.type = TITLE_ELEMENT;
	element->content = content;
	return element;
}

HTMLElementBody* createHTMLElementBody(Array* content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	HTMLElementBody* element = calloc(1, sizeof(HTMLElementBody));
	element->base.type = BODY_ELEMENT;
	element->content = content;
	return element;
}

HTMLElementDiv* createHTMLElementDiv(Array* content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	HTMLElementDiv* element = calloc(1, sizeof(HTMLElementDiv));
	element->base.type = DIV_ELEMENT;
	element->content = content;
	return element;
}

HTMLElementP* createHTMLElementP(Value* content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	HTMLElementP* element = calloc(1, sizeof(HTMLElementP));
	element->base.type = P_ELEMENT;
	element->content = content;
	return element;
}

HTMLElementH1* createHTMLElementH1(Value* content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	HTMLElementH1* element = calloc(1, sizeof(HTMLElementH1));
	element->base.type = H1_ELEMENT;
	element->content = content;
	return element;
}

HTMLElementImg* createHTMLElementImg(char* src, char* alt) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	HTMLElementImg* element = calloc(1, sizeof(HTMLElementImg));
	element->base.type = IMG_ELEMENT;
	element->src = src;
	element->alt = alt;
	return element;
}

HTMLElementA* createHTMLElementA(char* href, char* ref, char* extRef, Value* content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	HTMLElementA* element = calloc(1, sizeof(HTMLElementA));
	element->base.type = A_ELEMENT;
	element->href = href;
	element->ref = ref;
	element->extRef = extRef;
	element->content = content;
	return element;
}

HTMLElementCenter* createHTMLElementCenter(Array* content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	HTMLElementCenter* element = calloc(1, sizeof(HTMLElementCenter));
	element->base.type = CENTER_ELEMENT;
	element->content = content;
	return element;
}

// Acción para el programa
Program* createProgram(HTMLElement* root) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program* program = calloc(1, sizeof(Program));
	program->root = root;
	return program;
}

// Funciones de liberación de memoria
void releaseValue(Value* value) {
	if (value == NULL) return;
	
	switch (value->type) {
		case STRING_VALUE:
			free(value->string);
			break;
		case OBJECT_VALUE:
			releaseObject(value->object);
			break;
		case ARRAY_VALUE:
			releaseArray(value->array);
			break;
		default:
			break;
	}
	free(value);
}

void releaseStyleValue(StyleValue* value) {
	if (value == NULL) return;
	
	switch (value->type) {
		case STRING_STYLE:
			free(value->string);
			break;
		case CSS_KEYWORD_STYLE:
			free(value->cssKeyword);
			break;
		default:
			break;
	}
	free(value);
}

void releaseStyleProperty(StyleProperty* property) {
	if (property == NULL) return;
	free(property->key);
	releaseStyleValue(property->value);
	free(property);
}

void releaseStyleProperties(StyleProperties* properties) {
	if (properties == NULL) return;
	for (int i = 0; i < properties->count; i++) {
		releaseStyleProperty(properties->properties[i]);
	}
	free(properties->properties);
	free(properties);
}

void releaseEntry(Entry* entry) {
	if (entry == NULL) return;
	free(entry->key);
	releaseValue(entry->value);
	free(entry);
}

void releaseEntries(Entries* entries) {
	if (entries == NULL) return;
	for (int i = 0; i < entries->count; i++) {
		releaseEntry(entries->entries[i]);
	}
	free(entries->entries);
	free(entries);
}

void releaseArray(Array* array) {
	if (array == NULL) return;
	for (int i = 0; i < array->count; i++) {
		releaseValue(array->elements[i]);
	}
	free(array->elements);
	free(array);
}

void releaseObject(Object* object) {
	if (object == NULL) return;
	free(object->type);
	releaseEntries(object->entries);
	free(object);
}

void releaseHTMLElement(HTMLElement* element) {
	if (element == NULL) return;
	releaseObject(element->attributes);
	free(element);
}

void releaseProgram(Program* program) {
	if (program == NULL) return;
	releaseHTMLElement(program->root);
	free(program);
}
