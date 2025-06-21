#include <string.h>
#include "JsonPocessor.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

// Sistema de variables para el procesamiento
#define MAX_VARIABLES 100
static struct {
    char* name;
    ValueType type;
    union {
        int intValue;
        double floatValue;
        char* stringValue;
        boolean booleanValue;
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
static ProcessingResult _processConditional(Conditional * conditional);
static ProcessingResult _processLoop(Loop * loop);
static ProcessingResult _processVarRef(VarRef * varRef);
static ProcessingResult _processExpression(Expression * expression);
static boolean _evaluateExpression(Expression * expression);
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
                case INTEGER_VALUE:
                    processingVariables[i].value.intValue = *(int*)value;
                    break;
                case FLOAT_VALUE:
                    processingVariables[i].value.floatValue = *(double*)value;
                    break;
                case STRING_VALUE:
                    if (processingVariables[i].value.stringValue) {
                        free(processingVariables[i].value.stringValue);
                    }
                    processingVariables[i].value.stringValue = strdup((char*)value);
                    break;
                case BOOLEAN_VALUE:
                    processingVariables[i].value.booleanValue = *(boolean*)value;
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
            case INTEGER_VALUE:
                processingVariables[processingVariableCount].value.intValue = *(int*)value;
                break;
            case FLOAT_VALUE:
                processingVariables[processingVariableCount].value.floatValue = *(double*)value;
                break;
            case STRING_VALUE:
                processingVariables[processingVariableCount].value.stringValue = strdup((char*)value);
                break;
            case BOOLEAN_VALUE:
                processingVariables[processingVariableCount].value.booleanValue = *(boolean*)value;
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
                case INTEGER_VALUE:
                    *(int*)value = processingVariables[i].value.intValue;
                    break;
                case FLOAT_VALUE:
                    *(double*)value = processingVariables[i].value.floatValue;
                    break;
                case STRING_VALUE:
                    *(char**)value = strdup(processingVariables[i].value.stringValue);
                    break;
                case BOOLEAN_VALUE:
                    *(boolean*)value = processingVariables[i].value.booleanValue;
                    break;
                default:
                    return false;
            }
            return true;
        }
    }
    return false;
}

/**
 * Evalúa una expresión y retorna su valor booleano
 */
static boolean _evaluateExpression(Expression * expression) {
    if (!expression) return false;
    
    switch (expression->type) {
        case BOOLEAN_EXPR:
            return expression->data.booleanValue;
        case INTEGER_EXPR:
            return expression->data.integerValue != 0;
        case FLOAT_EXPR:
            return expression->data.floatValue != 0.0;
        case STRING_EXPR:
            return expression->data.stringValue != NULL && strlen(expression->data.stringValue) > 0;
        case VAR_REF_EXPR: {
            ValueType type;
            union {
                int intVal;
                double floatVal;
                char* stringVal;
                boolean boolVal;
            } val;
            if (_getVariable(expression->data.varRefValue->name, &type, &val)) {
                switch (type) {
                    case BOOLEAN_VALUE:
                        return val.boolVal;
                    case INTEGER_VALUE:
                        return val.intVal != 0;
                    case FLOAT_VALUE:
                        return val.floatVal != 0.0;
                    case STRING_VALUE:
                        boolean result = val.stringVal != NULL && strlen(val.stringVal) > 0;
                        free(val.stringVal);
                        return result;
                    default:
                        return false;
                }
            }
            return false;
        }
        case BINARY_EXPR: {
            boolean left = _evaluateExpression(expression->data.binaryExpr.left);
            boolean right = _evaluateExpression(expression->data.binaryExpr.right);
            switch (expression->data.binaryExpr.operator) {
                case AND_OP:
                    return left && right;
                case OR_OP:
                    return left || right;
                case EQUALS_OP:
                    return left == right;
                case NOT_EQUALS_OP:
                    return left != right;
                default:
                    return false;
            }
        }
        case UNARY_EXPR:
            return !_evaluateExpression(expression->data.unaryExpr.operand);
        default:
            return false;
    }
}

/**
 * Procesa una referencia a variable
 */
static ProcessingResult _processVarRef(VarRef * varRef) {
    if (!varRef || !varRef->name) {
        return _invalidProcessing();
    }
    
    ValueType type;
    union {
        int intVal;
        double floatVal;
        char* stringVal;
        boolean boolVal;
    } val;
    
    if (_getVariable(varRef->name, &type, &val)) {
        Value * processedValue = calloc(1, sizeof(Value));
        processedValue->type = type;
        
        switch (type) {
            case INTEGER_VALUE:
                processedValue->data.integerValue = val.intVal;
                break;
            case FLOAT_VALUE:
                processedValue->data.floatValue = val.floatVal;
                break;
            case STRING_VALUE:
                processedValue->data.stringValue = val.stringVal;
                break;
            case BOOLEAN_VALUE:
                processedValue->data.booleanValue = val.boolVal;
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

/**
 * Procesa un condicional
 */
static ProcessingResult _processConditional(Conditional * conditional) {
    if (!conditional || !conditional->condition) {
        return _invalidProcessing();
    }
    
    boolean conditionResult = _evaluateExpression(conditional->condition);
    
    if (conditionResult && conditional->thenObject) {
        return _processObject(conditional->thenObject);
    } else if (!conditionResult && conditional->elseObject) {
        return _processObject(conditional->elseObject);
    }
    
    // Si no se cumple ninguna condición, retornar un objeto vacío
    Value * emptyValue = calloc(1, sizeof(Value));
    emptyValue->type = OBJECT_VALUE;
    emptyValue->data.objectValue = calloc(1, sizeof(Object));
    emptyValue->data.objectValue->pairs = NULL;
    
    ProcessingResult result = {
        .succeed = true,
        .processedValue = emptyValue
    };
    return result;
}

/**
 * Procesa un loop
 */
static ProcessingResult _processLoop(Loop * loop) {
    if (!loop || !loop->condition || !loop->body) {
        return _invalidProcessing();
    }
    
    // Para simplificar, procesamos el loop una vez
    // En una implementación más completa, se ejecutaría múltiples veces
    boolean conditionResult = _evaluateExpression(loop->condition);
    
    if (conditionResult) {
        return _processObject(loop->body);
    }
    
    // Si no se cumple la condición, retornar un objeto vacío
    Value * emptyValue = calloc(1, sizeof(Value));
    emptyValue->type = OBJECT_VALUE;
    emptyValue->data.objectValue = calloc(1, sizeof(Object));
    emptyValue->data.objectValue->pairs = NULL;
    
    ProcessingResult result = {
        .succeed = true,
        .processedValue = emptyValue
    };
    return result;
}

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
        case INTEGER_VALUE:
        case FLOAT_VALUE:
        case BOOLEAN_VALUE:
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
        case CONDITIONAL_VALUE:
            return _processConditional(value->data.conditionalValue);
        case LOOP_VALUE:
            return _processLoop(value->data.loopValue);
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
