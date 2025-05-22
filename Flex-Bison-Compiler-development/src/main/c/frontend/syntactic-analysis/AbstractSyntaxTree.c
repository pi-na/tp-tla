#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntxTree");
}

void shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PUBLIC FUNCTIONS */

void releaseObject(Object * object) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (object != NULL) {
		releasePairList(object->pairs);
		free(object);
	}
}

void releaseProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		releaseObject(program->object);
		free(program);
	}
}

// Implementación de los nuevos destructores

void releasePairList(PairList * pairList) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (pairList == NULL) {
        return;
    }
    
    // Liberar siguiente nodo en la lista
    if (pairList->next != NULL) {
        releasePairList(pairList->next);
    }
    
    // Liberar el par actual
    if (pairList->pair != NULL) {
        releasePair(pairList->pair);
    }
    
    // Liberar el nodo actual
    free(pairList);
}

void releasePair(Pair * pair) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (pair == NULL) {
        return;
    }
    
    // Liberar la clave
    if (pair->key != NULL) {
        releaseKeyword(pair->key);
    }
    
    // Liberar el valor
    if (pair->value != NULL) {
        releaseValue(pair->value);
    }
    
    // Liberar el par
    free(pair);
}

void releaseKeyword(Keyword * keyword) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (keyword == NULL) {
        return;
    }
    free(keyword);
}

void releaseValue(Value * value) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (value == NULL) {
        return;
    }
    
    // Liberar el contenido según el tipo
    switch (value->type) {
        case STRING_VALUE:
            if (value->data.stringValue != NULL) {
                free(value->data.stringValue);
            }
            break;
        case OBJECT_VALUE:
            if (value->data.objectValue != NULL) {
                releaseObject(value->data.objectValue);
            }
            break;
        case ARRAY_VALUE:
            if (value->data.arrayValue != NULL) {
                releaseArray(value->data.arrayValue);
            }
            break;
        case VAR_REF_VALUE:
            if (value->data.varRefValue != NULL) {
                releaseVarRef(value->data.varRefValue);
            }
            break;
        default:
            // Para INTEGER_VALUE, FLOAT_VALUE, BOOLEAN_VALUE y NULL_VALUE no hay nada que liberar
            break;
    }
    
    // Liberar el valor
    free(value);
}

void releaseArray(Array * array) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (array == NULL) {
        return;
    }
    
    // Liberar la lista de valores
    if (array->values != NULL) {
        releaseValueList(array->values);
    }
    
    // Liberar el array
    free(array);
}

void releaseValueList(ValueList * valueList) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (valueList == NULL) {
        return;
    }
    
    // Liberar el siguiente nodo en la lista
    if (valueList->next != NULL) {
        releaseValueList(valueList->next);
    }
    
    // Liberar el valor actual
    if (valueList->value != NULL) {
        releaseValue(valueList->value);
    }
    
    // Liberar el nodo actual
    free(valueList);
}

void releaseVarRef(VarRef * varRef) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (varRef == NULL) {
        return;
    }
    
    // Liberar el nombre de la variable
    if (varRef->name != NULL) {
        free(varRef->name);
    }
    
    // Liberar la referencia a variable
    free(varRef);
}

