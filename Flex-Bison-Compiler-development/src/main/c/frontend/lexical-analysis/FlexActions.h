#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../shared/Type.h"
#include "../../shared/CompilerState.h"
#include "../../shared/Environment.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
#include "LexicalAnalyzerContext.h"
#include <stdio.h>
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeFlexActionsModule();

/** Shutdown module's internal state. */
void shutdownFlexActionsModule();

/**
 * Flex lexeme processing actions.
 */

// Comentarios
void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void EndMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

// Cadenas
void BeginStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
int EndStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void StringContentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void EscapedCharacterStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void UnicodeCharacterStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void InvalidStringEscapeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

// Símbolos
Token ParenthesisLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token BraceLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token BracketLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token ColonLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token CommaLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token SemiLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

// Literales
Token NullLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

// Palabras clave
Token KeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token OperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

// Identificadores
Token IdentifierLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

// Error
Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

#endif
