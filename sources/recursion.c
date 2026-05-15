#include<stdio.h>
#include "../headers/recursion.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


int countOccurence(File *f, char *name) {
    char line[2048];
    int count = 0;

    if (fgets(line, sizeof(line), f) == NULL)
        return 0;

    if (strstr(line, name) != NULL)
        count = 1;

    return count + countOccurence(f, name);
}

File* removeOccurence(File *f, char *word) {
    char line[2048];
    File *temp = fopen("temp.txt", "w");

    if (f == NULL || temp == NULL)
        return NULL;

    rewind(f);

    if (fgets(line, sizeof(line), f) == NULL) {
        fclose(temp);
        return f;
    }

    if (strstr(line, word) == NULL)
        fputs(line, temp);

    removeOccurence(f, word);

    fclose(temp);
    return f;
}

File* replaceOccurence(File *f, char *name, char *DoB, char *DoD) {
    char line[2048];
    File *temp = fopen("temp.txt", "w");

    if (f == NULL || temp == NULL)
        return NULL;

    rewind(f);

    if (fgets(line, sizeof(line), f) == NULL) {
        fclose(temp);
        return f;
    }

    if (strstr(line, name) != NULL)
        fprintf(temp, "%s - %s - %s\n", name, DoB, DoD);
    else
        fputs(line, temp);

    replaceOccurence(f, name, DoB, DoD);

    fclose(temp);
    return f;
}

void swap(char *a, char *b) {
    char temp = *a;
    *a = *b;
    *b = temp;
}

void permute(char *name, int l, int r) {
    if (l == r) {
        printf("%s\n", name);
        return;
    }

    for (int i = l; i <= r; i++) {
        swap(&name[l], &name[i]);
        permute(name, l + 1, r);
        swap(&name[l], &name[i]);
    }
}

void namePermutation(char *name) {
    if (name == NULL || strlen(name) == 0)
        return;
    permute(name, 0, (int)strlen(name) - 1);
}

void subseqHelper(char *word, int index, char *current) {
    if (word[index] == '\0') {
        if (strlen(current) > 0)
            printf("%s\n", current);
        return;
    }

    subseqHelper(word, index + 1, current);

    int len = (int)strlen(current);
    current[len] = word[index];
    current[len + 1] = '\0';

    subseqHelper(word, index + 1, current);

    current[len] = '\0';
}

void subseqName(char *word) {
    char current[1000] = "";
    if (word == NULL)
        return;
    subseqHelper(word, 0, current);
}

void longestSubyear(char *date1, char *date2) {
    if (date1 == NULL || date2 == NULL)
        return;

    if (strcmp(date1, date2) <= 0)
        printf("Events overlapping with %s and %s\n", date1, date2);
    else
        printf("Events overlapping with %s and %s\n", date2, date1);
}

int distinctSubseqWord(char *event) {
    if (event == NULL || *event == '\0')
        return 1;

    return 2 * distinctSubseqWord(event + 1);
}


bool isPalindromeWord(char *event, int l, int r) {
    if (event == NULL || strlen(event) == 0)
        return false;

    if (l >= r)
        return true;

    if (event[l] != event[r])
        return false;

    return isPalindromeWord(event, l + 1, r - 1);
}
