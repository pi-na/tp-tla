#include "HtmlGenerator.h"
#include "../../shared/Type.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

/* MODULE INTERNAL STATE */

const char _indentationCharacter = ' ';
const char _indentationSize = 2;
static Logger * _logger = NULL;

void initializeHtmlGeneratorModule() {
    _logger = createLogger("HtmlGenerator");
}

void shutdownHtmlGeneratorModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

/** PRIVATE FUNCTIONS */

static void _generateHtmlPrologue(void);
static void _generateHtmlEpilogue(void);
static void _generateValue(const unsigned int indentationLevel, Value * value);
static void _generateObject(const unsigned int indentationLevel, Object * object);
static void _generateArray(const unsigned int indentationLevel, Array * array);
static void _generatePair(const unsigned int indentationLevel, Pair * pair);
static char * _indentation(const unsigned int level);
static void _output(const unsigned int indentationLevel, const char * const format, ...);
static char * _escapeHtml(const char * input);
static char * _keywordToString(Keyword * keyword);

/**
 * Convierte una keyword a string
 */
static char * _keywordToString(Keyword * keyword) {
    if (!keyword) return strdup("");
    
    // Aquí deberías mapear los tokens a strings apropiados
    // Por simplicidad, retornamos un string genérico
    return strdup("keyword");
}

/**
 * Escapa caracteres HTML especiales
 */
static char * _escapeHtml(const char * input) {
    if (!input) return strdup("");
    
    size_t len = strlen(input);
    size_t newLen = len * 6 + 1; // Peor caso: todos los caracteres necesitan escape
    char * escaped = malloc(newLen);
    char * ptr = escaped;
    
    for (size_t i = 0; i < len; i++) {
        switch (input[i]) {
            case '<':
                strcpy(ptr, "&lt;");
                ptr += 4;
                break;
            case '>':
                strcpy(ptr, "&gt;");
                ptr += 4;
                break;
            case '&':
                strcpy(ptr, "&amp;");
                ptr += 5;
                break;
            case '"':
                strcpy(ptr, "&quot;");
                ptr += 6;
                break;
            case '\'':
                strcpy(ptr, "&#39;");
                ptr += 5;
                break;
            default:
                *ptr++ = input[i];
                break;
        }
    }
    *ptr = '\0';
    
    return escaped;
}

/**
 * Crea el prólogo del HTML generado
 */
static void _generateHtmlPrologue(void) {
    _output(0, "%s",
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "    <meta charset=\"UTF-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <title>Generated from JSON</title>\n"
        "    <style>\n"
        "        body { font-family: Arial, sans-serif; margin: 20px; }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <h1>HEADER DEBUG! Eliminame antes de entregar</h1>\n"
    );
}

/**
 * Crea el epílogo del HTML generado
 */
static void _generateHtmlEpilogue(void) {
    _output(0, "%s",
        "</body>\n"
        "</html>\n"
    );
}


/**
 * Genera la indentación para el nivel especificado
 */
static char * _indentation(const unsigned int level) {
    return indentation(_indentationCharacter, level, _indentationSize);
}


/**
 * Genera output formateado a stdout
 */
static void _output(const unsigned int indentationLevel, const char * const format, ...) {
    va_list arguments;
    va_start(arguments, format);
    char * indentationStr = _indentation(indentationLevel);
    char * effectiveFormat = concatenate(2, indentationStr, format);
    vfprintf(stdout, effectiveFormat, arguments);
    fflush(stdout);
    free(effectiveFormat);
    free(indentationStr);
    va_end(arguments);
}


/**
 * Genera HTML para el PROGRAM -- PUNTO DE ENTRADA
 */
static void _generateProgram(const Program * program) {
    // El program siempre es un object
    _generateObject(1, program->object);
}


static void _generateObject(const unsigned int indentationLevel, Object * object) {
    // Extraer type, content y atributos del objeto
    char * tagName = NULL;
    Value * contentVal = NULL;
    char * attributesBuffer = NULL;
    size_t attributesBufferSize = 0;
    
    // extraer type, content y acumular atributos
    for(PairList * current = object->pairs; current != NULL; current = current->next) {
        Pair * pair = current->pair;
        
        if (pair->key->type == TYPE) {
            if (pair->value->type == STRING_VALUE) {
                tagName = pair->value->data.stringValue;
            }
        } else if (pair->key->type == CONTENT) {
            contentVal = pair->value;
        } else {
            char * attrName = TOKEN_STRINGS[pair->key->type];
            char * attrValue = pair->value->data.stringValue;
            char * attrString = NULL;
            
            // Crear string del atributo
            if (asprintf(&attrString, " %s=\"%s\"", _toLowerCase(attrName), _escapeHtml(attrValue)) == -1) {
                logError(_logger, "Failed to allocate memory for attribute string");
                continue;
            }
            
            // Agregar al buffer
            size_t newSize = attributesBufferSize + strlen(attrString);
            char * newBuffer = realloc(attributesBuffer, newSize + 1);
            if (newBuffer == NULL) {
                logError(_logger, "Failed to reallocate attributes buffer");
                free(attrString);
                continue;
            }
            
            attributesBuffer = newBuffer;
            strcat(attributesBuffer, attrString);
            attributesBufferSize = newSize;
            
            free(attrString);
        }
    }
    
    if (tagName == NULL) {
        logError(_logger, "Object missing TYPE field");
        free(attributesBuffer);
        return;
    }
    
    // Generar HTML completo en una sola operación
    if (attributesBuffer != NULL) {
        _output(indentationLevel, "<%s%s>\n", tagName, attributesBuffer);
    } else {
        _output(indentationLevel, "<%s>\n", tagName);
    }
    
    // Renderizar contenido
    if (contentVal != NULL) {
        _generateValue(indentationLevel + 1, contentVal);
    }
    
    // Cerrar tag
    _output(indentationLevel, "</%s>\n", tagName);
    
    // Limpiar buffer
    free(attributesBuffer);
}


static void generateContent(const unsigned int indentationLevel, Pair * pair) {
    // Genera el contenido del par
    switch(pair->value->type) {
        case OBJECT_VALUE:
            _generateObject(indentationLevel + 1, pair->value->data.objectValue);
            break;
        case ARRAY_VALUE:
            _generateArray(indentationLevel + 1, pair->value->data.arrayValue);
            break;
        default:
            logError(_logger, "Unsupported value type in array: %d", pair->value->type);
            break;
    }
}


static void _generateArray(const unsigned int indentationLevel, Array * array) {
    // FOR EACH array->values
    for(ValueList * current = array->values; current != NULL; current = current->next) {
        switch(current->value->type) {
            case OBJECT_VALUE:
                _generateObject(indentationLevel + 1, current->value->data.objectValue);
                break;
            case ARRAY_VALUE:
                _generateArray(indentationLevel + 1, current->value->data.arrayValue);
                break;
            default:
                logError(_logger, "Unsupported value type in array: %d", current->value->type);
                break;
            
        }
    }
}


void generateHtml(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	_generateHtmlPrologue();
    // ->abstractSyntaxTree seria el nodo raiz, tipo Program
	_generateProgram(compilerState->abstractSyntaxtTree);
	_generateHtmlEpilogue();
	logDebugging(_logger, "Generation is done.");
}