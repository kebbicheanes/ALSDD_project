#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../headers/types.h"
#include "../headers/recursion.h"

int countOccurence(FILE *f, char *name) {
    char line[2048];
    if (fgets(line, sizeof(line), f) == NULL) return 0;
    int count = (strstr(line, name) != NULL) ? 1 : 0;
    return count + countOccurence(f, name);
}

FILE* removeOccurence(FILE *f, char *word) {
    char line[2048];
    FILE *temp = fopen("temp.txt", "w");
    if (!f || !temp) return f;
    rewind(f);
    if (fgets(line, sizeof(line), f) == NULL) { fclose(temp); return f; }
    if (strstr(line, word) == NULL) fputs(line, temp);
    removeOccurence(f, word);
    fclose(temp);
    return f;
}

FILE* replaceOccurence(FILE *f, char *name, char *DoB, char *DoD) {
    char line[2048];
    FILE *temp = fopen("temp.txt", "w");
    if (!f || !temp) return f;
    rewind(f);
    if (fgets(line, sizeof(line), f) == NULL) { fclose(temp); return f; }
    if (strstr(line, name) != NULL)
        fprintf(temp, "%s - %s - %s\n", name, DoB, DoD);
    else
        fputs(line, temp);
    replaceOccurence(f, name, DoB, DoD);
    fclose(temp);
    return f;
}

static void swap(char *a, char *b) { char t = *a; *a = *b; *b = t; }

static void permute(char *name, int l, int r) {
    if (l == r) { printf("%s\n", name); return; }
    for (int i = l; i <= r; i++) {
        swap(&name[l], &name[i]);
        permute(name, l + 1, r);
        swap(&name[l], &name[i]);
    }
}

void namePermutation(char *name) {
    if (!name || !name[0]) return;
    permute(name, 0, (int)strlen(name) - 1);
}

static void subseqHelper(char *word, int index, char *current) {
    if (word[index] == '\0') {
        if (strlen(current) > 0) printf("%s\n", current);
        return;
    }
    subseqHelper(word, index + 1, current);
    int len = (int)strlen(current);
    current[len]     = word[index];
    current[len + 1] = '\0';
    subseqHelper(word, index + 1, current);
    current[len] = '\0';
}

void subseqName(char *word) {
    char current[1000] = "";
    if (!word) return;
    subseqHelper(word, 0, current);
}

void longestSubyear(char *date1, char *date2) {
    if (!date1 || !date2) return;
    if (strcmp(date1, date2) <= 0)
        printf("  Events overlapping with %s and %s\n", date1, date2);
    else
        printf("  Events overlapping with %s and %s\n", date2, date1);
}

int distinctSubseqWord(char *event) {
    if (!event || !*event) return 1;
    return 2 * distinctSubseqWord(event + 1);
}

bool isPalindromeWord(char *event) {
    int len = (int)strlen(event);
    if (len <= 1) return true;
    if (event[0] == ' ') return isPalindromeWord(event + 1);
    if (event[len - 1] == ' ') {
        char tmp[512];
        strncpy(tmp, event, len - 1);
        tmp[len - 1] = '\0';
        return isPalindromeWord(tmp);
    }
    if (event[0] != event[len - 1]) return false;
    char inner[512];
    strncpy(inner, event + 1, len - 2);
    inner[len - 2] = '\0';
    return isPalindromeWord(inner);
}

static void clearIn(void) { int c; while ((c = getchar()) != '\n' && c != EOF); }

void runRecursion(FILE *f) {
    int choice;
    do {
        printf("\n+==========================================+\n");
        printf("|            RECURSION MODULES             |\n");
        printf("+==========================================+\n");
        printf("|  1.  Count occurrences of a name         |\n");
        printf("|  2.  Remove all occurrences of a name    |\n");
        printf("|  3.  Replace occurrences (update dates)  |\n");
        printf("|  4.  Print all permutations of a name    |\n");
        printf("|  5.  Generate all subsequences           |\n");
        printf("|  6.  Print overlapping date range        |\n");
        printf("|  7.  Count distinct subsequences         |\n");
        printf("|  8.  Check if a string is a palindrome   |\n");
        printf("|  0.  Back to main menu                   |\n");
        printf("+==========================================+\n");
        printf("  Choice: ");
        scanf("%d", &choice); clearIn();

        switch (choice) {
            case 1: {
                char name[150];
                printf("  Name to count: "); fgets(name, sizeof(name), stdin); name[strcspn(name,"\n")]='\0';
                rewind(f);
                printf("  '%s' found %d time(s).\n", name, countOccurence(f, name));
                break;
            }
            case 2: {
                char word[150];
                printf("  Name/word to remove: "); fgets(word, sizeof(word), stdin); word[strcspn(word,"\n")]='\0';
                f = removeOccurence(f, word);
                printf("  Done.\n"); break;
            }
            case 3: {
                char name[150], dob[30], dod[30];
                printf("  Name    : "); fgets(name, sizeof(name), stdin); name[strcspn(name,"\n")]='\0';
                printf("  New DoB : "); scanf("%s", dob); clearIn();
                printf("  New DoD : "); scanf("%s", dod); clearIn();
                f = replaceOccurence(f, name, dob, dod);
                printf("  Done.\n"); break;
            }
            case 4: {
                char name[150];
                printf("  Name: "); fgets(name, sizeof(name), stdin); name[strcspn(name,"\n")]='\0';
                namePermutation(name); break;
            }
            case 5: {
                char word[150];
                printf("  Word: "); fgets(word, sizeof(word), stdin); word[strcspn(word,"\n")]='\0';
                subseqName(word); break;
            }
            case 6: {
                char d1[30], d2[30];
                printf("  Start date: "); scanf("%s", d1); clearIn();
                printf("  End   date: "); scanf("%s", d2); clearIn();
                longestSubyear(d1, d2); break;
            }
            case 7: {
                char event[512];
                printf("  String: "); fgets(event, sizeof(event), stdin); event[strcspn(event,"\n")]='\0';
                printf("  Distinct subsequences: %d\n", distinctSubseqWord(event)); break;
            }
            case 8: {
                char event[512];
                printf("  String: "); fgets(event, sizeof(event), stdin); event[strcspn(event,"\n")]='\0';
                printf("  '%s' is %sa palindrome.\n", event, isPalindromeWord(event) ? "" : "NOT "); break;
            }
            case 0: printf("  Back to main menu.\n"); break;
            default: printf("  Invalid choice.\n");
        }
    } while (choice != 0);
}
