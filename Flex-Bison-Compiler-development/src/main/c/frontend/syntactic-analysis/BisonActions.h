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
Pair * PairSemanticAction(char * key, Value * value);

Value * StringValueSemanticAction(char * value);
Value * IntegerValueSemanticAction(int value);
Value * FloatValueSemanticAction(double value);
Value * BooleanValueSemanticAction(boolean value);
Value * NullValueSemanticAction();
Value * ObjectValueSemanticAction(Object * object);
Value * ArrayValueSemanticAction(Array * array);
Value * ConditionalValueSemanticAction(Conditional * conditional);
Value * LoopValueSemanticAction(Loop * loop);
Value * VariableRefValueSemanticAction(VarRef * varRef);
Value * ElementValueSemanticAction(Element * element);

Array * ArraySemanticAction(ValueList * values);
ValueList * emptyValueListAction();
ValueList * singleValueListSemanticAction(Value * value);
ValueList * valueListSemanticAction(ValueList * valueList, Value * newValue);

Conditional * ConditionalSemanticAction(Expression * condition, Object * thenBranch, Object * elseBranch);
Conditional * ObjectConditionalSemanticAction(Object * condObject);
Loop * LoopSemanticAction(char * iteratorName, Object * iterable, Object * body);
VarRef * VariableRefSemanticAction(char * name);

Expression * IntegerExpressionSemanticAction(int value);
Expression * FloatExpressionSemanticAction(double value);
Expression * StringExpressionSemanticAction(char * value);
Expression * BooleanExpressionSemanticAction(boolean value);
Expression * VariableRefExpressionSemanticAction(VarRef * varRef);
Expression * BinaryExpressionSemanticAction(OperatorType op, Expression * left, Expression * right);
Expression * UnaryExpressionSemanticAction(OperatorType op, Expression * operand);

AttributeList * emptyAttributeListAction();
AttributeList * singleAttributeListSemanticAction(Attribute * attribute);
AttributeList * attributeListSemanticAction(AttributeList * attributeList, Attribute * newAttribute);
Attribute * AttributeSemanticAction(char * name, Value * value);

Element * ElementSemanticAction(Object * object);

#endif
