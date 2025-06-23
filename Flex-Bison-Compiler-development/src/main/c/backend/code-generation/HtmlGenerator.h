#ifndef HTML_GENERATOR_H_INCLUDED
#define HTML_GENERATOR_H_INCLUDED


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>    // tolower
#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"

void initializeHtmlGeneratorModule();
void shutdownHtmlGeneratorModule();
void generateHtml(CompilerState * compilerState);
void setHtmlOutputFile(const char *filePath);

#endif
