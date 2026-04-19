#include <stdbool.h>
#ifndef STACKS_H
#define STACKS_H

typedef struct TStack {
    char          name[100];
    char          definition[1024];
    char          DoB[20];   /* Date of Birth  */
    char          DoD[256];   /* Date of Death  */
    struct TStack *next;
} TStack;

typedef struct TList {
    char          name[100];
    char          definition[1024];
    char          DoB[20];   /* Date of Birth  */
    char          DoD[256];   /* Date of Death  */
    struct TList *next;
} TList;

typedef struct TQueue {
    char          name[100];
    char          definition[1024];
    char          DoB[20];   /* Date of Birth  */
    char          DoD[256];   /* Date of Death  */
    struct TQueue *next;
} TQueue;


TStack* toStack(TList *merged);
TStack* getInfoPersonality(TStack *stk, char *name);
TStack* sortNameStack(TStack *s);
TStack* deleteName(TStack *stk, char *name);
TStack* updateStack(TStack *stk, char *name, char *def, char *DoB, char *DoD);
TQueue* stackToQueue(TStack *stk);
TList* stackToList(TStack *stk);
TStack* addNameStack(TStack *stk, char *name, char *definition, char *DoB, char *DoD);
TStack* definitionStack(TStack *stk);
TStack* pronunciationStack(TStack *stk);
char* getSmallest(TStack *stk);
void continuousSearch(TStack *stk);
bool isPersonalityKilled(char *word);
TStack* recRevStack (TStack *stk);

#endif
