#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */

Program * ObjectProgramSemanticAction(CompilerState * compilerState, Object * object);
Object * ObjectSemanticAction(PairList * pairs);
EntryList * singleEntryListSemanticAction(Entry * entry);
EntryList * entryListSemanticAction(EntryList * entryList, Entry * newEntry);
Entry * emptyEntryAction();

PairList * emptyPairListAction();
PairList * singlePairListSemanticAction(Pair * pair);
PairList * pairListSemanticAction(PairList * pairList, Pair * newPair);
Pair * PairSemanticAction(Keyword * key, Value * value);
Keyword * KeywordSemanticAction(Token type);
Value * TokenValueSemanticAction(Token type);

Value * StringValueSemanticAction(char * value);
Value * NullValueSemanticAction();
Value * ObjectValueSemanticAction(Object * object);
Value * ArrayValueSemanticAction(Array * array);
Value * VariableRefValueSemanticAction(VarRef * varRef);

Array * ArraySemanticAction(ValueList * values);
ValueList * emptyValueListAction();
ValueList * singleValueListSemanticAction(Value * value);
ValueList * valueListSemanticAction(ValueList * valueList, Value * newValue);

VarRef * VariableRefSemanticAction(char * name);

#endif
