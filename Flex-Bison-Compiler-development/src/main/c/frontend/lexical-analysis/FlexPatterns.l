%{

#include "FlexActions.h"

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x MULTILINE_COMMENT
%x STRING

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
DIGIT           [0-9]
ALPHA           [a-zA-Z]
IDENTIFIER      {ALPHA}[a-zA-Z0-9_]*
ESCAPED         \\["\\bfnrt]
UNICODE         \\u[0-9a-fA-F]{4}

%%

"/*"                                { BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"             { EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+     { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

\"                                  { BEGIN(STRING); BeginStringLexemeAction(createLexicalAnalyzerContext()); }
<STRING>\"                          { int token = EndStringLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); return token; }
<STRING>{ESCAPED}                   { EscapedCharacterStringLexemeAction(createLexicalAnalyzerContext()); }
<STRING>{UNICODE}                   { UnicodeCharacterStringLexemeAction(createLexicalAnalyzerContext()); }
<STRING>[^\\"]+                     { StringContentLexemeAction(createLexicalAnalyzerContext()); }
<STRING>\\                          { InvalidStringEscapeLexemeAction(createLexicalAnalyzerContext()); }
<STRING>.                           { StringContentLexemeAction(createLexicalAnalyzerContext()); }

"("                                 { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"                                 { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }

"{"                                 { return BraceLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACE); }
"}"                                 { return BraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACE); }

"["                                 { return BracketLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
"]"                                 { return BracketLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }

":"                                 { return ColonLexemeAction(createLexicalAnalyzerContext()); }
","                                 { return CommaLexemeAction(createLexicalAnalyzerContext()); }
";"                                 { return SemiLexemeAction(createLexicalAnalyzerContext()); }

"true"                              { return BooleanLexemeAction(createLexicalAnalyzerContext(), TRUE); }
"false"                             { return BooleanLexemeAction(createLexicalAnalyzerContext(), FALSE); }
"null"                              { return NullLexemeAction(createLexicalAnalyzerContext()); }

"if"                                { return KeywordLexemeAction(createLexicalAnalyzerContext(), IF); }
"else"                              { return KeywordLexemeAction(createLexicalAnalyzerContext(), ELSE); }
"for"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), FOR); }

"=="                                { return OperatorLexemeAction(createLexicalAnalyzerContext(), EQUALS); }
"!="                                { return OperatorLexemeAction(createLexicalAnalyzerContext(), NOT_EQUALS); }
">"                                 { return OperatorLexemeAction(createLexicalAnalyzerContext(), GREATER_THAN); }
"<"                                 { return OperatorLexemeAction(createLexicalAnalyzerContext(), LESS_THAN); }
">="                                { return OperatorLexemeAction(createLexicalAnalyzerContext(), GREATER_EQUAL); }
"<="                                { return OperatorLexemeAction(createLexicalAnalyzerContext(), LESS_EQUAL); }
"&&"                                { return OperatorLexemeAction(createLexicalAnalyzerContext(), AND); }
"||"                                { return OperatorLexemeAction(createLexicalAnalyzerContext(), OR); }
"!"                                 { return OperatorLexemeAction(createLexicalAnalyzerContext(), NOT); }

"+"                                 { return OperatorLexemeAction(createLexicalAnalyzerContext(), ADD); }
"-"                                 { return OperatorLexemeAction(createLexicalAnalyzerContext(), SUB); }
"*"                                 { return OperatorLexemeAction(createLexicalAnalyzerContext(), MUL); }
"/"                                 { return OperatorLexemeAction(createLexicalAnalyzerContext(), DIV); }
"="                                 { return OperatorLexemeAction(createLexicalAnalyzerContext(), ASSIGN); }
"$"                                 { return OperatorLexemeAction(createLexicalAnalyzerContext(), DOLLAR); }

-?{DIGIT}+                          { return IntegerLexemeAction(createLexicalAnalyzerContext()); }
-?{DIGIT}+\.{DIGIT}+                { return FloatLexemeAction(createLexicalAnalyzerContext()); }

{IDENTIFIER}                        { return IdentifierLexemeAction(createLexicalAnalyzerContext()); }

[[:space:]]+                        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.                                   { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"
