#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../shared/Environment.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../../shared/Type.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
#include "LexicalAnalyzerContext.h"
#include "Tokens.h"
#include <stdio.h>
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeFlexActionsModule();

/** Shutdown module's internal state. */
void shutdownFlexActionsModule();

/**
 * Flex lexeme processing actions.
 */

/* Acciones para comentarios */
void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void EndMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token ParenthesisLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token CommaLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

/* Acciones para palabras clave */
Token KeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token HTMLTagLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

/* Acciones para identificadores */
Token IdentifierLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

/* Acción para tokens desconocidos */
Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token BraceLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

#endif
