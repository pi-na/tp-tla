#include "FlexActions.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;
static boolean _logIgnoredLexemes = true;
static char* _currentString = NULL;  // Para acumular contenido de strings

void initializeFlexActionsModule() {
	_logIgnoredLexemes = getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
	_logger = createLogger("FlexActions");
}

void shutdownFlexActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
	// Asegurarse de que _currentString se libere al finalizar
	if (_currentString != NULL) {
		free(_currentString);
		_currentString = NULL;
	}
}

/* PRIVATE FUNCTIONS */

static void _logLexicalAnalyzerContext(const char * functionName, LexicalAnalyzerContext * lexicalAnalyzerContext);

/**
 * Logs a lexical-analyzer context in DEBUGGING level.
 */
static void _logLexicalAnalyzerContext(const char * functionName, LexicalAnalyzerContext * lexicalAnalyzerContext) {
	char * escapedLexeme = escape(lexicalAnalyzerContext->lexeme);
	logDebugging(_logger, "%s: %s (context = %d, length = %d, line = %d)",
		functionName,
		escapedLexeme,
		lexicalAnalyzerContext->currentContext,
		lexicalAnalyzerContext->length,
		lexicalAnalyzerContext->line);
	free(escapedLexeme);
}

/* PUBLIC FUNCTIONS */

// ----- Comentarios ----- //

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	if (_logIgnoredLexemes) {
		_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	}
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

void EndMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	if (_logIgnoredLexemes) {
		_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	}
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	if (_logIgnoredLexemes) {
		_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	}
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

// ----- Cadenas ----- //

void BeginStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	// Inicializar buffer de string
	if (_currentString != NULL) {
		free(_currentString);
	}
	_currentString = calloc(1, sizeof(char)); // String vacío para empezar
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

int EndStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	// Asignar el string completo al valor semántico
	lexicalAnalyzerContext->semanticValue->string = strdup(_currentString);
	// printf("DEBUG: Completed string: '%s', returning STRING token\n", _currentString);
	free(_currentString);
	_currentString = NULL;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return STRING;
}

void StringContentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	// Concatenar al buffer actual
	size_t currentLength = strlen(_currentString);
	size_t lexemeLength = lexicalAnalyzerContext->length;
	_currentString = realloc(_currentString, currentLength + lexemeLength + 1);
	strcat(_currentString, lexicalAnalyzerContext->lexeme);
	// printf("DEBUG: String content added: '%s', current string: '%s', token will be: %d\n", 
    //        lexicalAnalyzerContext->lexeme, _currentString, STRING);
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

void EscapedCharacterStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	// Procesar carácter escapado (\n, \t, etc)
	char escapedChar;
	switch (lexicalAnalyzerContext->lexeme[1]) {
		case '"': escapedChar = '"'; break;
		case '\\': escapedChar = '\\'; break;
		case 'b': escapedChar = '\b'; break;
		case 'f': escapedChar = '\f'; break;
		case 'n': escapedChar = '\n'; break;
		case 'r': escapedChar = '\r'; break;
		case 't': escapedChar = '\t'; break;
		default: escapedChar = lexicalAnalyzerContext->lexeme[1];
	}
	
	// Agregar el carácter escapado al buffer
	size_t currentLength = strlen(_currentString);
	_currentString = realloc(_currentString, currentLength + 2);
	_currentString[currentLength] = escapedChar;
	_currentString[currentLength + 1] = '\0';
	
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

void UnicodeCharacterStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	// Simplificación: Solo registrar la secuencia unicode como texto normal
	// Una implementación completa requeriría convertir \uXXXX a su carácter UTF-8
	StringContentLexemeAction(lexicalAnalyzerContext);
}

void InvalidStringEscapeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	logError(_logger, "Invalid escape sequence in string: %s", lexicalAnalyzerContext->lexeme);
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

// ----- Símbolos ----- //

Token ParenthesisLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = token;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return token;
}

Token BraceLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = token;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return token;
}

Token BracketLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = token;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return token;
}

Token ColonLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return COLON;
}

Token CommaLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return COMMA;
}

Token SemiLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return SEMI;
}

// ----- Literales ----- //

Token NullLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return JSON_NULL;
}

// ----- KEYS ----- //

Token KeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = token;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return token;
}

// ----- VALUES ----- //

Token ValueLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = token;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return token;
}

// ----- Operadores ----- //

Token OperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = token;
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return token;
}

// ----- Identificadores ----- //

Token IdentifierLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = strdup(lexicalAnalyzerContext->lexeme);
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return IDENTIFIER;
}

// ----- Error ----- //

Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
	return UNKNOWN;
}
