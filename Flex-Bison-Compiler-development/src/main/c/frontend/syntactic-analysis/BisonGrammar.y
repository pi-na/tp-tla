%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */

	Token token;

	/** Non-terminals. */
	
	Program * program;
	Object * object;
	EntryList * entryList;
	Entry * entry;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parse succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { releaseEntryList($$); } <entryList>
%destructor { releaseEntry($$); } <entry>

/** Terminals. */
%token <token> CLOSE_PARENTHESIS
%token <token> OPEN_PARENTHESIS
%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> COMMA

%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program
%type <object> object
%type <entryList> entryList
%type <entry> entry

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

/** TODO: PONER OBJECT! */
program: object													{ $$ = ObjectProgramSemanticAction(currentCompilerState(), $1); }
	;

/** TODO: ARRAYS [] */
object: OPEN_BRACE entryList CLOSE_BRACE							   	{ $$ = ObjectSemanticAction($2); }
	;

entryList: entry												{ $$ = singleEntryListSemanticAction($1); }
    | entryList COMMA entry										{ $$ = entryListSemanticAction($1, $3); }
	;


entry: %empty															{ $$ = emptyEntryAction(); }
	;
%%
