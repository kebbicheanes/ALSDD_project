#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../headers/LL_and_queues.h"

TList* getPersonality(File *f){
    TList *head = NULL;
    TList *last = NULL;
    char line[2048];
    rewind(f);


 while(fgets(line , sizeof(line),f)){
    line[strcspn(line, "\n")] = 0;
    if(strlen(line)< 3) continue;

    char *sep = strchr(line, '=');
if(sep){
    TList *newnode = (TList*)malloc(sizeof(TList));
    if(!newnode) continue;
               
        strncpy(newnode->name, line, sep - line);
        newnode->name[sep - line] = '\0'; 
        strcpy(newnode->definition, sep + 1);

        newnode->next = NULL; 
        if(head == NULL) {
            head = newnode;  
            last = head;
        } else {
            last->next = newnode; 
            last = newnode;      
        }
    }

 }
return head;
};
void printList(TList *head) {
    TList *current = head;
    while (current != NULL) {
        printf("Name: %s\n", current->name);
        printf("  Birth: %s | Death: %s\n", current->DoB, current->DoD);
        printf("  Def: %s\n", current->definition);
        printf("--------------------------\n");
        current = current->next;
    }
}
TList* getDatePersonality(File *f){
    TList *head = NULL;
    TList *last = NULL;
    char line[2048];
    rewind(f);

 while(fgets(line , sizeof(line),f)){
    line[strcspn(line, "\n")] = 0;
    if(strlen(line)< 3) continue;

    char *braceStart = strchr(line, '{');
    char *braceEnd = strchr(line, '{');

    if(braceStart && braceEnd){
         TList *newnode = (TList*)malloc(sizeof(TList));
            if(!newnode) continue;

            strncpy(newnode->name , line ,braceStart - line);
            newnode->name[braceStart - line]='\0';

            char dates[64];
            int len = braceEnd - (braceStart +1);
            strncpy(dates, braceStart + 1, len);
            dates[len] = '\0';

              char *dash = strchr(dates, '-');
            if (dash) {
                strncpy(newnode->DoB, dates, dash - dates);
                newnode->DoB[dash - dates] = '\0';
                strcpy(newnode->DoD, dash + 1);
            } else {
                strcpy(newnode->DoB, dates);
                strcpy(newnode->DoD, "N/A");
            }
           
            strcpy(newnode->definition, "N/A");
             newnode->next = NULL;
            if(head == NULL) { head = newnode; last = head; }
            else { last->next = newnode; last = newnode; }
        }
    }
    return head;
}
void getInfoByDates(TList *s, TList *DoB){
     char targetDate[20];
    printf("Enter the Date of Birth to search (e.g., 14/12/1922): ");
    scanf("%s", targetDate);

    TList *current = DoB; 
    int found = 0;

    while (current != NULL) {
    
        if (strcmp(current->DoB, targetDate) == 0) {
            printf("Found Personality:\n");
            printf("Name: %s\n", current->name);
            
            TList *searchDef = s;
            while(searchDef != NULL) {
                if(strcmp(searchDef->name, current->name) == 0) {
                    printf("Definition: %s\n", searchDef->definition);
                    break;
                }
                searchDef = searchDef->next;
            }
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("No personality found with date of birth: %s\n", targetDate);
    }
}
void getInfoByDates2(TList *s, TList *DoD){
     char targetDate[20];
    printf("Enter the Date of Death to search");
    scanf("%s", targetDate);

    TList *current = DoD; 
    int found = 0;

      while (current != NULL) {
    
        if (strcmp(current->DoD, targetDate) == 0) {
            printf("Found Personality:\n");
            printf("Name: %s\n", current->name);
            
            TList *searchDef = s;
            while(searchDef != NULL) {
                if(strcmp(searchDef->name, current->name) == 0) {
                    printf("Definition: %s\n", searchDef->definition);
                    break;
                }
                searchDef = searchDef->next;
            }
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("No personality found with date of death: %s\n", targetDate);
    }


}
TList* sortWord(TList *syn){

     if (syn == NULL || syn->next == NULL) return syn;

     int swapped;
     TList *ptr1;
     TList *lptr = NULL;

      do {
        swapped = 0;
        ptr1 = syn;

         while (ptr1->next != lptr) {
            
            if (strcmp(ptr1->name, ptr1->next->name) > 0) {
                swapData(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
         }
        lptr = ptr1;
     } while (swapped);

     return syn;
}
void swapData(TList *a, TList *b) {
    char tempName[100], tempDef[1024], tempDoB[20], tempDoD[256];


    strcpy(tempName, a->name);
    strcpy(tempDef, a->definition);
    strcpy(tempDoB, a->DoB);
    strcpy(tempDoD, a->DoD);

    strcpy(a->name, b->name);
    strcpy(a->definition, b->definition);
    strcpy(a->DoB, b->DoB);
    strcpy(a->DoD, b->DoD);

    strcpy(b->name, tempName);
    strcpy(b->definition, tempDef);
    strcpy(b->DoB, tempDoB);
    strcpy(b->DoD, tempDoD);
}
TList* sortWord2(TList *syn) {
    if (syn == NULL || syn->next == NULL) return syn;

    int swapped;
    TList *ptr1;
    TList *lptr = NULL;

    do {
        swapped = 0;
        ptr1 = syn;

        while (ptr1->next != lptr) {
            
            if (strlen(ptr1->name) > strlen(ptr1->next->name)) {
                swapData(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);

    return syn;
}
TList* sortPersonality(TList *syn) {
    if (syn == NULL || syn->next == NULL) return syn;

    int swapped;
    TList *ptr1;
    TList *lptr = NULL;

    do {
        swapped = 0;
        ptr1 = syn;

        while (ptr1->next != lptr) {
            
            int year1 = atoi(ptr1->DoB + 6);
            int year2 = atoi(ptr1->next->DoB + 6);

            if (year1 < year2) { 
                swapData(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);

    return syn;
}
TList* deletepersonality(File *f, TList *s, TList *a, char *name) {
    TList *current = s, *prev = NULL;
    while (current != NULL && strcmp(current->name, name) != 0) {
        prev = current;
        current = current->next;
    }
    if (current != NULL) {
        if (prev == NULL) s = current->next;
        else prev->next = current->next;
        free(current);
    }

    current = a; prev = NULL;
    while (current != NULL && strcmp(current->name, name) != 0) {
        prev = current;
        current = current->next;
    }
    if (current != NULL) {
        if (prev == NULL) a = current->next;
        else prev->next = current->next;
        free(current);
    }

    FILE *temp = fopen("temp.txt", "w");
    char line[2048];
    rewind(f);
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, name) == NULL) {
            fputs(line, temp);
        }
    }
    fclose(f);
    fclose(temp);
    remove("data.txt");
    rename("temp.txt", "data.txt");
    return s;
}
TList* updatePersonality(File *f, TList *s, TList *a, char *name, char *definition, char *DoB, char *DoD) {
    TList *current = s;
    while (current != NULL && strcmp(current->name, name) != 0) current = current->next;
    if (current != NULL) {
        strcpy(current->definition, definition);
        strcpy(current->DoB, DoB);
        strcpy(current->DoD, DoD);
    }

    current = a;
    while (current != NULL && strcmp(current->name, name) != 0) current = current->next;
    if (current != NULL) {
        strcpy(current->DoB, DoB);
        strcpy(current->DoD, DoD);
    }

    FILE *temp = fopen("temp.txt", "w");
    char line[2048];
    rewind(f);
    while (fgets(line, sizeof(line), f)) {
        if (strstr(line, name) != NULL) {
            fprintf(temp, "%s{%s-%s}=%s\n", name, DoB, DoD, definition);
        } else {
            fputs(line, temp);
        }
    }
    fclose(f);
    fclose(temp);
    remove("data.txt");
    rename("temp.txt", "data.txt");
    return s;
}
TList* similarPersonality(TList *s, char *word) {
    TList *result = NULL, *last = NULL;
    TList *current = s;
    while (current != NULL) {
        if (strcmp(current->DoB, word) == 0 || strcmp(current->DoD, word) == 0) {
            TList *newNode = (TList*)malloc(sizeof(TList));
            *newNode = *current;
            newNode->next = NULL;
            if (result == NULL) { result = newNode; last = result; }
            else { last->next = newNode; last = newNode; }
        }
        current = current->next;
    }
    return result;
}
TList* countPersonality(TList *s, date *prt) {
    char dateStr[20];
    sprintf(dateStr, "%02d/%02d/%d", prt->day, prt->month, prt->year);
    TList *result = NULL, *last = NULL;
    TList *current = s;
    while (current != NULL) {
        if (strstr(current->DoB, dateStr) || strstr(current->DoD, dateStr)) {
            TList *newNode = (TList*)malloc(sizeof(TList));
            *newNode = *current;
            newNode->next = NULL;
            if (result == NULL) { result = newNode; last = result; }
            else { last->next = newNode; last = newNode; }
        }
        current = current->next;
    }
    return result;
}
TList* palindromeName(TList *s) {
    TList *result = NULL, *last = NULL;
    TList *current = s;
    while (current != NULL) {
        int len = strlen(current->name);
        int isPal = 1;
        for (int i = 0; i < len / 2; i++) {
            if (current->name[i] != current->name[len - i - 1]) {
                isPal = 0; break;
            }
        }
        if (isPal && len > 1) {
            TList *newNode = (TList*)malloc(sizeof(TList));
            *newNode = *current;
            newNode->next = NULL;
            if (result == NULL) { result = newNode; last = result; }
            else { last->next = newNode; last = newNode; }
        }
        current = current->next;
    }
    return result;
}
TList* mergeNodes(TList *s, TList *a) {
    if (!s || !a) return NULL;
    TList *newNode = (TList*)malloc(sizeof(TList));
    strcpy(newNode->name, s->name);
    strcpy(newNode->DoB, a->DoB);
    strcpy(newNode->DoD, a->DoD);
    strcpy(newNode->definition, s->definition);
    newNode->next = NULL;
    return newNode;
}
TList* merge2Nodes(TList *s, TList *a) {
    TList *newNode = mergeNodes(s, a);
    newNode->next = newNode; 
    return newNode;
}
TList* addPersonality(TList *s, TList *a, char *name, char *DoB, char *DoD) {
    
    TList *newNode = (TList*)malloc(sizeof(TList));
    strcpy(newNode->name, name);
    strcpy(newNode->DoB, DoB);
    strcpy(newNode->DoD, DoD);
    newNode->next = s;
    
    
    FILE *f = fopen("data.txt", "a");
    fprintf(f, "\n%s{%s-%s}=New Personality", name, DoB, DoD);
    fclose(f);
    return newNode;
}
TList* addEvents(TList *b, char *namEvente, char *date) {
    TList *newNode = (TList*)malloc(sizeof(TList));
    strcpy(newNode->name, namEvente);
    strcpy(newNode->DoB, date);
    newNode->next = b;

    FILE *f = fopen("data.txt", "a");
    fprintf(f, "\n%s {%s}: Event added", namEvente, date);
    fclose(f);
    return newNode;
}
TQueue* sName(TList *s) {
    TQueue *queue = NULL, *last = NULL;
    TList *current = s;
    while (current != NULL) {
        int wordCount = 1;
        for (int i = 0; current->name[i]; i++) if (current->name[i] == ' ') wordCount++;
        
        TQueue *newQ = (TQueue*)malloc(sizeof(TQueue));
        strcpy(newQ->name, current->name);
        newQ->next = NULL;
        
    
        if (!queue) { queue = newQ; last = queue; }
        else { last->next = newQ; last = newQ; }
        current = current->next;
    }
    return queue;
}
TQueue* ageP(TList *a) {
    TQueue *queue = NULL;
    
    TList *sorted = sortPersonality(a); 
    TList *current = sorted;
    
    TQueue *last = NULL;
    while (current != NULL) {
        TQueue *newQ = (TQueue*)malloc(sizeof(TQueue));
        strcpy(newQ->name, current->name);
        newQ->next = NULL;
        if (!queue) { queue = newQ; last = queue; }
        else { last->next = newQ; last = newQ; }
        current = current->next;
    }
    return queue;
}
TQueue* toQueue(TList *merged) {
    TQueue *queue = NULL, *last = NULL;
    TList *current = merged;
    while (current != NULL) {
        TQueue *newQ = (TQueue*)malloc(sizeof(TQueue));
        strcpy(newQ->name, current->name);
        strcpy(newQ->definition, current->definition);
        newQ->next = NULL;
        if (!queue) { queue = newQ; last = queue; }
        else { last->next = newQ; last = newQ; }
        current = current->next;
    }
    return queue;
}   
