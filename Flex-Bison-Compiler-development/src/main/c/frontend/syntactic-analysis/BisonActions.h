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
Object * ObjectSemanticAction(EntryList * entries);
EntryList * singleEntryListSemanticAction(Entry * entry);
EntryList * entryListSemanticAction(EntryList * entryList, Entry * newEntry);
Entry * emptyEntryAction();

#endif
