#include <stdbool.h>
#ifndef RECURSION_H
#define RECURSION_H

typedef File File;

int countOccurence(File *f, char *name);
File* removeOccurence(File *f, char *word);
File* replaceOccurence(File *f, char *name, char *DoB, char *DoD);
void namePermutation(char *name);
void subseqName(char *word);
void longestSubyear(char *date1, char *date2);
int distinctSubseqWord(char *event);
bool isPalindromeWord(char *event);

#endif
