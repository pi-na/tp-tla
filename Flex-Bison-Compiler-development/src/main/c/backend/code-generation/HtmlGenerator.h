#ifndef HTML_GENERATOR_H_INCLUDED
#define HTML_GENERATOR_H_INCLUDED

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../domain-specific/JsonPocessor.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initializeHtmlGeneratorModule();
void shutdownHtmlGeneratorModule();

void generateHtml(CompilerState * compilerState, ProcessingResult * processingResult);

#endif
