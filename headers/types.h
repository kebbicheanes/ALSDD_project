#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int day;
    int month;
    int year;
} date;

typedef struct TList {
    char name[150];
    char DoB[30];
    char DoD[30];
    char definition[2048];
    struct TList *next;
    struct TList *prev;
} TList;

typedef struct TQueue {
    char name[150];
    char DoB[30];
    char DoD[30];
    char definition[2048];
    struct TQueue *next;
} TQueue;

typedef struct TStack {
    char name[150];
    char DoB[30];
    char DoD[30];
    char definition[2048];
    struct TStack *next;
} TStack;

typedef struct TTree {
    char name[150];
    char DoB[30];
    char DoD[30];
    char definition[2048];
    struct TTree *left;
    struct TTree *right;
} TTree;

#endif 

