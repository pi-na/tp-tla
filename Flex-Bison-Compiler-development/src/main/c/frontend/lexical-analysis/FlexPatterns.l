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

"type"                              { return KeywordLexemeAction(createLexicalAnalyzerContext(), TYPE); }
"content"                           { return KeywordLexemeAction(createLexicalAnalyzerContext(), CONTENT); }
"color"                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), COLOR); }
"style"                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), STYLE); }
"var"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), VAR); }
"background-color"                  { return KeywordLexemeAction(createLexicalAnalyzerContext(), BACKGROUND_COLOR); }
"div"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), DIV); }
"img"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), IMG); }
"src"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), SRC); }
"alt"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), ALT); }
"h1"                                { return KeywordLexemeAction(createLexicalAnalyzerContext(), H1); }
"h2"                                { return KeywordLexemeAction(createLexicalAnalyzerContext(), H2); }
"h3"                                { return KeywordLexemeAction(createLexicalAnalyzerContext(), H3); }
"h4"                                { return KeywordLexemeAction(createLexicalAnalyzerContext(), H4); }
"a"                                 { return KeywordLexemeAction(createLexicalAnalyzerContext(), A); }
"span"                              { return KeywordLexemeAction(createLexicalAnalyzerContext(), SPAN); }
"p"                                 { return KeywordLexemeAction(createLexicalAnalyzerContext(), P); }
"title"                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), TITLE); }
"body"                              { return KeywordLexemeAction(createLexicalAnalyzerContext(), BODY); }
"textalign"                         { return KeywordLexemeAction(createLexicalAnalyzerContext(), TEXTALIGN); }
"ref"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), REF); }

"("                                 { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"                                 { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }

"{"                                 { return BraceLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACE); }
"}"                                 { return BraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACE); }

"["                                 { return BracketLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
"]"                                 { return BracketLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }

":"                                 { return ColonLexemeAction(createLexicalAnalyzerContext()); }
","                                 { return CommaLexemeAction(createLexicalAnalyzerContext()); }
";"                                 { return SemiLexemeAction(createLexicalAnalyzerContext()); }

"null"                              { return NullLexemeAction(createLexicalAnalyzerContext()); }
"for"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), FOR); }

"$"                                 { return OperatorLexemeAction(createLexicalAnalyzerContext(), DOLLAR); }

{IDENTIFIER}                        { return IdentifierLexemeAction(createLexicalAnalyzerContext()); }

[[:space:]]+                        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.                                   { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"
