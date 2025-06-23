#include "HtmlGenerator.h"
#include "../../shared/Type.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

/* MODULE INTERNAL STATE */

const char _indentationCharacter = ' ';
const char _indentationSize = 2;
CompilerState * _compilerState = NULL; 
static Logger * _logger = NULL;
static FILE * _file_stream = NULL;

static void _generateHtmlPrologue(void);
static void _generateHtmlEpilogue(void);
static void _generateValue(const unsigned int indentationLevel, Value * value);
static void _generateObject(const unsigned int indentationLevel, Object * object);
static void _generateArray(const unsigned int indentationLevel, Array * array);
static void _generatePair(const unsigned int indentationLevel, Pair * pair);
static char * _indentation(const unsigned int level);
static void _output(const unsigned int indentationLevel, const char * const format, ...);
static char * _keywordToString(Keyword * keyword);
static char * _toLowerCase(const char * s);
static char * _tokenToString(int tokenValue);

void initializeHtmlGeneratorModule(const char *outputPath) {
    _logger = createLogger("HtmlGenerator");
    setHtmlOutputFile(outputPath);
}

void shutdownHtmlGeneratorModule() {
     if (_file_stream && _file_stream != stdout) {
        logDebugging(_logger,
                     "Closing HTML output file.");
        fclose(_file_stream);
        _file_stream = NULL;
    }
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

/**
 * Abre el fichero de salida para volcar el HTML.
 * Si falla, deja stdout como fallback.
 */
void setHtmlOutputFile(const char *filePath) {
    if (_file_stream && _file_stream != stdout) {
        fclose(_file_stream);
    }
    _file_stream = fopen(filePath, "w");
    if (!_file_stream) {
        logError(_logger,
                 "Cannot open HTML output file \"%s\": %s",
                 filePath);
        _file_stream = stdout;
    } else {
        logDebugging(_logger,
                     "Opened HTML output file: %s", filePath);
    }
}

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
 * Crea el prólogo del HTML generado
 */
static void _generateHtmlPrologue(void) {
    _output(0,
      "<!DOCTYPE html>\n"
    );
}

/**
 * Crea el epílogo del HTML generado
 */
// static void _generateHtmlEpilogue(void) {
//     _output(0, "%s",
//         "</body>\n"
//         "</html>\n"
//     );
// }


/**
 * Genera la indentación para el nivel especificado
 */
static char * _indentation(const unsigned int level) {
    return indentation(_indentationCharacter, level, _indentationSize);
}


/**
 * Genera output formateado a `stream`
 */
static void _output(const unsigned int indentationLevel,
                    const char * const format,
                    ...)
{
    va_list args;
    va_start(args, format);

    char *indentationStr   = _indentation(indentationLevel);
    char *effectiveFormat  = concatenate(2, indentationStr, format);

    vfprintf(_file_stream, effectiveFormat, args);
    fflush(_file_stream);

    free(effectiveFormat);
    free(indentationStr);
    va_end(args);
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
    char * attributesBuffer = malloc(1);
    attributesBuffer[0] = '\0';
    size_t attributesBufferSize = 0;
    
    for(PairList * current = object->pairs; current != NULL; current = current->next) {
        Pair * pair = current->pair;
        
        logDebugging(_logger, "Processing pair with key type: %d (TYPE=%d)", pair->key->type, TYPE);
        
        if (pair->key->type == LOOP) {
            Object *loopObj = pair->value->data.objectValue;
            Array  *iterableArray    = NULL;
            Object *iterateTemplate  = NULL;

            for (PairList *pl = loopObj->pairs; pl; pl = pl->next) {
                if (pl->pair->key->type == ITERABLE
                && pl->pair->value->type == ARRAY_VALUE) {
                    iterableArray = pl->pair->value->data.arrayValue;
                }
                else if (pl->pair->key->type == ITERATE
                    && pl->pair->value->type == OBJECT_VALUE) {
                    iterateTemplate = pl->pair->value->data.objectValue;
                }
            }

            if (!iterableArray || !iterateTemplate) {
                logError(_logger, "Invalid LOOP: falta iterable o iterate");
                free(attributesBuffer);
                return;
            }

            // por cada elemento, push $iterator, generate, pop
            for (ValueList *vl = iterableArray->values; vl; vl = vl->next) {
                Value *v = vl->value;

                if (v->type != STRING_VALUE) {
                    logError(_logger, "Elemento no es STRING_VALUE: %d", v->type);
                    continue;
                }

                symbolTablePush(
                    _compilerState->symbolTable,
                    "ITERATOR_REF",
                    v->data.stringValue
                );
                _generateObject(indentationLevel + 1, iterateTemplate);
                char *popped = NULL;
                symbolTablePop(_compilerState->symbolTable, &popped);
                free(popped);
            }

            free(attributesBuffer);
            return;
        }

        if (pair->key->type == TYPE) {
            if (pair->value->type == STRING_VALUE) {
                tagName = pair->value->data.stringValue;
                logDebugging(_logger, "Found TYPE field with value: %s", tagName);
            } else if (pair->value->type == TOKEN_VALUE) {
                logDebugging(_logger, "TOKEN_VALUE detected with token: %d", pair->value->data.tokenValue);
                tagName = _tokenToString(pair->value->data.tokenValue);
                logDebugging(_logger, "Found TYPE field with token value: %s (token: %d)", tagName ? tagName : "(null)", pair->value->data.tokenValue);
            }
        } else if (pair->key->type == CONTENT) {
            contentVal = pair->value;
            logDebugging(_logger, "Found CONTENT field");
        } else if (pair->key->type == STYLE) {
            if (pair->value->type == OBJECT_VALUE) {
                Object *styleObj = pair->value->data.objectValue;
                size_t cssBufSize = 0;
                char *cssBuf = malloc(1);
                cssBuf[0] = '\0';
                for (PairList *sp = styleObj->pairs; sp; sp = sp->next) {
                    Pair *stylePair = sp->pair;
                    const char *prop = _tokenToString(stylePair->key->type);
                    const char *val = stylePair->value->data.stringValue;
                    char *fragment;
                    if (asprintf(&fragment, "%s:%s;", prop, val) == -1) continue;
                    size_t fragLen = strlen(fragment);
                    cssBuf = realloc(cssBuf, cssBufSize + fragLen + 1);
                    memcpy(cssBuf + cssBufSize, fragment, fragLen);
                    cssBufSize += fragLen;
                    cssBuf[cssBufSize] = '\0';
                    free(fragment);
                }
                char *attrString;
                if (asprintf(&attrString, " style=\"%s\"", cssBuf) != -1) {
                size_t newSize = attributesBufferSize + strlen(attrString);
                attributesBuffer = realloc(attributesBuffer, newSize + 1);
                strcpy(attributesBuffer + attributesBufferSize, attrString);
                attributesBufferSize = newSize;
                free(attrString);
                }
                free(cssBuf);
                continue;
            } 
        }else {
            const char * attrName = _tokenToString(pair->key->type);
            char * attrValue = pair->value->data.stringValue;
            char * attrString = NULL;
            
            // Crear string del atributo
            char *lower = _toLowerCase(attrName);
            if (asprintf(&attrString, " %s=\"%s\"", lower, attrValue) == -1) {
                logError(_logger, "Failed to allocate memory for attribute string");
                continue;
            }

            free(lower);
            
            // Agregar al buffer
            
            size_t newSize = attributesBufferSize + strlen(attrString);
            char * newBuffer = realloc(attributesBuffer, newSize + 1);
            if (!newBuffer) {
                logError(_logger, "Failed to reallocate attributes buffer");
                free(attrString);
                continue;
            }
            
            attributesBuffer = newBuffer;
            attributesBuffer[attributesBufferSize] = '\0';
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

static char* _toLowerCase(const char* s) {
    if (!s) return strdup("");
    size_t len = strlen(s);
    char* result = malloc(len + 1);
    if (!result) return NULL;  // o maneja el error como prefieras
    for (size_t i = 0; i < len; i++) {
        result[i] = tolower((unsigned char)s[i]);
    }
    result[len] = '\0';
    return result;
}

static void _generateValue(const unsigned int indentationLevel, Value * value) {
    switch (value->type) {
        case STRING_VALUE:
            _output(indentationLevel, "%s\n", value->data.stringValue);
            break;
         case TOKEN_VALUE: {
            if (value->data.tokenValue == ITERATOR_REF) {
                char *iterVal = NULL;
                if (symbolTableGetValue(_compilerState->symbolTable,
                                        "ITERATOR_REF",
                                        &iterVal)) {
                    _output(indentationLevel, "%s\n", iterVal);
                    free(iterVal);
                } else {
                    _output(indentationLevel, "\n");
                }
            } else {
                // Si quisieras imprimir otros tokens crudos:
                const char *s = _tokenToString(value->data.tokenValue);
                _output(indentationLevel, "%s\n", s ? s : "");
            }
            break;
        }
        case OBJECT_VALUE:
            _generateObject(indentationLevel, value->data.objectValue);
            break;
        case ARRAY_VALUE:
            _generateArray(indentationLevel, value->data.arrayValue);
            break;
        default:
            logError(_logger, "Unknown Value type: %d", value->type);
            break;
    }
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
    _compilerState = compilerState;
	_generateHtmlPrologue();
    // ->abstractSyntaxTree seria el nodo raiz, tipo Program
	_generateProgram(compilerState->abstractSyntaxtTree);
	// _generateHtmlEpilogue();
	logDebugging(_logger, "Generation is done.");
}

static char * _tokenToString(int tokenValue) {
    switch (tokenValue) {
        case TYPE: return "type";
        case CONTENT: return "content";
        case DIV: return "div";
        case VAR: return "var";
        case IMG: return "img";
        case SRC: return "src";
        case ALT: return "alt";
        case BODY: return "body";
        case REF: return "href";
        case H1: return "h1";
        case H2: return "h2";
        case H3: return "h3";
        case H4: return "h4";
        case A: return "a";
        case SPAN: return "span";
        case P: return "p";
        case TITLE: return "title";
        case COLOR: return "color";
        case BACKGROUND_COLOR: return "background-color";
        case STYLE: return "style";
        case ITERABLE: return "iterable";
        case ITERATE: return "iterate";
        case ITERATOR_REF: return "iterator-ref";
        case ITERATE_STRING: return "iterate-string";
        case UL: return "ul";
        case LI: return "li";
        case ID: return "id";
        case CLASS: return "class";
        case WIDTH: return "width";
        case HEIGHT: return "height";
        case MARGIN: return "margin";
        case MARGIN_TOP: return "margin-top";
        case MARGIN_RIGHT: return "margin-right";
        case MARGIN_BOTTOM: return "margin-bottom";
        case MARGIN_LEFT: return "margin-left";
        case PADDING: return "padding";
        case PADDING_TOP: return "padding-top";
        case PADDING_RIGHT: return "padding-right";
        case PADDING_BOTTOM: return "padding-bottom";
        case PADDING_LEFT: return "padding-left";
        case BORDER_WIDTH: return "border-width";
        case BORDER_STYLE: return "border-style";
        case BORDER_COLOR: return "border-color";
        case BORDER_RADIUS: return "border-radius";
        case FONT_SIZE: return "font-size";
        case FONT_FAMILY: return "font-family";
        case FONT_WEIGHT: return "font-weight";
        case DISPLAY: return "display";
        case POSITION: return "position";
        case TOP: return "top";
        case RIGHT: return "right";
        case BOTTOM: return "bottom";
        case LEFT: return "left";
        case OVERFLOW: return "overflow";
        case OPACITY: return "opacity";
        default: return NULL;
    }
}