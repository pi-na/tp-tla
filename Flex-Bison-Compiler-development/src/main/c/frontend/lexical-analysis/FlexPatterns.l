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


/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%

"/*"								{ BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				{ EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+		{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"{"                                 { return OPEN_BRACE; }
"}"                                 { return CLOSE_BRACE; }
\[                                { return OPEN_BRACKET; }
\]                                { return CLOSE_BRACKET; }
":"                                 { return COLON; }
","                                 { return COMMA; }



"-"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), SUB); }
"*"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), MUL); }
"/"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), DIV); }
"+"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), ADD); }

"("									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }

[[:digit:]]+						{ return IntegerLexemeAction(createLexicalAnalyzerContext()); }

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

\"([^\"\\]|\\.)*\"         { return StringLexemeAction(createLexicalAnalyzerContext()); }
-?[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+)?   { return NumberLexemeAction(createLexicalAnalyzerContext()); }
true|false                 { return BooleanLexemeAction(createLexicalAnalyzerContext(), yytext[0] == 't' ? TRUE : FALSE); }
null                       { return NullLexemeAction(createLexicalAnalyzerContext()); }

"type"                     { return KeywordLexemeAction(createLexicalAnalyzerContext(), TYPE); }
"content"                  { return KeywordLexemeAction(createLexicalAnalyzerContext(), CONTENT); }
"style"                    { return KeywordLexemeAction(createLexicalAnalyzerContext(), STYLE); }
"src"                      { return KeywordLexemeAction(createLexicalAnalyzerContext(), SRC); }
"alt"                      { return KeywordLexemeAction(createLexicalAnalyzerContext(), ALT); }
"id"                       { return KeywordLexemeAction(createLexicalAnalyzerContext(), ID); }
"class"                    { return KeywordLexemeAction(createLexicalAnalyzerContext(), CLASS); }
"href"                     { return KeywordLexemeAction(createLexicalAnalyzerContext(), HREF); }
"ref"                      { return KeywordLexemeAction(createLexicalAnalyzerContext(), REF); }
"ext-ref"                  { return KeywordLexemeAction(createLexicalAnalyzerContext(), EXT_REF); }
"var"                      { return KeywordLexemeAction(createLexicalAnalyzerContext(), VAR); }
"if"                       { return KeywordLexemeAction(createLexicalAnalyzerContext(), IF); }
"then"                     { return KeywordLexemeAction(createLexicalAnalyzerContext(), THEN); }
"else"                     { return KeywordLexemeAction(createLexicalAnalyzerContext(), ELSE); }
"for"                      { return KeywordLexemeAction(createLexicalAnalyzerContext(), FOR); }
"while"                    { return KeywordLexemeAction(createLexicalAnalyzerContext(), WHILE); }

"&&"                       { return OperatorLexemeAction(createLexicalAnalyzerContext(), AND); }
\|\|                       { return OperatorLexemeAction(createLexicalAnalyzerContext(), OR); }
"!"                        { return OperatorLexemeAction(createLexicalAnalyzerContext(), NOT); }
"=="                       { return OperatorLexemeAction(createLexicalAnalyzerContext(), EQ); }
"!="                       { return OperatorLexemeAction(createLexicalAnalyzerContext(), NEQ); }
"<="                       { return OperatorLexemeAction(createLexicalAnalyzerContext(), LE); }
">="                       { return OperatorLexemeAction(createLexicalAnalyzerContext(), GE); }
"<"                        { return OperatorLexemeAction(createLexicalAnalyzerContext(), LT); }
">"                        { return OperatorLexemeAction(createLexicalAnalyzerContext(), GT); }

[a-zA-Z_][a-zA-Z0-9_]*     { return IdentifierLexemeAction(createLexicalAnalyzerContext()); }

%%