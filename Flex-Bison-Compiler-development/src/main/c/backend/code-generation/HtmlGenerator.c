#include "HtmlGenerator.h"
#include "../../shared/Type.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"

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
        "        .json-object { border: 1px solid #ccc; padding: 10px; margin: 5px 0; }\n"
        "        .json-array { border: 1px solid #999; padding: 10px; margin: 5px 0; }\n"
        "        .json-pair { margin: 5px 0; }\n"
        "        .json-key { font-weight: bold; color: #0066cc; }\n"
        "        .json-value { margin-left: 10px; }\n"
        "        .json-string { color: #009900; }\n"
        "        .json-number { color: #cc6600; }\n"
        "        .json-boolean { color: #cc0066; }\n"
        "        .json-null { color: #666666; font-style: italic; }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <h1>JSON to HTML Conversion</h1>\n"
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
 * Genera HTML para un par clave-valor
 */
static void _generatePair(const unsigned int indentationLevel, Pair * pair) {
    if (!pair) return;
    
    _output(indentationLevel, "<div class=\"json-pair\">\n");
    
    // Generar la clave
    _output(indentationLevel + 1, "<span class=\"json-key\">");
    if (pair->key) {
        char * keyStr = _keywordToString(pair->key);
        char * escapedKey = _escapeHtml(keyStr);
        _output(0, "%s", escapedKey);
        free(keyStr);
        free(escapedKey);
    }
    _output(0, ":</span>\n");
    
    // Generar el valor
    _output(indentationLevel + 1, "<div class=\"json-value\">\n");
    _generateValue(indentationLevel + 2, pair->value);
    _output(indentationLevel + 1, "</div>\n");
    
    _output(indentationLevel, "</div>\n");
}

/**
 * Genera HTML para un array
 */
static void _generateArray(const unsigned int indentationLevel, Array * array) {
    if (!array) return;
    
    _output(indentationLevel, "<div class=\"json-array\">\n");
    _output(indentationLevel + 1, "<h3>Array:</h3>\n");
    
    ValueList * current = array->values;
    int index = 0;
    while (current) {
        _output(indentationLevel + 1, "<div class=\"array-item\">\n");
        _output(indentationLevel + 2, "<strong>Item %d:</strong>\n", index);
        _generateValue(indentationLevel + 2, current->value);
        _output(indentationLevel + 1, "</div>\n");
        current = current->next;
        index++;
    }
    
    _output(indentationLevel, "</div>\n");
}

/**
 * Genera HTML para un objeto
 */
static void _generateObject(const unsigned int indentationLevel, Object * object) {
    if (!object) return;
    
    _output(indentationLevel, "<div class=\"json-object\">\n");
    
    PairList * current = object->pairs;
    while (current) {
        _generatePair(indentationLevel + 1, current->pair);
        current = current->next;
    }
    
    _output(indentationLevel, "</div>\n");
}

/**
 * Genera HTML para un valor
 */
static void _generateValue(const unsigned int indentationLevel, Value * value) {
    if (!value) {
        _output(indentationLevel, "<span class=\"json-null\">null</span>\n");
        return;
    }
    
    switch (value->type) {
        case STRING_VALUE: {
            char * escaped = _escapeHtml(value->data.stringValue);
            _output(indentationLevel, "<span class=\"json-string\">\"%s\"</span>\n", escaped);
            free(escaped);
            break;
        }
        case NULL_VALUE:
            _output(indentationLevel, "<span class=\"json-null\">null</span>\n");
            break;
        case OBJECT_VALUE:
            _generateObject(indentationLevel, value->data.objectValue);
            break;
        case ARRAY_VALUE:
            _generateArray(indentationLevel, value->data.arrayValue);
            break;
        case TOKEN_VALUE:
            _output(indentationLevel, "<span class=\"json-token\">token</span>\n");
            break;
        default:
            _output(indentationLevel, "<span class=\"json-unknown\">unknown</span>\n");
            break;
    }
}

void generateHtml(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	_generateHtmlPrologue();
	_generateProgram(compilerState->abstractSyntaxtTree);
	_generateHtmlEpilogue();
	logDebugging(_logger, "Generation is done.");
}