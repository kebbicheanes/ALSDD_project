#ifndef LL_ANS_QUEUES_H
#define LL_ANS_QUEUES_H



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

typedef struct {
    int day;
    int month;
    int year;
} date;

typedef FILE File;

TList* getPersonality(File *f);
TList* getDatePersonality(File *f);
void getInfoByDates(TList *s, TList *DoB);
void getInfoByDates2(TList *s, TList *DoD);
TList* sortWord(TList *syn);
TList* sortWord2(TList *syn);
TList* sortPersonality(TList *syn);
TList* deletepersonality(File *f, TList *s, TList *a, char *name);
TList* updatePersonality(File *f, TList *s, TList *a, char *name, char *definition, char *DoB, char *DoD);
TList* similarPersonality(TList *s, char *word);
TList* countPersonality(TList *s, date *prt);
TList* palindromeName(TList *s);
TList* mergeNodes(TList *s, TList *a);
TList* merge2Nodes(TList *s, TList *a);
TList* addPersonality(TList *s, TList *a, char *name, char *DoB, char *DoD);
TList* addEvents(TList *b, char *namEvente, char *date);
TQueue* sName(TList *s);
TQueue* ageP(TList *a);
TQueue* toQueue(TList *merged);

#endif
