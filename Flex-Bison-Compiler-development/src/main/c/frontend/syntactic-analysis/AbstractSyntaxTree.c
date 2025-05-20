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

void releaseEntryList(EntryList * entryList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (entryList == NULL) {
		return;
	}
	
	// Liberar todos los nodos de la lista enlazada recursivamente
	if (entryList->next != NULL) {
		releaseEntryList(entryList->next);
	}
	
	// Liberar la entrada del nodo actual
	if (entryList->entry != NULL) {
		releaseEntry(entryList->entry);
	}
	
	// Liberar el nodo actual
	free(entryList);
}

void releaseEntry(Entry * entry) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (entry != NULL) {
		free(entry);
	}
}

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
        free(pair->key);
    }
    
    // Liberar el valor
    if (pair->value != NULL) {
        releaseValue(pair->value);
    }
    
    // Liberar el par
    free(pair);
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
        case CONDITIONAL_VALUE:
            if (value->data.conditionalValue != NULL) {
                releaseConditional(value->data.conditionalValue);
            }
            break;
        case LOOP_VALUE:
            if (value->data.loopValue != NULL) {
                releaseLoop(value->data.loopValue);
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

void releaseConditional(Conditional * conditional) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (conditional == NULL) {
        return;
    }
    
    // Liberar la condición
    if (conditional->condition != NULL) {
        releaseExpression(conditional->condition);
    }
    
    // Liberar la rama then
    if (conditional->thenBranch != NULL) {
        releaseObject(conditional->thenBranch);
    }
    
    // Liberar la rama else (si existe)
    if (conditional->elseBranch != NULL) {
        releaseObject(conditional->elseBranch);
    }
    
    // Liberar el condicional
    free(conditional);
}

void releaseLoop(Loop * loop) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (loop == NULL) {
        return;
    }
    
    // Liberar la inicialización
    if (loop->initialization != NULL) {
        releaseExpression(loop->initialization);
    }
    
    // Liberar la condición
    if (loop->condition != NULL) {
        releaseExpression(loop->condition);
    }
    
    // Liberar el incremento
    if (loop->increment != NULL) {
        releaseExpression(loop->increment);
    }
    
    // Liberar el cuerpo
    if (loop->body != NULL) {
        releaseObject(loop->body);
    }
    
    // Liberar el bucle
    free(loop);
}

void releaseExpression(Expression * expression) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (expression == NULL) {
        return;
    }
    
    // Liberar el contenido según el tipo
    switch (expression->type) {
        case STRING_EXPR:
            if (expression->data.stringValue != NULL) {
                free(expression->data.stringValue);
            }
            break;
        case VAR_REF_EXPR:
            if (expression->data.varRefValue != NULL) {
                releaseVarRef(expression->data.varRefValue);
            }
            break;
        case BINARY_EXPR:
            if (expression->data.binaryExpr.left != NULL) {
                releaseExpression(expression->data.binaryExpr.left);
            }
            if (expression->data.binaryExpr.right != NULL) {
                releaseExpression(expression->data.binaryExpr.right);
            }
            break;
        case UNARY_EXPR:
            if (expression->data.unaryExpr.operand != NULL) {
                releaseExpression(expression->data.unaryExpr.operand);
            }
            break;
        default:
            // Para INTEGER_EXPR, FLOAT_EXPR y BOOLEAN_EXPR no hay nada que liberar
            break;
    }
    
    // Liberar la expresión
    free(expression);
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

void releaseAttributeList(AttributeList * attributeList) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (attributeList == NULL) {
        return;
    }
    
    // Liberar el siguiente nodo en la lista
    if (attributeList->next != NULL) {
        releaseAttributeList(attributeList->next);
    }
    
    // Liberar el atributo actual
    if (attributeList->attribute != NULL) {
        releaseAttribute(attributeList->attribute);
    }
    
    // Liberar el nodo actual
    free(attributeList);
}

void releaseAttribute(Attribute * attribute) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (attribute == NULL) {
        return;
    }
    
    // Liberar el nombre del atributo
    if (attribute->name != NULL) {
        free(attribute->name);
    }
    
    // Liberar el valor del atributo
    if (attribute->value != NULL) {
        releaseValue(attribute->value);
    }
    
    // Liberar el atributo
    free(attribute);
}

void releaseElement(Element * element) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (element == NULL) {
        return;
    }
    
    // Liberar el nombre de la etiqueta
    if (element->tag != NULL) {
        free(element->tag);
    }
    
    // Liberar la lista de atributos
    if (element->attributes != NULL) {
        releaseAttributeList(element->attributes);
    }
    
    // Liberar la lista de hijos
    if (element->children != NULL) {
        releaseValueList(element->children);
    }
    
    // Liberar el elemento
    free(element);
}
