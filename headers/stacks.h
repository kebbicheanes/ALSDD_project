#ifndef STACKS_H
#define STACKS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"

TStack* toStack(TList *merged);
TStack* getInfoPersonality(TStack *stk, char *name);
TStack* sortNameStack(TStack *s);
TStack* deleteName(TStack *stk, char *name);
TStack* updateStack(TStack *stk, char *name, char *def, char *DoB, char *DoD);
TQueue* stackToQueue(TStack *stk);
TList*  stackToList(TStack *stk);
TStack* addNameStack(TStack *stk, char *name, char *definition, char *DoB, char *DoD);
TStack* definitionStack(TStack *stk);
TStack* pronunciationStack(TStack *stk, TStack **longStk);
char*   getSmallest(TStack *stk);
void    continuousSearch(TStack *stk);
bool    isPersonalityKilled(char *word);
TStack* recRevStack(TStack *stk);
void    runStacks(TList *merged);

#endif
