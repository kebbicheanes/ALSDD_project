#ifndef LL_AND_QUEUES_H
#define LL_AND_QUEUES_H

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

TList*  getPersonality(FILE *f);
TList*  getDatePersonality(FILE *f);
void    getInfoByDates(TList *s, TList *DoB);
void    getInfoByDates2(TList *s, TList *DoD);
TList*  sortWord(TList *syn);
TList*  sortWord2(TList *syn);
TList*  sortPersonality(TList *syn);
TList*  deletepersonality(FILE *f, TList *s, TList *a, char *name);
TList*  updatePersonality(FILE *f, TList *s, TList *a, char *name, char *definition, char *DoB, char *DoD);
TList*  similarPersonality(TList *s, char *word);
TList*  countPersonality(TList *s, date *prt);
TList*  palindromeName(TList *s);
TList*  mergeNodes(TList *s, TList *a);
TList*  merge2Nodes(TList *s, TList *a);
TList*  addPersonality(TList *s, TList *a, char *name, char *DoB, char *DoD);
TList*  addEvents(TList *b, char *namEvente, char *date);
TQueue* sName(TList *s);
TQueue* ageP(TList *a);
TQueue* toQueue(TList *merged);
void    printList(TList *head);
void    voidLibrary(void);
void    runLibrary(void);

#endif
