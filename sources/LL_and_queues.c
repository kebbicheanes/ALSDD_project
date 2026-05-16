#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/types.h"
#include "../headers/LL_and_queues.h"

static void swapData(TList *a, TList *b) {
    char tmp[2048];
    strcpy(tmp, a->name);       strcpy(a->name,       b->name);       strcpy(b->name,       tmp);
    strcpy(tmp, a->definition); strcpy(a->definition, b->definition); strcpy(b->definition, tmp);
    strcpy(tmp, a->DoB);        strcpy(a->DoB,        b->DoB);        strcpy(b->DoB,        tmp);
    strcpy(tmp, a->DoD);        strcpy(a->DoD,        b->DoD);        strcpy(b->DoD,        tmp);
}


TList* getPersonality(FILE *f) {
    TList *head = NULL, *last = NULL;
    char line[2048];
    rewind(f);

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        line[strcspn(line, "\r")] = '\0';
        if (strlen(line) < 3) continue;

        char *sep = strchr(line, '=');
        if (!sep) continue;

        TList *node = (TList*)malloc(sizeof(TList));
        if (!node) continue;
        memset(node, 0, sizeof(TList));

        char *braceStart = strchr(line, '{');
        char *braceEnd   = strchr(line, '}');

        if (braceStart && braceEnd && braceStart < sep) {
            int nLen = (int)(braceStart - line);
            strncpy(node->name, line, nLen);
            node->name[nLen] = '\0';
            int nl = (int)strlen(node->name);
            while (nl > 0 && (node->name[nl-1] == ' ' || node->name[nl-1] == '\t'))
                node->name[--nl] = '\0';

            char dates[64] = {0};
            int dLen = (int)(braceEnd - braceStart - 1);
            if (dLen > 0 && dLen < 64) {
                strncpy(dates, braceStart + 1, dLen);
                dates[dLen] = '\0';
            }
            char *dash = strchr(dates, '-');
            if (dash) {
                strncpy(node->DoB, dates, (int)(dash - dates));
                node->DoB[(int)(dash - dates)] = '\0';
                strcpy(node->DoD, dash + 1);
            } else {
                strcpy(node->DoB, dates);
                strcpy(node->DoD, "N/A");
            }
        } else {
            int nLen = (int)(sep - line);
            strncpy(node->name, line, nLen);
            node->name[nLen] = '\0';
            strcpy(node->DoB, "N/A");
            strcpy(node->DoD, "N/A");
        }

        strcpy(node->definition, sep + 1);
        node->next = NULL;
        node->prev = NULL;

        if (!head) { head = node; last = node; }
        else       { last->next = node; node->prev = last; last = node; }
    }
    return head;
}


TList* getDatePersonality(FILE *f) {
    TList *head = NULL, *last = NULL;
    char line[2048];
    rewind(f);

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        line[strcspn(line, "\r")] = '\0';
        if (strlen(line) < 3) continue;

        char *braceStart = strchr(line, '{');
        char *braceEnd   = strchr(line, '}');
        if (!braceStart || !braceEnd) continue;

        TList *node = (TList*)malloc(sizeof(TList));
        if (!node) continue;
        memset(node, 0, sizeof(TList));

        int nLen = (int)(braceStart - line);
        strncpy(node->name, line, nLen);
        node->name[nLen] = '\0';
        int nl = (int)strlen(node->name);
        while (nl > 0 && (node->name[nl-1] == ' ' || node->name[nl-1] == '\t'))
            node->name[--nl] = '\0';

        char dates[64] = {0};
        int dLen = (int)(braceEnd - braceStart - 1);
        if (dLen > 0 && dLen < 64) {
            strncpy(dates, braceStart + 1, dLen);
            dates[dLen] = '\0';
        }
        char *dash = strchr(dates, '-');
        if (dash) {
            strncpy(node->DoB, dates, (int)(dash - dates));
            node->DoB[(int)(dash - dates)] = '\0';
            strcpy(node->DoD, dash + 1);
        } else {
            strcpy(node->DoB, dates);
            strcpy(node->DoD, "N/A");
        }

        strcpy(node->definition, "N/A");
        node->next = NULL;
        node->prev = NULL;

        if (!head) { head = node; last = node; }
        else       { last->next = node; last = node; }
    }
    return head;
}

void printList(TList *head) {
    if (!head) { printf("  (empty list)\n"); return; }
    TList *cur = head;
    while (cur) {
        printf("  Name : %s\n", cur->name);
        printf("  Birth: %s  |  Death: %s\n", cur->DoB, cur->DoD);
        if (cur->definition[0] && strcmp(cur->definition, "N/A") != 0)
            printf("  Def  : %s\n", cur->definition);
        printf("  -----------------------------------------\n");
        cur = cur->next;
    }
}

void getInfoByDates(TList *s, TList *DoB) {
    char targetDate[30];
    printf("  Enter DoB to search (e.g. 14/12/1922): ");
    scanf("%s", targetDate);
    while (getchar() != '\n');

    TList *cur = DoB;
    int found = 0;
    while (cur) {
        if (strcmp(cur->DoB, targetDate) == 0) {
            printf("  Found: %s  (DoB: %s)\n", cur->name, cur->DoB);
            TList *d = s;
            while (d) {
                if (strcmp(d->name, cur->name) == 0) {
                    printf("  Def : %s\n", d->definition);
                    break;
                }
                d = d->next;
            }
            found = 1;
        }
        cur = cur->next;
    }
    if (!found) printf("  No personality found with DoB: %s\n", targetDate);
}

void getInfoByDates2(TList *s, TList *DoD) {
    char targetDate[30];
    printf("  Enter DoD to search (e.g. 18/10/1970): ");
    scanf("%s", targetDate);
    while (getchar() != '\n');

    TList *cur = DoD;
    int found = 0;
    while (cur) {
        if (strcmp(cur->DoD, targetDate) == 0) {
            printf("  Found: %s  (DoD: %s)\n", cur->name, cur->DoD);
            TList *d = s;
            while (d) {
                if (strcmp(d->name, cur->name) == 0) {
                    printf("  Def : %s\n", d->definition);
                    break;
                }
                d = d->next;
            }
            found = 1;
        }
        cur = cur->next;
    }
    if (!found) printf("  No personality found with DoD: %s\n", targetDate);
}

TList* sortWord(TList *syn) {
    if (!syn || !syn->next) return syn;
    int swapped;
    TList *lptr = NULL;
    do {
        swapped = 0;
        TList *p = syn;
        while (p->next != lptr) {
            if (strcmp(p->name, p->next->name) > 0) { swapData(p, p->next); swapped = 1; }
            p = p->next;
        }
        lptr = p;
    } while (swapped);
    return syn;
}

TList* sortWord2(TList *syn) {
    if (!syn || !syn->next) return syn;
    int swapped;
    TList *lptr = NULL;
    do {
        swapped = 0;
        TList *p = syn;
        while (p->next != lptr) {
            if (strlen(p->name) > strlen(p->next->name)) { swapData(p, p->next); swapped = 1; }
            p = p->next;
        }
        lptr = p;
    } while (swapped);
    return syn;
}

TList* sortPersonality(TList *syn) {
    if (!syn || !syn->next) return syn;
    int swapped;
    TList *lptr = NULL;
    do {
        swapped = 0;
        TList *p = syn;
        while (p->next != lptr) {
            const char *s1 = strrchr(p->DoB, '/');
            const char *s2 = strrchr(p->next->DoB, '/');
            int y1 = s1 ? atoi(s1 + 1) : atoi(p->DoB);
            int y2 = s2 ? atoi(s2 + 1) : atoi(p->next->DoB);
            if (y1 > y2) { swapData(p, p->next); swapped = 1; }
            p = p->next;
        }
        lptr = p;
    } while (swapped);
    return syn;
}

TList* deletepersonality(FILE *f, TList *s, TList *a, char *name) {
    TList *cur = s, *prev = NULL;
    while (cur && strcmp(cur->name, name) != 0) { prev = cur; cur = cur->next; }
    if (cur) { if (!prev) s = cur->next; else prev->next = cur->next; free(cur); }

    cur = a; prev = NULL;
    while (cur && strcmp(cur->name, name) != 0) { prev = cur; cur = cur->next; }
    if (cur) { if (!prev) a = cur->next; else prev->next = cur->next; free(cur); }
    (void)a;

    FILE *tmp = fopen("temp.txt", "w");
    if (!tmp) return s;
    char line[2048];
    rewind(f);
    while (fgets(line, sizeof(line), f))
        if (!strstr(line, name)) fputs(line, tmp);
    fclose(f); fclose(tmp);
    remove("data.txt"); rename("temp.txt", "data.txt");
    return s;
}

TList* updatePersonality(FILE *f, TList *s, TList *a, char *name,
                         char *definition, char *DoB, char *DoD) {
    TList *cur = s;
    while (cur && strcmp(cur->name, name) != 0) cur = cur->next;
    if (cur) { strcpy(cur->definition, definition); strcpy(cur->DoB, DoB); strcpy(cur->DoD, DoD); }
    cur = a;
    while (cur && strcmp(cur->name, name) != 0) cur = cur->next;
    if (cur) { strcpy(cur->DoB, DoB); strcpy(cur->DoD, DoD); }

    FILE *tmp = fopen("temp.txt", "w");
    if (!tmp) return s;
    char line[2048];
    rewind(f);
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, name)) fprintf(tmp, "%s{%s-%s}=%s\n", name, DoB, DoD, definition);
        else fputs(line, tmp);
    }
    fclose(f); fclose(tmp);
    remove("data.txt"); rename("temp.txt", "data.txt");
    return s;
}

TList* similarPersonality(TList *s, char *word) {
    TList *res = NULL, *last = NULL;
    TList *cur = s;
    while (cur) {
        if (strstr(cur->DoB, word) || strstr(cur->DoD, word)) {
            TList *node = (TList*)malloc(sizeof(TList));
            *node = *cur; node->next = NULL; node->prev = NULL;
            if (!res) { res = node; last = node; } else { last->next = node; last = node; }
        }
        cur = cur->next;
    }
    return res;
}

TList* countPersonality(TList *s, date *prt) {
    char dateStr[30];
    sprintf(dateStr, "%02d/%02d/%d", prt->day, prt->month, prt->year);
    TList *res = NULL, *last = NULL;
    TList *cur = s;
    while (cur) {
        if (strstr(cur->DoB, dateStr) || strstr(cur->DoD, dateStr)) {
            TList *node = (TList*)malloc(sizeof(TList));
            *node = *cur; node->next = NULL; node->prev = NULL;
            if (!res) { res = node; last = node; } else { last->next = node; last = node; }
        }
        cur = cur->next;
    }
    return res;
}

TList* palindromeName(TList *s) {
    TList *res = NULL, *last = NULL;
    TList *cur = s;
    while (cur) {
        int len = (int)strlen(cur->name), pal = 1;
        for (int i = 0; i < len / 2; i++)
            if (cur->name[i] != cur->name[len - i - 1]) { pal = 0; break; }
        if (pal && len > 1) {
            TList *node = (TList*)malloc(sizeof(TList));
            *node = *cur; node->next = NULL; node->prev = NULL;
            if (!res) { res = node; last = node; } else { last->next = node; last = node; }
        }
        cur = cur->next;
    }
    return res;
}

TList* mergeNodes(TList *s, TList *a) {
    if (!s || !a) return NULL;
    TList *node = (TList*)malloc(sizeof(TList));
    strcpy(node->name, s->name);
    strcpy(node->definition, s->definition);
    strcpy(node->DoB, a->DoB);
    strcpy(node->DoD, a->DoD);
    node->next = NULL; node->prev = NULL;
    return node;
}

TList* merge2Nodes(TList *s, TList *a) {
    TList *node = mergeNodes(s, a);
    if (node) node->next = node;
    return node;
}

TList* addPersonality(TList *s, TList *a, char *name, char *DoB, char *DoD) {
    TList *node = (TList*)malloc(sizeof(TList));
    strcpy(node->name, name);
    strcpy(node->DoB, DoB);
    strcpy(node->DoD, DoD);
    strcpy(node->definition, "New Personality");
    node->next = s; node->prev = NULL;
    if (s) s->prev = node;
    FILE *fp = fopen("data.txt", "a");
    if (fp) { fprintf(fp, "%s{%s-%s}=New Personality\n", name, DoB, DoD); fclose(fp); }
    (void)a;
    return node;
}

TList* addEvents(TList *b, char *namEvente, char *date) {
    TList *node = (TList*)malloc(sizeof(TList));
    strcpy(node->name, namEvente);
    strcpy(node->DoB, date);
    strcpy(node->DoD, "N/A");
    strcpy(node->definition, "Event added");
    node->next = b; node->prev = NULL;
    FILE *fp = fopen("data.txt", "a");
    if (fp) { fprintf(fp, "%s {%s}: Event added\n", namEvente, date); fclose(fp); }
    return node;
}

TQueue* sName(TList *s) {
    TQueue *queue = NULL, *last = NULL;
    TList *cur = s;
    while (cur) {
        TQueue *q = (TQueue*)malloc(sizeof(TQueue));
        strcpy(q->name, cur->name);
        strcpy(q->definition, cur->definition);
        strcpy(q->DoB, cur->DoB);
        strcpy(q->DoD, cur->DoD);
        q->next = NULL;
        if (!queue) { queue = q; last = q; } else { last->next = q; last = q; }
        cur = cur->next;
    }
    return queue;
}

TQueue* ageP(TList *a) {
    TList *sorted = sortPersonality(a);
    TQueue *queue = NULL, *last = NULL;
    TList *cur = sorted;
    while (cur) {
        TQueue *q = (TQueue*)malloc(sizeof(TQueue));
        strcpy(q->name, cur->name);
        strcpy(q->DoB, cur->DoB);
        strcpy(q->DoD, cur->DoD);
        q->definition[0] = '\0';
        q->next = NULL;
        if (!queue) { queue = q; last = q; } else { last->next = q; last = q; }
        cur = cur->next;
    }
    return queue;
}

TQueue* toQueue(TList *merged) {
    TQueue *queue = NULL, *last = NULL;
    TList *cur = merged;
    while (cur) {
        TQueue *q = (TQueue*)malloc(sizeof(TQueue));
        strcpy(q->name, cur->name);
        strcpy(q->definition, cur->definition);
        strcpy(q->DoB, cur->DoB);
        strcpy(q->DoD, cur->DoD);
        q->next = NULL;
        if (!queue) { queue = q; last = q; } else { last->next = q; last = q; }
        cur = cur->next;
    }
    return queue;
}

static void freeList(TList *head) {
    while (head) { TList *t = head->next; free(head); head = t; }
}
static void freeQueue(TQueue *head) {
    while (head) { TQueue *t = head->next; free(head); head = t; }
}
static void printQueue(TQueue *q) {
    if (!q) { printf("  (empty)\n"); return; }
    while (q) {
        printf("  %-30s  DoB: %-14s  DoD: %s\n", q->name, q->DoB, q->DoD);
        q = q->next;
    }
}

void voidLibrary(void) { printf("  (no-op)\n"); }


void runLibrary(void) {
    FILE *f = fopen("data.txt", "r+");
    if (!f) { perror("Cannot open data.txt"); return; }

    TList *s = getPersonality(f);
    TList *a = getDatePersonality(f);

    int choice;
    do {
        printf("\n+==========================================+\n");
        printf("|         PERSONALITIES MANAGER            |\n");
        printf("+==========================================+\n");
        printf("|  1.  Display all personalities           |\n");
        printf("|  2.  Sort alphabetically by name         |\n");
        printf("|  3.  Sort by name length                 |\n");
        printf("|  4.  Sort by birth year                  |\n");
        printf("|  5.  Search by Date of Birth             |\n");
        printf("|  6.  Search by Date of Death             |\n");
        printf("|  7.  Find palindrome names               |\n");
        printf("|  8.  Find similar by date string         |\n");
        printf("|  9.  Find by exact date (dd mm yyyy)     |\n");
        printf("|  10. Add a personality                   |\n");
        printf("|  11. Add an event                        |\n");
        printf("|  12. Update a personality                |\n");
        printf("|  13. Delete a personality                |\n");
        printf("|  14. Merge first nodes (bidirectional)   |\n");
        printf("|  15. Name queue  (sName)                 |\n");
        printf("|  16. Age queue   (ageP)                  |\n");
        printf("|  17. Full queue  (toQueue)               |\n");
        printf("|  0.  Back to main menu                   |\n");
        printf("+==========================================+\n");
        printf("  Choice: ");
        scanf("%d", &choice);
        while (getchar() != '\n');

        switch (choice) {
            case 1: printf("\n--- All personalities ---\n"); printList(s); break;
            case 2: s = sortWord(s);        printf("\n--- Sorted alpha ---\n");        printList(s); break;
            case 3: s = sortWord2(s);       printf("\n--- Sorted by length ---\n");    printList(s); break;
            case 4: a = sortPersonality(a); printf("\n--- Sorted by birth year ---\n"); printList(a); break;
            case 5: getInfoByDates(s, a);  break;
            case 6: getInfoByDates2(s, a); break;
            case 7: { TList *p = palindromeName(s); printf("\n--- Palindromes ---\n"); printList(p); freeList(p); break; }
            case 8: {
                char word[50];
                printf("  Date string: "); scanf("%s", word); while(getchar()!='\n');
                TList *sim = similarPersonality(a, word);
                printf("\n--- Similar '%s' ---\n", word); printList(sim); freeList(sim); break;
            }
            case 9: {
                date d;
                printf("  Day month year: "); scanf("%d %d %d", &d.day, &d.month, &d.year); while(getchar()!='\n');
                TList *cnt = countPersonality(a, &d);
                printf("\n--- Matching %02d/%02d/%d ---\n", d.day, d.month, d.year); printList(cnt); freeList(cnt); break;
            }
            case 10: {
                char name[100], dob[20], dod[20];
                printf("  Name: "); fgets(name, sizeof(name), stdin); name[strcspn(name,"\n")]='\0';
                printf("  DoB (DD/MM/YYYY): "); scanf("%s", dob);
                printf("  DoD (DD/MM/YYYY): "); scanf("%s", dod); while(getchar()!='\n');
                s = addPersonality(s, a, name, dob, dod);
                printf("  '%s' added.\n", name); break;
            }
            case 11: {
                char evName[100], evDate[20];
                printf("  Event name: "); fgets(evName, sizeof(evName), stdin); evName[strcspn(evName,"\n")]='\0';
                printf("  Date: "); scanf("%s", evDate); while(getchar()!='\n');
                a = addEvents(a, evName, evDate);
                printf("  Event added.\n"); break;
            }
            case 12: {
                char name[100], def[1024], dob[20], dod[20];
                printf("  Name: "); fgets(name, sizeof(name), stdin); name[strcspn(name,"\n")]='\0';
                printf("  New definition: "); fgets(def, sizeof(def), stdin); def[strcspn(def,"\n")]='\0';
                printf("  New DoB: "); scanf("%s", dob);
                printf("  New DoD: "); scanf("%s", dod); while(getchar()!='\n');
                s = updatePersonality(f, s, a, name, def, dob, dod);
                printf("  Updated.\n"); break;
            }
            case 13: {
                char name[100];
                printf("  Name to delete: "); fgets(name, sizeof(name), stdin); name[strcspn(name,"\n")]='\0';
                s = deletepersonality(f, s, a, name);
                printf("  Deleted.\n"); break;
            }
            case 14: { TList *m = mergeNodes(s, a); printf("\n--- Merged ---\n"); printList(m); freeList(m); break; }
            case 15: { TQueue *q = sName(s); printf("\n--- Name queue ---\n"); printQueue(q); freeQueue(q); break; }
            case 16: { TQueue *q = ageP(a);  printf("\n--- Age queue ---\n");  printQueue(q); freeQueue(q); break; }
            case 17: { TQueue *q = toQueue(s); printf("\n--- Full queue ---\n"); printQueue(q); freeQueue(q); break; }
            case 0:  printf("  Back to main menu.\n"); break;
            default: printf("  Invalid choice.\n");
        }
    } while (choice != 0);

    freeList(s);
    freeList(a);
    fclose(f);
}
