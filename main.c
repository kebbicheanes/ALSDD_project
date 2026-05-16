#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/LL_and_queues.h"
#include "headers/stacks.h"
#include "headers/BST.h"
#include "headers/recursion.h"

int main(void) {

    FILE *f = fopen("data.txt", "r+");
    if (!f) { perror("Cannot open data.txt"); return EXIT_FAILURE; }

    TList *s = getPersonality(f);
    TList *a = getDatePersonality(f);

    TList *merged = NULL, *tail = NULL;
    TList *cs = s, *ca = a;
    while (cs && ca) {
        TList *node = (TList*)malloc(sizeof(TList));
        strcpy(node->name,       cs->name);
        strcpy(node->definition, cs->definition);
        strcpy(node->DoB,        ca->DoB);
        strcpy(node->DoD,        ca->DoD);
        node->next = NULL;
        node->prev = NULL;
        if (!merged) { merged = node; tail = node; }
        else         { tail->next = node; node->prev = tail; tail = node; }
        cs = cs->next;
        ca = ca->next;
    }

    int choice;
    do {
        printf("\n+==========================================+\n");
        printf("|   HISTORY OF ALGERIA DATABASE MANAGER    |\n");
        printf("+==========================================+\n");
        printf("|  1.  Linked Lists & Queues               |\n");
        printf("|  2.  Stacks                              |\n");
        printf("|  3.  Binary Search Tree (BST)            |\n");
        printf("|  4.  Recursion                           |\n");
        printf("|  0.  Exit                                |\n");
        printf("+==========================================+\n");
        printf("  Choice: ");
        scanf("%d", &choice);
        while (getchar() != '\n');

        switch (choice) {
            case 1: runLibrary();        break;
            case 2: runStacks(merged);   break;
            case 3: runBST(f);           break;
            case 4: runRecursion(f);     break;
            case 0: printf("  Goodbye!\n"); break;
            default: printf("  Invalid choice.\n");
        }
    } while (choice != 0);

    TList *cur;
    cur = s;      while (cur) { TList *t = cur->next; free(cur); cur = t; }
    cur = a;      while (cur) { TList *t = cur->next; free(cur); cur = t; }
    cur = merged; while (cur) { TList *t = cur->next; free(cur); cur = t; }
    fclose(f);
    return EXIT_SUCCESS;
}

