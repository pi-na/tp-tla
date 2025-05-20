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
<STRING>\"                          { EndStringLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); return STRING; }
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

[[:space:]]+                        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

\"([^\"\\]|\\.)*\"                  { return StringLexemeAction(createLexicalAnalyzerContext()); }
-?[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+)?   { return NumberLexemeAction(createLexicalAnalyzerContext()); }
true|false                          { return BooleanLexemeAction(createLexicalAnalyzerContext(), yytext[0] == 't' ? TRUE : FALSE); }
null                                { return NullLexemeAction(createLexicalAnalyzerContext()); }

"type"                              { return KeywordLexemeAction(createLexicalAnalyzerContext(), TYPE); }
"content"                           { return KeywordLexemeAction(createLexicalAnalyzerContext(), CONTENT); }
"style"                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), STYLE); }
"src"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), SRC); }
"alt"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), ALT); }
"id"                                { return KeywordLexemeAction(createLexicalAnalyzerContext(), ID); }
"class"                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), CLASS); }
"href"                              { return KeywordLexemeAction(createLexicalAnalyzerContext(), HREF); }
"ref"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), REF); }
"ext-ref"                           { return KeywordLexemeAction(createLexicalAnalyzerContext(), EXT_REF); }
"var"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), VAR); }
"if"                                { return KeywordLexemeAction(createLexicalAnalyzerContext(), IF); }
"then"                              { return KeywordLexemeAction(createLexicalAnalyzerContext(), THEN); }
"else"                              { return KeywordLexemeAction(createLexicalAnalyzerContext(), ELSE); }
"for"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(), FOR); }
"while"                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), WHILE); }

"&&"                                { return LogicalOperatorLexemeAction(createLexicalAnalyzerContext(), AND); }
"\|\|"                              { return LogicalOperatorLexemeAction(createLexicalAnalyzerContext(), OR); }
"!"                                 { return LogicalOperatorLexemeAction(createLexicalAnalyzerContext(), NOT); }
"=="                                { return ComparisonOperatorLexemeAction(createLexicalAnalyzerContext(), EQ); }
"!="                                { return ComparisonOperatorLexemeAction(createLexicalAnalyzerContext(), NEQ); }
"<="                                { return ComparisonOperatorLexemeAction(createLexicalAnalyzerContext(), LE); }
">="                                { return ComparisonOperatorLexemeAction(createLexicalAnalyzerContext(), GE); }
"<"                                 { return ComparisonOperatorLexemeAction(createLexicalAnalyzerContext(), LT); }
">"                                 { return ComparisonOperatorLexemeAction(createLexicalAnalyzerContext(), GT); }

"html"                              { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), HTML_TAG); }
"head"                              { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), HEAD_TAG); }
"title"                             { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), TITLE_TAG); }
"body"                              { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), BODY_TAG); }
"div"                               { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), DIV_TAG); }
"p"                                 { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), P_TAG); }
"h1"                                { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), H1_TAG); }
"img"                               { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), IMG_TAG); }
"a"                                 { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), A_TAG); }
"b"                                 { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), B_TAG); }
"i"                                 { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), I_TAG); }
"br"                                { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), BR_TAG); }
"hr"                                { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), HR_TAG); }
"center"                            { return HTMLTagLexemeAction(createLexicalAnalyzerContext(), CENTER_TAG); }

[a-zA-Z_][a-zA-Z0-9_]*              { return IdentifierLexemeAction(createLexicalAnalyzerContext()); }

.                                   { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"

Token StringLexemeAction(LexicalAnalyzerContext * ctx) {
    _logLexicalAnalyzerContext(__FUNCTION__, ctx);
    ctx->semanticValue->string = strdup(ctx->lexeme); // Asume que semanticValue tiene un campo string
    destroyLexicalAnalyzerContext(ctx);
    return STRING;
}

Token NumberLexemeAction(LexicalAnalyzerContext * ctx) {
    _logLexicalAnalyzerContext(__FUNCTION__, ctx);
    if (strchr(ctx->lexeme, '.') || strchr(ctx->lexeme, 'e') || strchr(ctx->lexeme, 'E')) {
        ctx->semanticValue->real = atof(ctx->lexeme);
        destroyLexicalAnalyzerContext(ctx);
        return REAL;
    } else {
        ctx->semanticValue->integer = atoi(ctx->lexeme);
        destroyLexicalAnalyzerContext(ctx);
        return INTEGER;
    }
}

Token BooleanLexemeAction(LexicalAnalyzerContext * ctx, Token token) {
    _logLexicalAnalyzerContext(__FUNCTION__, ctx);
    ctx->semanticValue->boolean = (token == TRUE);
    destroyLexicalAnalyzerContext(ctx);
    return token;
}

Token NullLexemeAction(LexicalAnalyzerContext * ctx) {
    _logLexicalAnalyzerContext(__FUNCTION__, ctx);
    ctx->semanticValue->token = NULL_TOKEN;
    destroyLexicalAnalyzerContext(ctx);
    return NULL_TOKEN;
}

Token KeywordLexemeAction(LexicalAnalyzerContext * ctx, Token token) {
    _logLexicalAnalyzerContext(__FUNCTION__, ctx);
    ctx->semanticValue->token = token;
    destroyLexicalAnalyzerContext(ctx);
    return token;
}

Token OperatorLexemeAction(LexicalAnalyzerContext * ctx, Token token) {
    _logLexicalAnalyzerContext(__FUNCTION__, ctx);
    ctx->semanticValue->token = token;
    destroyLexicalAnalyzerContext(ctx);
    return token;
}

Token IdentifierLexemeAction(LexicalAnalyzerContext * ctx) {
    _logLexicalAnalyzerContext(__FUNCTION__, ctx);
    ctx->semanticValue->string = strdup(ctx->lexeme);
    destroyLexicalAnalyzerContext(ctx);
    return IDENTIFIER;
}
