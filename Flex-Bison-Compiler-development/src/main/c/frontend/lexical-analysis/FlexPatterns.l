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

\"type\"                { return KeywordLexemeAction(createLexicalAnalyzerContext(), TYPE); }
\"content\"             { return KeywordLexemeAction(createLexicalAnalyzerContext(), CONTENT); }
\"loop\"                { return KeywordLexemeAction(createLexicalAnalyzerContext(), LOOP); }
\"iterable\"            { return KeywordLexemeAction(createLexicalAnalyzerContext(), ITERABLE); }
\"iterate\"             { return KeywordLexemeAction(createLexicalAnalyzerContext(), ITERATE); }
\"style\"               { return KeywordLexemeAction(createLexicalAnalyzerContext(), STYLE); }
"\"width\""             { return KeywordLexemeAction(createLexicalAnalyzerContext(), WIDTH); }
"\"height\""            { return KeywordLexemeAction(createLexicalAnalyzerContext(), HEIGHT); }
\"background-color\"    { return KeywordLexemeAction(createLexicalAnalyzerContext(), BACKGROUND_COLOR); }
"\"margin\""            { return KeywordLexemeAction(createLexicalAnalyzerContext(), MARGIN); }
"\"margin-top\""        { return KeywordLexemeAction(createLexicalAnalyzerContext(), MARGIN_TOP); }
"\"margin-right\""      { return KeywordLexemeAction(createLexicalAnalyzerContext(), MARGIN_RIGHT); }
"\"margin-bottom\""     { return KeywordLexemeAction(createLexicalAnalyzerContext(), MARGIN_BOTTOM); }
"\"margin-left\""       { return KeywordLexemeAction(createLexicalAnalyzerContext(), MARGIN_LEFT); }
"\"padding\""           { return KeywordLexemeAction(createLexicalAnalyzerContext(), PADDING); }
"\"padding-top\""       { return KeywordLexemeAction(createLexicalAnalyzerContext(), PADDING_TOP); }
"\"padding-right\""     { return KeywordLexemeAction(createLexicalAnalyzerContext(), PADDING_RIGHT); }
"\"padding-bottom\""    { return KeywordLexemeAction(createLexicalAnalyzerContext(), PADDING_BOTTOM); }
"\"padding-left\""      { return KeywordLexemeAction(createLexicalAnalyzerContext(), PADDING_LEFT); }
"\"border-width\""      { return KeywordLexemeAction(createLexicalAnalyzerContext(), BORDER_WIDTH); }
"\"border-style\""      { return KeywordLexemeAction(createLexicalAnalyzerContext(), BORDER_STYLE); }
"\"border-color\""      { return KeywordLexemeAction(createLexicalAnalyzerContext(), BORDER_COLOR); }
"\"border-radius\""     { return KeywordLexemeAction(createLexicalAnalyzerContext(), BORDER_RADIUS); }
"\"font-size\""         { return KeywordLexemeAction(createLexicalAnalyzerContext(), FONT_SIZE); }
"\"font-family\""       { return KeywordLexemeAction(createLexicalAnalyzerContext(), FONT_FAMILY); }
"\"font-weight\""       { return KeywordLexemeAction(createLexicalAnalyzerContext(), FONT_WEIGHT); }
"\"display\""           { return KeywordLexemeAction(createLexicalAnalyzerContext(), DISPLAY); }
"\"position\""          { return KeywordLexemeAction(createLexicalAnalyzerContext(), POSITION); }
"\"top\""               { return KeywordLexemeAction(createLexicalAnalyzerContext(), TOP); }
"\"right\""             { return KeywordLexemeAction(createLexicalAnalyzerContext(), RIGHT); }
"\"bottom\""            { return KeywordLexemeAction(createLexicalAnalyzerContext(), BOTTOM); }
"\"left\""              { return KeywordLexemeAction(createLexicalAnalyzerContext(), LEFT); }
"\"overflow\""          { return KeywordLexemeAction(createLexicalAnalyzerContext(), OVERFLOW); }
"\"opacity\""           { return KeywordLexemeAction(createLexicalAnalyzerContext(), OPACITY); }


$iterate                              { return KeywordLexemeAction(createLexicalAnalyzerContext(), ITERATOR_REF); }
\"color\"                             { return ValueLexemeAction(createLexicalAnalyzerContext(), COLOR); }
\"var\"                               { return ValueLexemeAction(createLexicalAnalyzerContext(), VAR); }
\"div\"                               { return ValueLexemeAction(createLexicalAnalyzerContext(), DIV); }
\"img\"                               { return ValueLexemeAction(createLexicalAnalyzerContext(), IMG); }
\"src\"                               { return ValueLexemeAction(createLexicalAnalyzerContext(), SRC); }
\"alt\"                               { return ValueLexemeAction(createLexicalAnalyzerContext(), ALT); }
\"h1\"                                { return ValueLexemeAction(createLexicalAnalyzerContext(), H1); }
\"h2\"                                { return ValueLexemeAction(createLexicalAnalyzerContext(), H2); }
\"h3\"                                { return ValueLexemeAction(createLexicalAnalyzerContext(), H3); }
\"h4\"                                { return ValueLexemeAction(createLexicalAnalyzerContext(), H4); }
\"a\"                                 { return ValueLexemeAction(createLexicalAnalyzerContext(), A); }
\"span\"                              { return ValueLexemeAction(createLexicalAnalyzerContext(), SPAN); }
\"ul\"                                { return ValueLexemeAction(createLexicalAnalyzerContext(), UL); }
\"li\"                                { return ValueLexemeAction(createLexicalAnalyzerContext(), LI); }
\"p\"                                 { return ValueLexemeAction(createLexicalAnalyzerContext(), P); }
\"title\"                             { return ValueLexemeAction(createLexicalAnalyzerContext(), TITLE); }
\"body\"                              { return ValueLexemeAction(createLexicalAnalyzerContext(), BODY); }
\"textalign\"                         { return ValueLexemeAction(createLexicalAnalyzerContext(), TEXTALIGN); }
\"ref\"                               { return ValueLexemeAction(createLexicalAnalyzerContext(), REF); }

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

"null"                              { return NullLexemeAction(createLexicalAnalyzerContext()); }

"$"                                 { return OperatorLexemeAction(createLexicalAnalyzerContext(), DOLLAR); }

[[:space:]]+                        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.                                   { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"
