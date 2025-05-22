// #include "backend/code-generation/Generator.h"
// #include "backend/domain-specific/Calculator.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "frontend/syntactic-analysis/SyntacticAnalyzer.h"
#include "shared/CompilerState.h"
#include "shared/Environment.h"
#include "shared/Logger.h"
#include "shared/String.h"
#include <stdio.h>
#include <string.h>

// Forward declaration of helper functions
void dumpObject(Object* obj, int indent);
void dumpValue(Value* value, int indent);

/**
 * The main entry-point of the entire application. If you use "strtok" to
 * parse anything inside this project instead of using Flex and Bison, I will
 * find you, and I will kill you (Bryan Mills; "Taken", 2008).
 */
const int main(const int count, const char ** arguments) {
	Logger * logger = createLogger("EntryPoint");
	initializeFlexActionsModule();
	initializeBisonActionsModule();
	initializeSyntacticAnalyzerModule();
	initializeAbstractSyntaxTreeModule();
	//initializeCalculatorModule();
	//initializeGeneratorModule();

	// Logs the arguments of the application.
	for (int k = 0; k < count; ++k) {
		logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
	}

	// Begin compilation process.
	CompilerState compilerState = {
		.abstractSyntaxtTree = NULL,
		.succeed = false,
		.value = 0
	};
	const SyntacticAnalysisStatus syntacticAnalysisStatus = parse(&compilerState);
	CompilationStatus compilationStatus = SUCCEED;
	Program * program = compilerState.abstractSyntaxtTree;
	if (syntacticAnalysisStatus == ACCEPT) {
		// Print success message
		printf("COMPILATION SUCCESSFUL!\n");
		
		// Debug: dump the AST if in debug mode
		#ifdef DEBUG
		if (program != NULL && program->object != NULL) {
			printf("AST DUMP:\n");
			dumpObject(program->object, 0);
		}
		#endif
	}
	else {
		logError(logger, "The syntactic-analysis phase rejects the input program.");
		printf("COMPILATION FAILED: Syntactic analysis rejected the input\n");
		compilationStatus = FAILED;
	}
	logDebugging(logger, "Releasing AST resources...");
	releaseProgram(program);
	logDebugging(logger, "Releasing modules resources...");
	//shutdownGeneratorModule();
	//shutdownCalculatorModule();
	shutdownAbstractSyntaxTreeModule();
	shutdownSyntacticAnalyzerModule();
	shutdownBisonActionsModule();
	shutdownFlexActionsModule();
	logDebugging(logger, "Compilation is done.");
	destroyLogger(logger);
	return compilationStatus;
}

// Helper function to print indentation
void printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

// Dump the content of an object (JSON-like)
void dumpObject(Object* obj, int indent) {
    if (obj == NULL) {
        printIndent(indent);
        printf("NULL\n");
        return;
    }
    
    printIndent(indent);
    printf("{\n");
    
    PairList* current = obj->pairs;
    while (current != NULL) {
        printIndent(indent + 1);
        printf("\"%s\": ", current->pair->key);
        dumpValue(current->pair->value, indent + 1);
        current = current->next;
    }
    
    printIndent(indent);
    printf("}\n");
}

// Dump a value
void dumpValue(Value* value, int indent) {
    if (value == NULL) {
        printf("NULL\n");
        return;
    }
    
    switch (value->type) {
        case STRING_VALUE:
            printf("\"%s\"\n", value->data.stringValue);
            break;
        case NULL_VALUE:
            printf("null\n");
            break;
        case OBJECT_VALUE:
            printf("\n");
            dumpObject(value->data.objectValue, indent + 1);
            break;
        case ARRAY_VALUE:
            printf("[\n");
            ValueList* valueList = value->data.arrayValue->values;
            while (valueList != NULL) {
                printIndent(indent + 1);
                dumpValue(valueList->value, indent + 1);
                valueList = valueList->next;
            }
            printIndent(indent);
            printf("]\n");
            break;
        default:
            printf("Unknown value type: %d\n", value->type);
            break;
    }
}
