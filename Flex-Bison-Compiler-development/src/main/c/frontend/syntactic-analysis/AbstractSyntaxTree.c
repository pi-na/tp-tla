#include "AbstractSyntaxTree.h"

static Logger *_logger = NULL;

void initializeAbstractSyntaxTreeModule() {
    _logger = createLogger("AbstractSyntaxTree");
}

void shutdownAbstractSyntaxTreeModule() {
    if (_logger) destroyLogger(_logger);
}

void releaseProgram(Program *program) {
    logDebugging(_logger, "Destructor: releaseProgram");
    if (!program) return;
    releaseObject(program->object);
    free(program);
}

void releaseObject(Object *object) {
    logDebugging(_logger, "Destructor: releaseObject");
    if (!object) return;
    releasePairList(object->pairs);
    free(object);
}

void releasePairList(PairList *pairList) {
    logDebugging(_logger, "Destructor: releasePairList");
    if (!pairList) return;
    releasePairList(pairList->next);
    releasePair(pairList->pair);
    free(pairList);
}

void releasePair(Pair *pair) {
    logDebugging(_logger, "Destructor: releasePair");
    if (!pair) return;
    releaseKeyword(pair->key);
    releaseValue(pair->value);
    free(pair);
}

void releaseKeyword(Keyword *keyword) {
    logDebugging(_logger, "Destructor: releaseKeyword");
    if (!keyword) return;
    free(keyword);
}

void releaseValue(Value *value) {
    logDebugging(_logger, "Destructor: releaseValue");
    if (!value) return;
    switch (value->type) {
        case STRING_VALUE:
            free(value->data.stringValue);
            break;
        case OBJECT_VALUE:
            releaseObject(value->data.objectValue);
            break;
        case ARRAY_VALUE:
            releaseArray(value->data.arrayValue);
            break;
        case VAR_REF_VALUE:
            releaseVarRef(value->data.varRefValue);
            break;
        default:
            break;
    }
    free(value);
}

void releaseArray(Array *array) {
    logDebugging(_logger, "Destructor: releaseArray");
    if (!array) return;
    releaseValueList(array->values);
    free(array);
}

void releaseValueList(ValueList *valueList) {
    logDebugging(_logger, "Destructor: releaseValueList");
    if (!valueList) return;
    releaseValueList(valueList->next);
    releaseValue(valueList->value);
    free(valueList);
}

void releaseVarRef(VarRef *varRef) {
    logDebugging(_logger, "Destructor: releaseVarRef");
    if (!varRef) return;
    free(varRef->name);
    free(varRef);
}
