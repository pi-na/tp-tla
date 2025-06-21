#include <string.h>
#include "JsonProcessor.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

// Sistema de variables para el procesamiento
#define MAX_VARIABLES 100
static struct {
    char* name;
    ValueType type;
    union {
        char* stringValue;
        Object * objectValue;
        Array * arrayValue;
        VarRef * varRefValue;
        Token tokenValue;
    } value;
} processingVariables[MAX_VARIABLES];
static int processingVariableCount = 0;

void initializeJsonProcessorModule() {
    _logger = createLogger("JsonProcessor");
    processingVariableCount = 0;
}

void shutdownJsonProcessorModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
    // Limpiar variables
    for (int i = 0; i < processingVariableCount; i++) {
        if (processingVariables[i].name) {
            free(processingVariables[i].name);
        }
        if (processingVariables[i].type == STRING_VALUE && processingVariables[i].value.stringValue) {
            free(processingVariables[i].value.stringValue);
        }
    }
    processingVariableCount = 0;
}

/** PRIVATE FUNCTIONS */

static ProcessingResult _invalidProcessing();
static ProcessingResult _processValue(Value * value);
static ProcessingResult _processObject(Object * object);
static ProcessingResult _processArray(Array * array);
static ProcessingResult _processVarRef(VarRef * varRef);
static void _setVariable(const char* name, ValueType type, void* value);
static boolean _getVariable(const char* name, ValueType* type, void* value);

/**
 * Un procesamiento que siempre retorna un resultado inválido.
 */
static ProcessingResult _invalidProcessing() {
    ProcessingResult processingResult = {
        .succeed = false,
        .processedValue = NULL
    };
    return processingResult;
}

/**
 * Establece una variable en el contexto de procesamiento
 */
static void _setVariable(const char* name, ValueType type, void* value) {
    // Buscar si ya existe
    for (int i = 0; i < processingVariableCount; i++) {
        if (strcmp(processingVariables[i].name, name) == 0) {
            // Actualizar variable existente
            processingVariables[i].type = type;
            switch (type) {
                case STRING_VALUE:
                    if (processingVariables[i].value.stringValue) {
                        free(processingVariables[i].value.stringValue);
                    }
                    processingVariables[i].value.stringValue = strdup((char*)value);
                    break;
                case OBJECT_VALUE:
                    processingVariables[i].value.objectValue = (Object*)value;
                    break;
                case ARRAY_VALUE:
                    processingVariables[i].value.arrayValue = (Array*)value;
                    break;
                case VAR_REF_VALUE:
                    processingVariables[i].value.varRefValue = (VarRef*)value;
                    break;
                case TOKEN_VALUE:
                    processingVariables[i].value.tokenValue = *(Token*)value;
                    break;
                default:
                    break;
            }
            return;
        }
    }
    
    // Crear nueva variable
    if (processingVariableCount < MAX_VARIABLES) {
        processingVariables[processingVariableCount].name = strdup(name);
        processingVariables[processingVariableCount].type = type;
        switch (type) {
            case STRING_VALUE:
                processingVariables[processingVariableCount].value.stringValue = strdup((char*)value);
                break;
            case OBJECT_VALUE:
                processingVariables[processingVariableCount].value.objectValue = (Object*)value;
                break;
            case ARRAY_VALUE:
                processingVariables[processingVariableCount].value.arrayValue = (Array*)value;
                break;
            case VAR_REF_VALUE:
                processingVariables[processingVariableCount].value.varRefValue = (VarRef*)value;
                break;
            case TOKEN_VALUE:
                processingVariables[processingVariableCount].value.tokenValue = *(Token*)value;
                break;
            default:
                break;
        }
        processingVariableCount++;
    }
}

/**
 * Obtiene una variable del contexto de procesamiento
 */
static boolean _getVariable(const char* name, ValueType* type, void* value) {
    for (int i = 0; i < processingVariableCount; i++) {
        if (strcmp(processingVariables[i].name, name) == 0) {
            *type = processingVariables[i].type;
            switch (processingVariables[i].type) {
                case STRING_VALUE:
                    *(char**)value = strdup(processingVariables[i].value.stringValue);
                    break;
                case OBJECT_VALUE:
                    *(Object**)value = processingVariables[i].value.objectValue;
                    break;
                case ARRAY_VALUE:
                    *(Array**)value = processingVariables[i].value.arrayValue;
                    break;
                case VAR_REF_VALUE:
                    *(VarRef**)value = processingVariables[i].value.varRefValue;
                    break;
                case TOKEN_VALUE:
                    *(Token*)value = processingVariables[i].value.tokenValue;
                    break;
                default:
                    return false;
            }
            return true;
        }
    }
    return false;
}

/*
 * Expression evaluation functions commented out since Expression type is not defined
 * 
 * static boolean _evaluateExpression(Expression * expression) {
 *     // Implementation would go here
 *     return false;
 * }
 */

/**
 * Procesa una referencia a variable
 */
static ProcessingResult _processVarRef(VarRef * varRef) {
    if (!varRef || !varRef->name) {
        return _invalidProcessing();
    }
    
    ValueType type;
    union {
        char* stringVal;
        Object* objectVal;
        Array* arrayVal;
        VarRef* varRefVal;
        Token tokenVal;
    } val;
    
    if (_getVariable(varRef->name, &type, &val)) {
        Value * processedValue = calloc(1, sizeof(Value));
        processedValue->type = type;
        
        switch (type) {
            case STRING_VALUE:
                processedValue->data.stringValue = val.stringVal;
                break;
            case OBJECT_VALUE:
                processedValue->data.objectValue = val.objectVal;
                break;
            case ARRAY_VALUE:
                processedValue->data.arrayValue = val.arrayVal;
                break;
            case VAR_REF_VALUE:
                processedValue->data.varRefValue = val.varRefVal;
                break;
            case TOKEN_VALUE:
                processedValue->data.tokenValue = val.tokenVal;
                break;
            default:
                free(processedValue);
                return _invalidProcessing();
        }
        
        ProcessingResult result = {
            .succeed = true,
            .processedValue = processedValue
        };
        return result;
    }
    
    return _invalidProcessing();
}

/*
 * Conditional and Loop processing functions commented out since these types are not defined
 * 
 * static ProcessingResult _processConditional(Conditional * conditional) {
 *     // Implementation would go here
 *     return _invalidProcessing();
 * }
 * 
 * static ProcessingResult _processLoop(Loop * loop) {
 *     // Implementation would go here
 *     return _invalidProcessing();
 * }
 */

/**
 * Procesa un array
 */
static ProcessingResult _processArray(Array * array) {
    if (!array) {
        return _invalidProcessing();
    }
    
    Array * processedArray = calloc(1, sizeof(Array));
    ValueList * currentOriginal = array->values;
    ValueList * previousProcessed = NULL;
    
    while (currentOriginal) {
        ProcessingResult valueResult = _processValue(currentOriginal->value);
        if (valueResult.succeed) {
            ValueList * newValueList = calloc(1, sizeof(ValueList));
            newValueList->value = valueResult.processedValue;
            newValueList->next = NULL;
            
            if (previousProcessed) {
                previousProcessed->next = newValueList;
            } else {
                processedArray->values = newValueList;
            }
            previousProcessed = newValueList;
        }
        currentOriginal = currentOriginal->next;
    }
    
    Value * processedValue = calloc(1, sizeof(Value));
    processedValue->type = ARRAY_VALUE;
    processedValue->data.arrayValue = processedArray;
    
    ProcessingResult result = {
        .succeed = true,
        .processedValue = processedValue
    };
    return result;
}

/**
 * Procesa un objeto
 */
static ProcessingResult _processObject(Object * object) {
    if (!object) {
        return _invalidProcessing();
    }
    
    Object * processedObject = calloc(1, sizeof(Object));
    PairList * currentOriginal = object->pairs;
    PairList * previousProcessed = NULL;
    
    while (currentOriginal) {
        if (currentOriginal->pair && currentOriginal->pair->value) {
            ProcessingResult valueResult = _processValue(currentOriginal->pair->value);
            if (valueResult.succeed) {
                PairList * newPairList = calloc(1, sizeof(PairList));
                newPairList->pair = calloc(1, sizeof(Pair));
                newPairList->pair->key = currentOriginal->pair->key; // Reutilizar la clave
                newPairList->pair->value = valueResult.processedValue;
                newPairList->next = NULL;
                
                if (previousProcessed) {
                    previousProcessed->next = newPairList;
                } else {
                    processedObject->pairs = newPairList;
                }
                previousProcessed = newPairList;
            }
        }
        currentOriginal = currentOriginal->next;
    }
    
    Value * processedValue = calloc(1, sizeof(Value));
    processedValue->type = OBJECT_VALUE;
    processedValue->data.objectValue = processedObject;
    
    ProcessingResult result = {
        .succeed = true,
        .processedValue = processedValue
    };
    return result;
}

/**
 * Procesa un valor
 */
static ProcessingResult _processValue(Value * value) {
    if (!value) {
        return _invalidProcessing();
    }
    
    switch (value->type) {
        case STRING_VALUE:
        case NULL_VALUE:
        case TOKEN_VALUE: {
            // Valores primitivos se copian directamente
            Value * processedValue = calloc(1, sizeof(Value));
            *processedValue = *value;
            if (value->type == STRING_VALUE && value->data.stringValue) {
                processedValue->data.stringValue = strdup(value->data.stringValue);
            }
            ProcessingResult result = {
                .succeed = true,
                .processedValue = processedValue
            };
            return result;
        }
        case OBJECT_VALUE:
            return _processObject(value->data.objectValue);
        case ARRAY_VALUE:
            return _processArray(value->data.arrayValue);
        case VAR_REF_VALUE:
            return _processVarRef(value->data.varRefValue);
        default:
            return _invalidProcessing();
    }
}

/** PUBLIC FUNCTIONS */

ProcessingResult processProgram(Program * program) {
    if (!program || !program->object) {
        return _invalidProcessing();
    }
    
    logDebugging(_logger, "Processing JSON program...");
    return _processObject(program->object);
}
