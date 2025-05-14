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
	Object* object;
	Array* array;
	Entry* entry;
	Entries* entries;
	StyleProperties* styleProperties;
	Value* value;
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
%token <string> IDENTIFIER

/** Non-terminals. */
%type <object> object
%type <array> array arrayElements
%type <entries> entries
%type <entries> htmlEntries headEntries bodyEntries
%type <entries> divEntries pEntries h1Entries
%type <entries> imgEntries aEntries centerEntries
%type <entry> entry
%type <value> value

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: object
	;

object: OPEN_BRACE entries CLOSE_BRACE
	;

entries: htmlEntries     /* Para el elemento raíz HTML */
	| headEntries        /* Para <head> */
	| bodyEntries        /* Para <body> */
	| divEntries         /* Para <div> */
	| pEntries          /* Para <p> */
	| h1Entries         /* Para <h1> */
	| imgEntries        /* Para <img> */
	| aEntries          /* Para <a> */
	| centerEntries     /* Para <center> */
	;

/* Reglas para elemento HTML raíz */
htmlEntries: TYPE COLON HTML_TAG COMMA headAndBodyEntries                /* type primero */
	| headAndBodyEntries COMMA TYPE COLON HTML_TAG                      /* type al final */
	;

headAndBodyEntries: headEntry COMMA bodyEntry
	| bodyEntry COMMA headEntry
	;

headEntry: OPEN_BRACE headEntries CLOSE_BRACE
	;

bodyEntry: OPEN_BRACE bodyEntries CLOSE_BRACE
	;

/* Reglas para <head> */
headEntries: TYPE COLON HEAD_TAG COMMA titleEntry                       /* type primero */
	| titleEntry COMMA TYPE COLON HEAD_TAG                             /* type al final */
	;

titleEntry: CONTENT COLON OPEN_BRACE titleEntries CLOSE_BRACE
	;

titleEntries: TYPE COLON TITLE_TAG COMMA titleRequiredEntries          /* type primero */
	| titleRequiredEntries COMMA TYPE COLON TITLE_TAG                  /* type al final */
	;

titleRequiredEntries: CONTENT COLON STRING
	;

/* Reglas para <body> */
bodyEntries: TYPE COLON BODY_TAG COMMA bodyContentEntries bodyOptionalEntries    /* type primero */
	| bodyContentEntries COMMA TYPE COLON BODY_TAG bodyOptionalEntries          /* type en medio */
	| bodyOptionalEntries TYPE COLON BODY_TAG COMMA bodyContentEntries          /* type antes del final */
	| bodyOptionalEntries bodyContentEntries COMMA TYPE COLON BODY_TAG          /* type al final */
	;

bodyContentEntries: CONTENT COLON bodyContent
	;

bodyContent: value
	| array
	;

bodyOptionalEntries: /* empty */
	| COMMA STYLE COLON styleObject bodyOptionalEntries
	| COMMA CLASS COLON STRING bodyOptionalEntries
	| COMMA ID COLON STRING bodyOptionalEntries
	;

/* Reglas para <div> */
divEntries: TYPE COLON DIV_TAG COMMA divRequiredEntries divOptionalEntries    /* type primero */
	| divRequiredEntries COMMA TYPE COLON DIV_TAG divOptionalEntries          /* type en medio */
	| divOptionalEntries TYPE COLON DIV_TAG COMMA divRequiredEntries          /* type antes del final */
	| divOptionalEntries divRequiredEntries COMMA TYPE COLON DIV_TAG          /* type al final */
	;

divRequiredEntries: CONTENT COLON divContent
	;

divContent: value
	| array
	;

divOptionalEntries: /* empty */
	| COMMA STYLE COLON styleObject divOptionalEntries
	| COMMA CLASS COLON STRING divOptionalEntries
	| COMMA ID COLON STRING divOptionalEntries
	;

/* Reglas para <p> */
pEntries: TYPE COLON P_TAG COMMA pRequiredEntries pOptionalEntries    /* type primero */
	| pRequiredEntries COMMA TYPE COLON P_TAG pOptionalEntries          /* type en medio */
	| pOptionalEntries TYPE COLON P_TAG COMMA pRequiredEntries          /* type antes del final */
	| pOptionalEntries pRequiredEntries COMMA TYPE COLON P_TAG          /* type al final */
	;

pRequiredEntries: CONTENT COLON value
	;

pOptionalEntries: /* empty */
	| COMMA STYLE COLON styleObject pOptionalEntries
	| COMMA CLASS COLON STRING pOptionalEntries
	| COMMA ID COLON STRING pOptionalEntries
	;

/* Reglas para <h1> */
h1Entries: TYPE COLON H1_TAG COMMA h1RequiredEntries h1OptionalEntries    /* type primero */
	| h1RequiredEntries COMMA TYPE COLON H1_TAG h1OptionalEntries          /* type en medio */
	| h1OptionalEntries TYPE COLON H1_TAG COMMA h1RequiredEntries          /* type antes del final */
	| h1OptionalEntries h1RequiredEntries COMMA TYPE COLON H1_TAG          /* type al final */
	;

h1RequiredEntries: CONTENT COLON value
	;

h1OptionalEntries: /* empty */
	| COMMA STYLE COLON styleObject h1OptionalEntries
	| COMMA CLASS COLON STRING h1OptionalEntries
	| COMMA ID COLON STRING h1OptionalEntries
	;

/* Reglas para <a> */
aEntries: TYPE COLON A_TAG COMMA aRequiredEntries aOptionalEntries    /* type primero */
	| aRequiredEntries COMMA TYPE COLON A_TAG aOptionalEntries          /* type en medio */
	| aOptionalEntries TYPE COLON A_TAG COMMA aRequiredEntries          /* type antes del final */
	| aOptionalEntries aRequiredEntries COMMA TYPE COLON A_TAG          /* type al final */
	;

aRequiredEntries: aLinkEntry COMMA aContentEntry
	| aContentEntry COMMA aLinkEntry
	;

aLinkEntry: HREF COLON STRING
	| REF COLON STRING
	| EXT_REF COLON STRING
	;

aContentEntry: CONTENT COLON value
	;

aOptionalEntries: /* empty */
	| COMMA STYLE COLON styleObject aOptionalEntries
	| COMMA CLASS COLON STRING aOptionalEntries
	| COMMA ID COLON STRING aOptionalEntries
	;

/* Reglas para <center> */
centerEntries: TYPE COLON CENTER_TAG COMMA centerRequiredEntries centerOptionalEntries    /* type primero */
	| centerRequiredEntries COMMA TYPE COLON CENTER_TAG centerOptionalEntries          /* type en medio */
	| centerOptionalEntries TYPE COLON CENTER_TAG COMMA centerRequiredEntries          /* type antes del final */
	| centerOptionalEntries centerRequiredEntries COMMA TYPE COLON CENTER_TAG          /* type al final */
	;

centerRequiredEntries: CONTENT COLON centerContent
	;

centerContent: value
	| array
	;

centerOptionalEntries: /* empty */
	| COMMA STYLE COLON styleObject centerOptionalEntries
	| COMMA CLASS COLON STRING centerOptionalEntries
	| COMMA ID COLON STRING centerOptionalEntries
	;

/* Reglas para estilos mejoradas */
styleObject: OPEN_BRACE styleProperties CLOSE_BRACE
	;

styleProperties: styleProperty
	| styleProperty COMMA styleProperties
	;

styleProperty: styleKey COLON styleValue
	;

styleKey: IDENTIFIER
	;

styleValue: STRING
	| INTEGER
	| REAL
	| cssKeyword
	;

cssKeyword: CENTER_TAG    /* para text-align: center */
	| /* otros valores CSS válidos */
	;

array: OPEN_BRACKET arrayElements CLOSE_BRACKET
	;

arrayElements: /* empty */
	| value
	| value COMMA arrayElements
	;

%%
