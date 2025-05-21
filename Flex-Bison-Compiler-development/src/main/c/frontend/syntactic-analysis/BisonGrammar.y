%{

#include "../../shared/Type.h"
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
	
	/** Sacamos el EntryList!!!!!. */

	// Nuevos nodos del AST
	ValueList * valueList;
	Value * value;
	Array * array;
	Pair * pair;
	PairList * pairList;
	Conditional * conditional;
	Loop * loop;
	Expression * expression;
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
%destructor { releaseConditional($$); } <conditional>
%destructor { releaseLoop($$); } <loop>
%destructor { releaseExpression($$); } <expression>
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

%token <string> STRING
%token <string> IDENTIFIER
%token <integer> INTEGER
%token <floating> FLOAT
%token <boolean> TRUE FALSE
%token <token> JSON_NULL

// Palabras clave
%token <token> IF ELSE FOR

// Operadores
%token <token> EQUALS NOT_EQUALS GREATER_THAN LESS_THAN GREATER_EQUAL LESS_EQUAL AND OR NOT
%token <token> ADD SUB MUL DIV ASSIGN DOLLAR

%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program
%type <object> object
%type <pair> pair
%type <pairList> pairList
%type <value> value
%type <array> array
%type <valueList> valueList
%type <conditional> conditional
%type <loop> loop
%type <expression> expression
%type <varRef> variableRef
%type <attributeList> attributeList
%type <attribute> attribute
%type <element> element

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left OR
%left AND
%left EQUALS NOT_EQUALS
%left GREATER_THAN LESS_THAN GREATER_EQUAL LESS_EQUAL
%left ADD SUB
%left MUL DIV
%right NOT

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
    | INTEGER                                                 { $$ = IntegerValueSemanticAction($1); }
    | FLOAT                                                   { $$ = FloatValueSemanticAction($1); }
    | TRUE                                                    { $$ = BooleanValueSemanticAction(true); }
    | FALSE                                                   { $$ = BooleanValueSemanticAction(false); }
    | JSON_NULL                                               { $$ = NullValueSemanticAction(); }
    | object                                                  { $$ = ObjectValueSemanticAction($1); }
    | array                                                   { $$ = ArrayValueSemanticAction($1); }
    | conditional                                             { $$ = ConditionalValueSemanticAction($1); }
    | loop                                                    { $$ = LoopValueSemanticAction($1); }
    | variableRef                                             { $$ = VariableRefValueSemanticAction($1); }
    ;

array: OPEN_BRACKET valueList CLOSE_BRACKET                   { $$ = ArraySemanticAction($2); }
    ;

valueList: %empty                                             { $$ = emptyValueListAction(); }
    | value                                                   { $$ = singleValueListSemanticAction($1); }
    | valueList COMMA value                                   { $$ = valueListSemanticAction($1, $3); }
    ;

conditional: IF OPEN_PARENTHESIS expression CLOSE_PARENTHESIS object
                                                             { $$ = ConditionalSemanticAction($3, $5, NULL); }
    | IF OPEN_PARENTHESIS expression CLOSE_PARENTHESIS object ELSE object
                                                             { $$ = ConditionalSemanticAction($3, $5, $7); }
    ;

loop: FOR OPEN_PARENTHESIS expression SEMI expression SEMI expression CLOSE_PARENTHESIS object
                                                             { $$ = LoopSemanticAction($3, $5, $7, $9); }
    ;

variableRef: DOLLAR IDENTIFIER                               { $$ = VariableRefSemanticAction($2); }
    ;

expression: INTEGER                                           { $$ = IntegerExpressionSemanticAction($1); }
    | FLOAT                                                   { $$ = FloatExpressionSemanticAction($1); }
    | STRING                                                  { $$ = StringExpressionSemanticAction($1); }
    | TRUE                                                    { $$ = BooleanExpressionSemanticAction(true); }
    | FALSE                                                   { $$ = BooleanExpressionSemanticAction(false); }
    | variableRef                                             { $$ = VariableRefExpressionSemanticAction($1); }
    | expression EQUALS expression                            { $$ = BinaryExpressionSemanticAction(EQUALS_OP, $1, $3); }
    | expression NOT_EQUALS expression                        { $$ = BinaryExpressionSemanticAction(NOT_EQUALS_OP, $1, $3); }
    | expression GREATER_THAN expression                      { $$ = BinaryExpressionSemanticAction(GREATER_THAN_OP, $1, $3); }
    | expression LESS_THAN expression                         { $$ = BinaryExpressionSemanticAction(LESS_THAN_OP, $1, $3); }
    | expression GREATER_EQUAL expression                     { $$ = BinaryExpressionSemanticAction(GREATER_EQUAL_OP, $1, $3); }
    | expression LESS_EQUAL expression                        { $$ = BinaryExpressionSemanticAction(LESS_EQUAL_OP, $1, $3); }
    | expression AND expression                               { $$ = BinaryExpressionSemanticAction(AND_OP, $1, $3); }
    | expression OR expression                                { $$ = BinaryExpressionSemanticAction(OR_OP, $1, $3); }
    | NOT expression                                          { $$ = UnaryExpressionSemanticAction(NOT_OP, $2); }
    | expression ADD expression                               { $$ = BinaryExpressionSemanticAction(ADD_OP, $1, $3); }
    | expression SUB expression                               { $$ = BinaryExpressionSemanticAction(SUB_OP, $1, $3); }
    | expression MUL expression                               { $$ = BinaryExpressionSemanticAction(MUL_OP, $1, $3); }
    | expression DIV expression                               { $$ = BinaryExpressionSemanticAction(DIV_OP, $1, $3); }
    | OPEN_PARENTHESIS expression CLOSE_PARENTHESIS           { $$ = $2; }
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
