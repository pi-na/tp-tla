#ifndef JSON_PROCESSOR_H_INCLUDED
#define JSON_PROCESSOR_H_INCLUDED

#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"

/**
 * Resultado del procesamiento de JSON
 */
typedef struct ProcessingResult {
    boolean succeed;
    Value * processedValue;
} ProcessingResult;

void initializeJsonProcessorModule();
void shutdownJsonProcessorModule();

ProcessingResult processProgram(Program * program);

#endif
