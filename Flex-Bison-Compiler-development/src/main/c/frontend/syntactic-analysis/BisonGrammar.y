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
	int integer;
	double floating;
	boolean boolean;

	/** Non-terminals. */
	Program * program;
	Object * object;

	// Nuevos nodos del AST
	ValueList * valueList;
	Value * value;
	Array * array;
	Pair * pair;
	PairList * pairList;
	Loop * loop;
	VarRef * varRef;
	AttributeList * attributeList;
	Attribute * attribute;
	Element * element;
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
%destructor { releaseAttributeList($$); } <attributeList>
%destructor { releaseAttribute($$); } <attribute>
%destructor { releaseElement($$); } <element>

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
%token <floating> FLOAT
%token <token> JSON_NULL
%token <token> DOLLAR
%token <token> UNKNOWN

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
%type <attributeList> attributeList
%type <attribute> attribute
%type <element> element

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: object                                                { $$ = ObjectProgramSemanticAction(currentCompilerState(), $1); }
	;

object: OPEN_BRACE pairList CLOSE_BRACE                        { $$ = ObjectSemanticAction($2); }
	;

pairList: %empty                                               { $$ = emptyPairListAction(); }
    | pair                                                     { $$ = singlePairListSemanticAction($1); }
    | pairList COMMA pair                                      { $$ = pairListSemanticAction($1, $3); }
	;

pair: STRING COLON value                                      { $$ = PairSemanticAction($1, $3); }
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

element: object                                              { $$ = ElementSemanticAction($1); }
    ;

attributeList: %empty                                        { $$ = emptyAttributeListAction(); }
    | attribute                                              { $$ = singleAttributeListSemanticAction($1); }
    | attributeList COMMA attribute                          { $$ = attributeListSemanticAction($1, $3); }
    ;

attribute: STRING COLON value                               { $$ = AttributeSemanticAction($1, $3); }
    ;

%%
