#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "frontend/syntactic-analysis/SyntacticAnalyzer.h"
#include "backend/domain-specific/JsonProcessor.h"
#include "backend/code-generation/HtmlGenerator.h"
#include "shared/CompilerState.h"
#include "shared/Logger.h"
#include "shared/String.h"
#include "shared/symbolTable.h"

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
	initializeSymbolTableModule();
	SymbolTable * symbolTable = createSymbolTable();

	if (count < 2) {
		logError(logger, "Uso: %s <ruta_lista_de_simbolos> <ruta_archivo_de_salida>", arguments[0]);
	}

	const char * symbolsPath = arguments[1];
	const char * outputPath = arguments[2];

	initializeHtmlGeneratorModule(outputPath);

	if(!symbolTableLoadFromFile(symbolTable, symbolsPath)){
		logError(logger, "Error cargando símbolos desde '%s'", symbolsPath);
	}

	// Logs the arguments of the application.
	for (int k = 0; k < count; ++k) {
		logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
	}

	// Begin compilation process.
	CompilerState compilerState = {
		.abstractSyntaxtTree = NULL,
		.succeed = false,
		.value = 0,
		.symbolTable = symbolTable
	};
	const SyntacticAnalysisStatus syntacticAnalysisStatus = parse(&compilerState);
	CompilationStatus compilationStatus = SUCCEED;
	Program * program = compilerState.abstractSyntaxtTree;

	if (syntacticAnalysisStatus == ACCEPT) {
		logDebugging(logger, "Generating code...");
		generateHtml(&compilerState);
	}
	else {
		logError(logger, "The syntactic-analysis phase rejects the input program.");
		compilationStatus = FAILED;
	}

	logDebugging(logger, "Releasing AST resources...");
	releaseProgram(program);
	logDebugging(logger, "Releasing modules resources...");
	shutdownHtmlGeneratorModule();
	shutdownAbstractSyntaxTreeModule();
	shutdownSyntacticAnalyzerModule();
	shutdownBisonActionsModule();
	shutdownFlexActionsModule();
	shutdownSymbolTableModule();
	destroySymbolTable(compilerState.symbolTable);
	logDebugging(logger, "Compilation is done.");
	destroyLogger(logger);
	return compilationStatus;
}