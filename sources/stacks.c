#include <stdio.h>
#include <stdlib.h>
#include "../headers/types.h"
#include "../headers/stacks.h"
#include "../headers/stacks.h"       



static TStack* pushNode(TStack *top, const char *name, const char *def,
                        const char *dob, const char *dod) {
    TStack *node = (TStack*)malloc(sizeof(TStack));
    if (!node) return top;
    strncpy(node->name,       name, sizeof(node->name) - 1);
    strncpy(node->definition, def,  sizeof(node->definition) - 1);
    strncpy(node->DoB,        dob,  sizeof(node->DoB) - 1);
    strncpy(node->DoD,        dod,  sizeof(node->DoD) - 1);
    node->name[sizeof(node->name)-1]             = '\0';
    node->definition[sizeof(node->definition)-1] = '\0';
    node->DoB[sizeof(node->DoB)-1]               = '\0';
    node->DoD[sizeof(node->DoD)-1]               = '\0';
    node->next = top;
    return node;
}


static int wordCount(const char *str) {
    int count = 0, inWord = 0;
    for (; *str; str++) {
        if (*str == ' ') { inWord = 0; }
        else if (!inWord) { inWord = 1; count++; }
    }
    return count;
}


static int extractYear(const char *date) {
    if (!date || !*date) return 0;
    const char *slash2 = strrchr(date, '/');
    if (slash2) return atoi(slash2 + 1);
    return atoi(date);
}


void printStack(TStack *stk) {
    if (!stk) { printf("  (empty stack)\n"); return; }
    int i = 0;
    while (stk) {
        printf("[%d] Name: %s\n", i++, stk->name);
        printf("     DoB : %s  |  DoD : %s\n", stk->DoB, stk->DoD);
        printf("     Def : %s\n", stk->definition);
        printf("     ─────────────────────────────────\n");
        stk = stk->next;
    }
}

TStack* toStack(TList *merged) {
    TStack *top = NULL;
    TList  *cur = merged;
    while (cur) {
        top = pushNode(top, cur->name, cur->definition, cur->DoB, cur->DoD);
        cur = cur->next;
    }
    return top;
}


TQueue* stackToQueue(TStack *stk) {
    int n = 0;
    TStack *p = stk;
    while (p) { n++; p = p->next; }
    if (n == 0) return NULL;

    TStack **arr = (TStack**)malloc(n * sizeof(TStack*));
    p = stk;
    for (int i = 0; i < n; i++) { arr[i] = p; p = p->next; }

    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (strcmp(arr[j]->name, arr[j+1]->name) > 0) {
                TStack *tmp = arr[j]; arr[j] = arr[j+1]; arr[j+1] = tmp;
            }

    TQueue *head = NULL, *tail = NULL;
    for (int i = 0; i < n; i++) {
        TQueue *q = (TQueue*)malloc(sizeof(TQueue));
        if (!q) continue;
        strcpy(q->name,       arr[i]->name);
        strcpy(q->definition, arr[i]->definition);
        strcpy(q->DoB,        arr[i]->DoB);
        strcpy(q->DoD,        arr[i]->DoD);
        q->next = NULL;
        if (!head) { head = q; tail = q; }
        else       { tail->next = q; tail = q; }
    }
    free(arr);
    return head;
}


TList* stackToList(TStack *stk) {
    TList *head = NULL, *tail = NULL;
    TStack *cur = stk;

    while (cur) {
        TList *newNode = (TList*)malloc(sizeof(TList));
        if (!newNode) { cur = cur->next; continue; }
        strcpy(newNode->name,       cur->name);
        strcpy(newNode->definition, cur->definition);
        strcpy(newNode->DoB,        cur->DoB);
        strcpy(newNode->DoD,        cur->DoD);
        newNode->next = NULL;
        newNode->prev = NULL;

        if (!head || strcmp(newNode->name, head->name) <= 0) {
            newNode->next = head;
            if (head) head->prev = newNode;
            head = newNode;
            if (!tail) tail = newNode;
        } else {
            TList *pos = head;
            while (pos->next && strcmp(newNode->name, pos->next->name) > 0)
                pos = pos->next;
            newNode->next = pos->next;
            if (pos->next) pos->next->prev = newNode;
            pos->next     = newNode;
            newNode->prev = pos;
            if (!newNode->next) tail = newNode;
        }
        cur = cur->next;
    }
    return head;
}

TStack* getInfoPersonality(TStack *stk, char *name) {
    TStack *cur = stk;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            printf("\n── Found: %s ──\n", cur->name);
            printf("  DoB : %s\n  DoD : %s\n  Def : %s\n",
                   cur->DoB, cur->DoD, cur->definition);
            TStack *result = NULL;
            result = pushNode(result, cur->name, cur->definition,
                              cur->DoB, cur->DoD);
            return result;
        }
        cur = cur->next;
    }
    printf("  Personality '%s' not found in stack.\n", name);
    return NULL;
}

TStack* sortNameStack(TStack *s) {
    TStack *sorted = NULL;
    TStack *cur    = s;
    while (cur) {
        TStack *next = cur->next;
        /* insert cur into sorted in the right position */
        if (!sorted || strcmp(cur->name, sorted->name) <= 0) {
            cur->next = sorted;
            sorted    = cur;
        } else {
            TStack *pos = sorted;
            while (pos->next && strcmp(cur->name, pos->next->name) > 0)
                pos = pos->next;
            cur->next = pos->next;
            pos->next = cur;
        }
        cur = next;
    }
    return sorted;
}


TStack* definitionStack(TStack *stk) {
    if (!stk || !stk->next) return stk;
    int swapped;
    do {
        swapped = 0;
        TStack *cur = stk;
        while (cur->next) {
            if (wordCount(cur->definition) > wordCount(cur->next->definition)) {
                /* swap data */
                char tmp[2048];
                #define SWAP_FIELD(a, b, buf, sz) \
                    strncpy(buf, (a), sz); strncpy((a), (b), sz); strncpy((b), buf, sz);
                char tmpName[150], tmpDoB[30], tmpDoD[30], tmpDef[2048];
                SWAP_FIELD(cur->name,       cur->next->name,       tmpName, 150);
                SWAP_FIELD(cur->DoB,        cur->next->DoB,        tmpDoB,  30);
                SWAP_FIELD(cur->DoD,        cur->next->DoD,        tmpDoD,  30);
                SWAP_FIELD(cur->definition, cur->next->definition, tmpDef,  2048);
                (void)tmp;
                swapped = 1;
            }
            cur = cur->next;
        }
    } while (swapped);
    return stk;
}

TStack* pronunciationStack(TStack *stk, TStack **longStk) {
    TStack *shortTop = NULL, *longTop = NULL;
    TStack *cur = stk;
    while (cur) {
        TStack *next = cur->next;
        if (wordCount(cur->definition) <= 10) {
            cur->next = shortTop; shortTop = cur;
        } else {
            cur->next = longTop;  longTop  = cur;
        }
        cur = next;
    }
    if (longStk) *longStk = longTop;
    return shortTop;
}

TStack* addNameStack(TStack *stk, char *name, char *definition,
                     char *DoB, char *DoD) {
    TStack *newNode = (TStack*)malloc(sizeof(TStack));
    if (!newNode) return stk;
    strncpy(newNode->name,       name,       sizeof(newNode->name) - 1);
    strncpy(newNode->definition, definition, sizeof(newNode->definition) - 1);
    strncpy(newNode->DoB,        DoB,        sizeof(newNode->DoB) - 1);
    strncpy(newNode->DoD,        DoD,        sizeof(newNode->DoD) - 1);
    newNode->name[sizeof(newNode->name)-1]             = '\0';
    newNode->definition[sizeof(newNode->definition)-1] = '\0';
    newNode->DoB[sizeof(newNode->DoB)-1]               = '\0';
    newNode->DoD[sizeof(newNode->DoD)-1]               = '\0';

    if (!stk || strcmp(name, stk->name) <= 0) {
        newNode->next = stk;
        return newNode;
    }
    TStack *pos = stk;
    while (pos->next && strcmp(name, pos->next->name) > 0)
        pos = pos->next;
    newNode->next = pos->next;
    pos->next     = newNode;
    return stk;
}

TStack* deleteName(TStack *stk, char *name) {
    if (!stk) return NULL;
    if (strcmp(stk->name, name) == 0) {
        TStack *rest = stk->next;
        free(stk);
        return rest;
    }
    TStack *cur = stk;
    while (cur->next && strcmp(cur->next->name, name) != 0)
        cur = cur->next;
    if (cur->next) {
        TStack *del = cur->next;
        cur->next   = del->next;
        free(del);
    } else {
        printf("  '%s' not found in stack.\n", name);
    }
    return stk;
}


TStack* updateStack(TStack *stk, char *name, char *def,
                    char *DoB, char *DoD) {
    TStack *cur = stk;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            strncpy(cur->definition, def, sizeof(cur->definition) - 1);
            strncpy(cur->DoB,        DoB, sizeof(cur->DoB)        - 1);
            strncpy(cur->DoD,        DoD, sizeof(cur->DoD)        - 1);
            cur->definition[sizeof(cur->definition)-1] = '\0';
            cur->DoB[sizeof(cur->DoB)-1] = '\0';
            cur->DoD[sizeof(cur->DoD)-1] = '\0';
            printf("  '%s' updated successfully.\n", name);
            return stk;
        }
        cur = cur->next;
    }
    printf("  '%s' not found in stack.\n", name);
    return stk;
}

char* getSmallest(TStack *stk) {
    if (!stk) return NULL;
    TStack *best = stk, *cur = stk->next;
    while (cur) {
        if (wordCount(cur->definition) < wordCount(best->definition))
            best = cur;
        cur = cur->next;
    }
    return best->name;
}

void continuousSearch(TStack *stk) {
    printf("\n── Overlapping events ──\n");
    int found = 0;
    for (TStack *a = stk; a; a = a->next) {
        int aStart = extractYear(a->DoB);
        int aEnd   = extractYear(a->DoD);
        if (aStart == 0 || aEnd == 0 || strcmp(a->DoD, "N/A") == 0) continue;
        for (TStack *b = a->next; b; b = b->next) {
            int bStart = extractYear(b->DoB);
            int bEnd   = extractYear(b->DoD);
            if (bStart == 0 || bEnd == 0 || strcmp(b->DoD, "N/A") == 0) continue;
            if (aStart <= bEnd && bStart <= aEnd) {
                printf("  [OVERLAP] '%s' (%d–%d)  <->  '%s' (%d–%d)\n",
                       a->name, aStart, aEnd, b->name, bStart, bEnd);
                found = 1;
            }
        }
    }
    if (!found) printf("  No overlapping events found.\n");
}

bool isPersonalityKilled(char *word) {
    if (!word) return false;
    char lower[2048];
    strncpy(lower, word, sizeof(lower) - 1);
    lower[sizeof(lower)-1] = '\0';
    for (int i = 0; lower[i]; i++)
        if (lower[i] >= 'A' && lower[i] <= 'Z') lower[i] += 32;
    return (strstr(lower, "killed")      != NULL ||
            strstr(lower, "executed")    != NULL ||
            strstr(lower, "assassinated")!= NULL ||
            strstr(lower, "detonated")   != NULL);
}

static TStack* appendBottom(TStack *top, TStack *node) {
    if (!top) { node->next = NULL; return node; }
    top->next = appendBottom(top->next, node);
    return top;
}

TStack* recRevStack(TStack *stk) {
    if (!stk || !stk->next) return stk;
    TStack *rest    = recRevStack(stk->next);
    stk->next       = NULL;
    return appendBottom(rest, stk);
}

static void clearInputStk(void) { int c; while ((c = getchar()) != '\n' && c != EOF); }

static void printQueueStk(TQueue *q) {
    if (!q) { printf("  (empty)\n"); return; }
    int i = 0;
    while (q) {
        printf("  [%d] %s", i++, q->name);
        if (q->definition[0]) printf("  |  %s", q->definition);
        printf("\n");
        q = q->next;
    }
}

static void freeQueueStk(TQueue *q) { while (q) { TQueue *t = q->next; free(q); q = t; } }
static void freeStackStk(TStack *s) { while (s) { TStack *t = s->next; free(s); s = t; } }


void runStacks(TList *merged) {
    TStack *stk = toStack(merged);
    int choice;
    do {
        printf("\n+==========================================+\n");
        printf("|              STACK MANAGER               |\n");
        printf("+==========================================+\n");
        printf("|  1.  Display stack                       |\n");
        printf("|  2.  Search personality by name          |\n");
        printf("|  3.  Sort stack alphabetically           |\n");
        printf("|  4.  Sort by definition word count       |\n");
        printf("|  5.  Split short / long events           |\n");
        printf("|  6.  Add a personality                   |\n");
        printf("|  7.  Delete a personality                |\n");
        printf("|  8.  Update a personality                |\n");
        printf("|  9.  Get shortest definition             |\n");
        printf("|  10. Continuous (overlapping) events     |\n");
        printf("|  11. Check if personality was killed     |\n");
        printf("|  12. Reverse stack (recursive)           |\n");
        printf("|  13. Convert stack to sorted queue       |\n");
        printf("|  14. Convert stack to bidirectional list |\n");
        printf("|  0.  Back to main menu                   |\n");
        printf("+==========================================+\n");
        printf("  Choice: ");
        scanf("%d", &choice);
        clearInputStk();

        switch (choice) {
            case 1:
                printStack(stk);
                break;

            case 2: {
                char name[150];
                printf("  Name to search: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0';
                TStack *res = getInfoPersonality(stk, name);
                freeStackStk(res);
                break;
            }

            case 3:
                stk = sortNameStack(stk);
                printf("  Stack sorted alphabetically.\n");
                printStack(stk);
                break;

            case 4:
                stk = definitionStack(stk);
                printf("  Stack sorted by definition word count.\n");
                printStack(stk);
                break;

            case 5: {
                TStack *longStk = NULL;
                TStack *shortStk = pronunciationStack(stk, &longStk);
                printf("\n  ── Short events ──\n");
                printStack(shortStk);
                printf("\n  ── Long events ──\n");
                printStack(longStk);
                /* rebuild: join both halves back */
                stk = shortStk;
                if (!stk) { stk = longStk; }
                else {
                    TStack *tail = stk;
                    while (tail->next) tail = tail->next;
                    tail->next = longStk;
                }
                break;
            }

            case 6: {
                char name[150], def[2048], dob[30], dod[30];
                printf("  Name       : "); fgets(name, sizeof(name), stdin); name[strcspn(name,"\n")]='\0';
                printf("  Definition : "); fgets(def,  sizeof(def),  stdin); def[strcspn(def,"\n")]='\0';
                printf("  DoB (DD/MM/YYYY): "); scanf("%s", dob); clearInputStk();
                printf("  DoD (DD/MM/YYYY): "); scanf("%s", dod); clearInputStk();
                stk = addNameStack(stk, name, def, dob, dod);
                printf("  '%s' added to stack.\n", name);
                break;
            }

            case 7: {
                char name[150];
                printf("  Name to delete: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0';
                stk = deleteName(stk, name);
                break;
            }

            case 8: {
                char name[150], def[2048], dob[30], dod[30];
                printf("  Name to update : "); fgets(name, sizeof(name), stdin); name[strcspn(name,"\n")]='\0';
                printf("  New definition : "); fgets(def,  sizeof(def),  stdin); def[strcspn(def,"\n")]='\0';
                printf("  New DoB        : "); scanf("%s", dob); clearInputStk();
                printf("  New DoD        : "); scanf("%s", dod); clearInputStk();
                stk = updateStack(stk, name, def, dob, dod);
                break;
            }

            case 9: {
                char *s = getSmallest(stk);
                if (s) printf("  Personality with shortest definition: %s\n", s);
                break;
            }

            case 10:
                continuousSearch(stk);
                break;

            case 11: {
                char name[150];
                printf("  Enter personality name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0';
                TStack *cur = stk;
                while (cur && strcmp(cur->name, name) != 0) cur = cur->next;
                if (cur) {
                    bool killed = isPersonalityKilled(cur->definition);
                    printf("  '%s' was %skilled.\n", name, killed ? "" : "NOT ");
                } else {
                    printf("  Personality not found.\n");
                }
                break;
            }

            case 12:
                stk = recRevStack(stk);
                printf("  Stack reversed.\n");
                printStack(stk);
                break;

            case 13: {
                TQueue *q = stackToQueue(stk);
                printf("\n  ── Sorted Queue from Stack ──\n");
                printQueueStk(q);
                freeQueueStk(q);
                break;
            }

            case 14: {
                TList *list = stackToList(stk);
                printf("\n  ── Bidirectional Sorted List from Stack ──\n");
                TList *cur = list;
                while (cur) {
                    printf("  Name: %s  |  DoB: %s  |  DoD: %s\n  Def : %s\n  ─────\n",
                           cur->name, cur->DoB, cur->DoD, cur->definition);
                    cur = cur->next;
                }
                
                cur = list;
                while (cur) { TList *t = cur->next; free(cur); cur = t; }
                break;
            }

            case 0:
                printf("  Returning to main menu.\n");
                break;

            default:
                printf("  Invalid choice.\n");
        }
    } while (choice != 0);

    freeStackStk(stk);
}
