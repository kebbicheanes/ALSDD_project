#include <stdbool.h>
#ifndef BTS_H
#define BTS_H

typedef struct TTree {
    char         name[100];
    char         definition[1024];
    char         DoB[20];
    char         DoD[256];
    struct TTree *left;
    struct TTree *right;
} TTree;

typedef struct TStack {
    char          name[100];
    char          definition[1024];
    char          DoB[20];   /* Date of Birth  */
    char          DoD[256];   /* Date of Death  */
    struct TStack *next;
} TStack;

typedef File File;

TTree* toTree(TStack *stk);
TTree* fillTree(FILE *f);
TTree* getInfoNameTree(TTree *tr, char *name);
TStack* addNameBST(TTree *tr, char *name, char *DoB, char *DoD);
TTree* deleteNameBST(TTree *tr, char *name);
TTree* updateNameBST(TTree *tr, char *name, char *s, char *DoB, char *DoD);
TTree* traversalBSTinOrder(TTree *tr); 
TTree* traversalBSTpreOrder(TTree *tr);
TTree* traversalBSTpostOrder(TTree *tr);
void heightSizeBST(TTree *tr);
TTree* lowestCommonAncestor(TTree *tr, char *word1, char *word2);
int countNodesRange(TTree *tr, int l, int h);
TTree* inOrderSuccessor(TTree *tr, char *word);
TTree* BSTMirror(TTree *tr);
bool isBalancedBST(TTree *tr);
TTree* BTSMerge(TTree *tr1, TTree *tr2);

#endif
