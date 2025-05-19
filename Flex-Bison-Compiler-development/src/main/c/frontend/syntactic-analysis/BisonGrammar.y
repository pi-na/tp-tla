%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */
	char* string;
	int integer;
	double real;
	int boolean;
	Token token;

	/** Non-terminals. */
	Constant* constant;
	Expression* expression;
	Factor* factor;
	Program* program;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parse succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { releaseConstant($$); } <constant>
%destructor { releaseExpression($$); } <expression>
%destructor { releaseFactor($$); } <factor>

/** Terminals. */
%token <token> OPEN_BRACE CLOSE_BRACE
%token <token> OPEN_BRACKET CLOSE_BRACKET
%token <token> COLON COMMA
%token <string> STRING
%token <integer> INTEGER
%token <real> REAL
%token <boolean> TRUE FALSE
%token <token> NULL_TOKEN
%token <token> TYPE CONTENT STYLE SRC ALT ID CLASS
%token <token> HREF REF EXT_REF VAR
%token <token> IF THEN ELSE FOR WHILE
%token <token> HTML_TAG HEAD_TAG TITLE_TAG BODY_TAG
%token <token> DIV_TAG P_TAG H1_TAG IMG_TAG A_TAG
%token <token> B_TAG I_TAG BR_TAG HR_TAG CENTER_TAG
%token <token> AND OR NOT EQ NEQ LE GE LT GT
%token <token> ADD SUB MUL DIV
%token <token> OPEN_PARENTHESIS CLOSE_PARENTHESIS
%token <string> IDENTIFIER
%token <token> UNKNOWN

/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV

%%

program: expression                                                    { $$ = ExpressionProgramSemanticAction(currentCompilerState(), $1); }
    ;

expression: expression[left] ADD expression[right]                    { $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
    | expression[left] DIV expression[right]                        { $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
    | expression[left] MUL expression[right]                        { $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
    | expression[left] SUB expression[right]                        { $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
    | factor                                                        { $$ = FactorExpressionSemanticAction($1); }
    ;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS                { $$ = ExpressionFactorSemanticAction($2); }
    | constant                                                        { $$ = ConstantFactorSemanticAction($1); }
    ;

constant: INTEGER                                                    { $$ = IntegerConstantSemanticAction($1); }
    ;

/* Regla para aceptar tokens no utilizados */
unused_token: OPEN_BRACE | CLOSE_BRACE | OPEN_BRACKET | CLOSE_BRACKET
    | COLON | COMMA | STRING | REAL | TRUE | FALSE | NULL_TOKEN
    | TYPE | CONTENT | STYLE | SRC | ALT | ID | CLASS
    | HREF | REF | EXT_REF | VAR | IF | THEN | ELSE | FOR | WHILE
    | HTML_TAG | HEAD_TAG | TITLE_TAG | BODY_TAG | DIV_TAG | P_TAG
    | H1_TAG | IMG_TAG | A_TAG | B_TAG | I_TAG | BR_TAG | HR_TAG
    | CENTER_TAG | AND | OR | NOT | EQ | NEQ | LE | GE | LT | GT
    | IDENTIFIER | UNKNOWN
    ;

%%