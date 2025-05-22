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
%token <token> LOOP
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
%token <token> ITERABLE
%token <token> ITERATE
%token <token> ITERATOR_REF
%token <token> ITERATE_STRING
%token <token> UL
%token <token> LI
%token <token> ID
%token <token> CLASS
%token <token> WIDTH
%token <token> HEIGHT

/** Non-terminals. */
%type <program> program
%type <object> object
%type <pair> pair
%type <pairList> pairList
%type <value> value
%type <array> array
%type <valueList> valueList
%type <varRef> variableRef
%type <value> text
%type <value> property
%type <value> key

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

// NOTA: Ya no se validan claves duplicadas, compatibilidad de contenido ni validez de elementos HTML en la gramática.
// Esas validaciones se delegan al backend.
// Las producciones type_pair y content_pair no pueden forzar que el string sea exactamente 'type' o 'content', eso también se delega al backend.

program: object                                                { $$ = ObjectProgramSemanticAction(currentCompilerState(), $1); }
	;

object: OPEN_BRACE pairList CLOSE_BRACE { $$ = ObjectSemanticAction($2); }
     ;

pairList: pair                                                     { $$ = singlePairListSemanticAction($1); }
    | pairList COMMA pair                                      { $$ = pairListSemanticAction($1, $3); }
	;

pair: key COLON value                                      { $$ = PairSemanticAction($1, $3); }
	;

key: TYPE													{ $$ = TokenValueSemanticAction($1); }
	| CONTENT												{ $$ = TokenValueSemanticAction($1); }
	| LOOP													{ $$ = TokenValueSemanticAction($1); }
	| ITERABLE												{ $$ = TokenValueSemanticAction($1); }
	| ITERATE												{ $$ = TokenValueSemanticAction($1); }
	| ID													{ $$ = TokenValueSemanticAction($1); }
	| CLASS													{ $$ = TokenValueSemanticAction($1); }
	| WIDTH													{ $$ = TokenValueSemanticAction($1); }
	| HEIGHT												{ $$ = TokenValueSemanticAction($1); }
	| SRC													{ $$ = TokenValueSemanticAction($1); }
	| ALT													{ $$ = TokenValueSemanticAction($1); }
	| REF													{ $$ = TokenValueSemanticAction($1); }
	;

value: STRING                                                 	{ $$ = StringValueSemanticAction($1); }
    | JSON_NULL                                               	{ $$ = NullValueSemanticAction(); }
    | object                                                  	{ $$ = ObjectValueSemanticAction($1); }
    | array                                                   	{ $$ = ArrayValueSemanticAction($1); }
    | variableRef                                             	{ $$ = VariableRefValueSemanticAction($1); }
	| DIV 														{ $$ = TokenValueSemanticAction($1); }
	| VAR														{ $$ = TokenValueSemanticAction($1); }
	| IMG														{ $$ = TokenValueSemanticAction($1); }
	| BODY														{ $$ = TokenValueSemanticAction($1); }
	| STYLE														{ $$ = TokenValueSemanticAction($1); }
	| ITERATOR_REF												{ $$ = TokenValueSemanticAction($1); }
	| text														{ $$ = $1; }
	| property													{ $$ = $1; }
	;

property: TEXTALIGN												{ $$ = TokenValueSemanticAction($1); }
	| COLOR														{ $$ = TokenValueSemanticAction($1); }
	| BACKGROUND_COLOR											{ $$ = TokenValueSemanticAction($1); }
	;

/** TODO maybe style lleva su propia produccion */

text: H1													  { $$ = TokenValueSemanticAction($1); }
	| H2													  { $$ = TokenValueSemanticAction($1); }
	| H3													  { $$ = TokenValueSemanticAction($1); }
	| H4													  { $$ = TokenValueSemanticAction($1); }
	| A														  { $$ = TokenValueSemanticAction($1); }
	| SPAN													  { $$ = TokenValueSemanticAction($1); }
	| P														  { $$ = TokenValueSemanticAction($1); }
	| TITLE													  { $$ = TokenValueSemanticAction($1); }
	| UL													  { $$ = TokenValueSemanticAction($1); }
	| LI													  { $$ = TokenValueSemanticAction($1); }
	;
array: OPEN_BRACKET valueList CLOSE_BRACKET                   { $$ = ArraySemanticAction($2); }
    ;

valueList: %empty                                             { $$ = emptyValueListAction(); }
    | value                                                   { $$ = singleValueListSemanticAction($1); }
    | valueList COMMA value                                   { $$ = valueListSemanticAction($1, $3); }
    ;							  							

variableRef: DOLLAR IDENTIFIER                               { $$ = VariableRefSemanticAction($2); }
    ;

%%
