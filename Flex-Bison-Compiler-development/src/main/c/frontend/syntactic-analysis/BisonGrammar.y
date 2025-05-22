%{

#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */
	Token token;
	char * string;
	double floating;
	boolean boolean;

	/** Non-terminals. */
	Program * program;
	Object * object;

	ValueList * valueList;
	Value * value;
	Array * array;
	Pair * pair;
	PairList * pairList;
	Loop * loop;
	VarRef * varRef;
	Keyword * keyword;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parse succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { releaseValueList($$); } <valueList>
%destructor { releaseValue($$); } <value>
%destructor { releaseArray($$); } <array>
%destructor { releasePair($$); } <pair>
%destructor { releasePairList($$); } <pairList>
%destructor { releaseLoop($$); } <loop>
%destructor { releaseVarRef($$); } <varRef>

/** Terminals. */
%token <token> CLOSE_PARENTHESIS
%token <token> OPEN_PARENTHESIS
%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> OPEN_BRACKET
%token <token> CLOSE_BRACKET
%token <token> COMMA
%token <token> COLON
%token <token> SEMI
%token <token> FOR
%token <token> IN
%token <string> STRING
%token <string> IDENTIFIER
%token <token> JSON_NULL
%token <token> DOLLAR
%token <token> UNKNOWN
%token <token> TYPE
%token <token> CONTENT
%token <token> DIV
%token <token> VAR
%token <token> IMG
%token <token> SRC
%token <token> ALT
%token <token> BODY
%token <token> REF
%token <token> H1
%token <token> H2
%token <token> H3
%token <token> H4
%token <token> A
%token <token> SPAN
%token <token> P
%token <token> TITLE
%token <token> TEXTALIGN
%token <token> COLOR
%token <token> BACKGROUND_COLOR
%token <token> STYLE

/** Non-terminals. */
%type <program> program
%type <object> object
%type <pair> pair
%type <pairList> pairList
%type <value> value
%type <array> array
%type <valueList> valueList
%type <loop> loop
%type <varRef> variableRef
%type <keyword> text
%type <keyword> property
%type <keyword> key

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

// NOTA: Ya no se validan claves duplicadas, compatibilidad de contenido ni validez de elementos HTML en la gramática.
// Esas validaciones se delegan al backend.
// Las producciones type_pair y content_pair no pueden forzar que el string sea exactamente 'type' o 'content', eso también se delega al backend.

program: object                                                { $$ = ObjectProgramSemanticAction(currentCompilerState(), $1); }
	;

object: OPEN_BRACE pairList CLOSE_BRACE { $$ = ObjectSemanticAction($2); }
     ;

pairList: %empty                                               { $$ = emptyPairListAction(); }
    | pair                                                     { $$ = singlePairListSemanticAction($1); }
    | pairList COMMA pair                                      { $$ = pairListSemanticAction($1, $3); }
	;

pair: key COLON value                                      { $$ = PairSemanticAction($1, $3); }
	;

key: TYPE													{ $$ = KeywordSemanticAction($1); }
	| CONTENT												{ $$ = KeywordSemanticAction($1); }
	| DIV 													{ $$ = KeywordSemanticAction($1); }
	| VAR													{ $$ = KeywordSemanticAction($1); }
	| IMG													{ $$ = KeywordSemanticAction($1); }
	| SRC													{ $$ = KeywordSemanticAction($1); }
	| ALT													{ $$ = KeywordSemanticAction($1); }
	| BODY													{ $$ = KeywordSemanticAction($1); }
	| REF													{ $$ = KeywordSemanticAction($1); }
	| STYLE													{ $$ = KeywordSemanticAction($1); }
	| text													{ $$ = $1; }
	| property												{ $$ = $1; }
	;

property: TEXTALIGN											{ $$ = KeywordSemanticAction($1); }
	| COLOR													{ $$ = KeywordSemanticAction($1); }
	| BACKGROUND_COLOR										{ $$ = KeywordSemanticAction($1); }
	;

/** TODO maybe style lleva su propia produccion*/

text: H1													  { $$ = KeywordSemanticAction($1); }
	| H2													  { $$ = KeywordSemanticAction($1); }
	| H3													  { $$ = KeywordSemanticAction($1); }
	| H4													  { $$ = KeywordSemanticAction($1); }
	| A														  { $$ = KeywordSemanticAction($1); }
	| SPAN													  { $$ = KeywordSemanticAction($1); }
	| P														  { $$ = KeywordSemanticAction($1); }
	| TITLE													  { $$ = KeywordSemanticAction($1); }
	;

value: STRING                                                 { $$ = StringValueSemanticAction($1); }
    | JSON_NULL                                               { $$ = NullValueSemanticAction(); }
    | object                                                  { $$ = ObjectValueSemanticAction($1); }
    | array                                                   { $$ = ArrayValueSemanticAction($1); }
    | loop                                                    { $$ = LoopValueSemanticAction($1); }
    | variableRef                                             { $$ = VariableRefValueSemanticAction($1); }
    ;

array: OPEN_BRACKET valueList CLOSE_BRACKET                   { $$ = ArraySemanticAction($2); }
    ;

valueList: %empty                                             { $$ = emptyValueListAction(); }
    | value                                                   { $$ = singleValueListSemanticAction($1); }
    | valueList COMMA value                                   { $$ = valueListSemanticAction($1, $3); }
    ;

loop: FOR OPEN_PARENTHESIS IDENTIFIER IN object CLOSE_PARENTHESIS object
                                                             { $$ = LoopSemanticAction($3, $5, $7); }
    ;

variableRef: DOLLAR IDENTIFIER                               { $$ = VariableRefSemanticAction($2); }
    ;

%%
