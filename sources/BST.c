#include <stdio.h>
#include <stdlib.h>
#include "../headers/types.h"
#include "../headers/BST.h"
#include "../headers/BST.h"       


static TTree* newTreeNode(const char *name, const char *def,
                          const char *dob,  const char *dod) {
    TTree *node = (TTree*)malloc(sizeof(TTree));
    if (!node) return NULL;
    strncpy(node->name,       name, sizeof(node->name)       - 1);
    strncpy(node->definition, def,  sizeof(node->definition) - 1);
    strncpy(node->DoB,        dob,  sizeof(node->DoB)        - 1);
    strncpy(node->DoD,        dod,  sizeof(node->DoD)        - 1);
    node->name[sizeof(node->name)-1]             = '\0';
    node->definition[sizeof(node->definition)-1] = '\0';
    node->DoB[sizeof(node->DoB)-1]               = '\0';
    node->DoD[sizeof(node->DoD)-1]               = '\0';
    node->left  = NULL;
    node->right = NULL;
    return node;
}

static TTree* bstInsert(TTree *root, const char *name, const char *def,
                        const char *dob, const char *dod) {
    if (!root) return newTreeNode(name, def, dob, dod);
    int cmp = strcmp(name, root->name);
    if      (cmp < 0) root->left  = bstInsert(root->left,  name, def, dob, dod);
    else if (cmp > 0) root->right = bstInsert(root->right, name, def, dob, dod);
    return root;
}


static int height(TTree *tr) {
    if (!tr) return 0;
    int l = height(tr->left), r = height(tr->right);
    return 1 + (l > r ? l : r);
}

static int size(TTree *tr) {
    if (!tr) return 0;
    return 1 + size(tr->left) + size(tr->right);
}

static void collectInOrder(TTree *tr, TTree **arr, int *idx) {
    if (!tr) return;
    collectInOrder(tr->left, arr, idx);
    arr[(*idx)++] = tr;
    collectInOrder(tr->right, arr, idx);
}

static TTree* buildBalanced(TTree **arr, int lo, int hi) {
    if (lo > hi) return NULL;
    int mid = (lo + hi) / 2;
    TTree *root  = arr[mid];
    root->left   = buildBalanced(arr, lo,    mid - 1);
    root->right  = buildBalanced(arr, mid + 1, hi);
    return root;
}

void freeTree(TTree *tr) {
    if (!tr) return;
    freeTree(tr->left);
    freeTree(tr->right);
    free(tr);
}

static TTree* findMin(TTree *tr) {
    while (tr && tr->left) tr = tr->left;
    return tr;
}

static int extractYear(const char *date) {
    if (!date || !*date) return 0;
    const char *slash2 = strrchr(date, '/');
    if (slash2) return atoi(slash2 + 1);
    return atoi(date);
}


void printTreeInOrder(TTree *tr) {
    if (!tr) return;
    printTreeInOrder(tr->left);
    printf("  Name: %-40s  DoB: %-12s  DoD: %-12s\n",
           tr->name, tr->DoB, tr->DoD);
    printf("  Def : %s\n  ─────────────────────────────────\n", tr->definition);
    printTreeInOrder(tr->right);
}

TTree* toTree(TStack *stk) {
    TTree *root = NULL;
    TStack *cur = stk;
    while (cur) {
        root = bstInsert(root, cur->name, cur->definition, cur->DoB, cur->DoD);
        cur  = cur->next;
    }
    return root;
}

TTree* fillTree(FILE *f) {
    TTree *root = NULL;
    char   line[2048];
    rewind(f);
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) < 3) continue;

        char *eq = strchr(line, '=');
        if (eq) {
            char name[150] = {0}, dob[30] = "N/A", dod[30] = "N/A";
            char *bs = strchr(line, '{');
            char *be = strchr(line, '}');
            if (bs && be && bs < eq) {
                strncpy(name, line, (int)(bs - line));
                name[(int)(bs - line)] = '\0';
                /* trim trailing space */
                int nl = strlen(name);
                while (nl > 0 && name[nl-1] == ' ') name[--nl] = '\0';
                char dates[64] = {0};
                int  dl = (int)(be - bs - 1);
                strncpy(dates, bs + 1, dl);
                char *dash = strchr(dates, '-');
                if (dash) {
                    strncpy(dob, dates, (int)(dash - dates));
                    dob[(int)(dash - dates)] = '\0';
                    strcpy(dod, dash + 1);
                } else {
                    strcpy(dob, dates);
                }
            } else {
                strncpy(name, line, (int)(eq - line));
                name[(int)(eq - line)] = '\0';
            }
            root = bstInsert(root, name, eq + 1, dob, dod);
            continue;
        }

        char *colon = strchr(line, ':');
        if (colon) {
            char name[150] = {0};
            char *bs = strchr(line, '{');
            if (bs && bs < colon) {
                strncpy(name, line, (int)(bs - line));
                name[(int)(bs - line)] = '\0';
            } else {
                strncpy(name, line, (int)(colon - line));
                name[(int)(colon - line)] = '\0';
            }
            int nl = strlen(name);
            while (nl > 0 && name[nl-1] == ' ') name[--nl] = '\0';
            root = bstInsert(root, name, colon + 1, "N/A", "N/A");
        }
    }
    return root;
}

TTree* getInfoNameTree(TTree *tr, char *name) {
    if (!tr) { printf("  '%s' not found in tree.\n", name); return NULL; }
    int cmp = strcmp(name, tr->name);
    if (cmp == 0) {
        printf("\n── Found in BST: %s ──\n", tr->name);
        printf("  DoB : %s\n  DoD : %s\n  Def : %s\n",
               tr->DoB, tr->DoD, tr->definition);
        return tr;
    }
    if (cmp < 0) return getInfoNameTree(tr->left,  name);
    else         return getInfoNameTree(tr->right, name);
}

TStack* addNameBST(TTree *tr, char *name, char *DoB, char *DoD) {
    tr = bstInsert(tr, name, "", DoB, DoD);  
    int n = size(tr);
    TTree **arr = (TTree**)malloc(n * sizeof(TTree*));
    int idx = 0;
    collectInOrder(tr, arr, &idx);
    TStack *stk = NULL;
    for (int i = n - 1; i >= 0; i--) {
        TStack *node = (TStack*)malloc(sizeof(TStack));
        strcpy(node->name,       arr[i]->name);
        strcpy(node->definition, arr[i]->definition);
        strcpy(node->DoB,        arr[i]->DoB);
        strcpy(node->DoD,        arr[i]->DoD);
        node->next = stk;
        stk = node;
    }
    free(arr);
    return stk;
}

TTree* deleteNameBST(TTree *tr, char *name) {
    if (!tr) { printf("  '%s' not found in tree.\n", name); return NULL; }
    int cmp = strcmp(name, tr->name);
    if (cmp < 0) {
        tr->left  = deleteNameBST(tr->left,  name);
    } else if (cmp > 0) {
        tr->right = deleteNameBST(tr->right, name);
    } else {
        if (!tr->left) {
            TTree *right = tr->right; free(tr); return right;
        }
        if (!tr->right) {
            TTree *left = tr->left; free(tr); return left;
        }
        TTree *succ = findMin(tr->right);
        strcpy(tr->name,       succ->name);
        strcpy(tr->definition, succ->definition);
        strcpy(tr->DoB,        succ->DoB);
        strcpy(tr->DoD,        succ->DoD);
        tr->right = deleteNameBST(tr->right, succ->name);
    }
    return tr;
}

TTree* updateNameBST(TTree *tr, char *name, char *s,
                     char *DoB, char *DoD) {
    TTree *node = getInfoNameTree(tr, name);
    if (node) {
        strncpy(node->definition, s,   sizeof(node->definition) - 1);
        strncpy(node->DoB,        DoB, sizeof(node->DoB)        - 1);
        strncpy(node->DoD,        DoD, sizeof(node->DoD)        - 1);
        node->definition[sizeof(node->definition)-1] = '\0';
        node->DoB[sizeof(node->DoB)-1] = '\0';
        node->DoD[sizeof(node->DoD)-1] = '\0';
        printf("  '%s' updated in tree.\n", name);
    }
    return tr;
}

TTree* traversalBSTinOrder(TTree *tr) {
    printf("\n── In-Order Traversal ──\n");
    printTreeInOrder(tr);
    return tr;
}

static void preOrderPrint(TTree *tr) {
    if (!tr) return;
    printf("  Name: %-40s  DoB: %-12s  DoD: %-12s\n  Def : %s\n  ─────\n",
           tr->name, tr->DoB, tr->DoD, tr->definition);
    preOrderPrint(tr->left);
    preOrderPrint(tr->right);
}

TTree* traversalBSTpreOrder(TTree *tr) {
    printf("\n── Pre-Order Traversal ──\n");
    preOrderPrint(tr);
    return tr;
}

static void postOrderPrint(TTree *tr) {
    if (!tr) return;
    postOrderPrint(tr->left);
    postOrderPrint(tr->right);
    printf("  Name: %-40s  DoB: %-12s  DoD: %-12s\n  Def : %s\n  ─────\n",
           tr->name, tr->DoB, tr->DoD, tr->definition);
}

TTree* traversalBSTpostOrder(TTree *tr) {
    printf("\n── Post-Order Traversal ──\n");
    postOrderPrint(tr);
    return tr;
}


void heightSizeBST(TTree *tr) {
    printf("\n── BST Stats ──\n");
    printf("  Height : %d\n", height(tr));
    printf("  Size   : %d\n", size(tr));
}

TTree* lowestCommonAncestor(TTree *tr, char *word1, char *word2) {
    if (!tr) return NULL;
    if (strcmp(word1, tr->name) < 0 && strcmp(word2, tr->name) < 0)
        return lowestCommonAncestor(tr->left,  word1, word2);
    if (strcmp(word1, tr->name) > 0 && strcmp(word2, tr->name) > 0)
        return lowestCommonAncestor(tr->right, word1, word2);
    /* This node straddles the two words – it is the LCA */
    printf("\n── Lowest Common Ancestor of '%s' and '%s' ──\n  %s\n",
           word1, word2, tr->name);
    return tr;
}

int countNodesRange(TTree *tr, int l, int h) {
    if (!tr) return 0;
    int year = extractYear(tr->DoB);
    int count = (year >= l && year <= h) ? 1 : 0;
    return count + countNodesRange(tr->left, l, h)
                 + countNodesRange(tr->right, l, h);
}

TTree* inOrderSuccessor(TTree *tr, char *word) {
    TTree *successor = NULL;
    TTree *cur       = tr;
    while (cur) {
        int cmp = strcmp(word, cur->name);
        if (cmp < 0) { successor = cur; cur = cur->left; }
        else if (cmp > 0) { cur = cur->right; }
        else {
            if (cur->right) successor = findMin(cur->right);
            break;
        }
    }
    if (successor)
        printf("\n── In-Order Successor of '%s' ──\n  %s\n",
               word, successor->name);
    else
        printf("\n  '%s' has no in-order successor.\n", word);
    return successor;
}

TTree* BSTMirror(TTree *tr) {
    if (!tr) return NULL;
    TTree *tmp   = tr->left;
    tr->left     = BSTMirror(tr->right);
    tr->right    = BSTMirror(tmp);
    return tr;
}

static bool isBalancedHelper(TTree *tr, int *h) {
    if (!tr) { *h = 0; return true; }
    int lh = 0, rh = 0;
    bool lb = isBalancedHelper(tr->left,  &lh);
    bool rb = isBalancedHelper(tr->right, &rh);
    *h = 1 + (lh > rh ? lh : rh);
    int diff = lh - rh;
    if (diff < 0) diff = -diff;
    return lb && rb && (diff <= 1);
}

bool isBalancedBST(TTree *tr) {
    int h = 0;
    bool result = isBalancedHelper(tr, &h);
    printf("\n── BST Balance Check ──\n  %s\n",
           result ? "The tree IS balanced." : "The tree is NOT balanced.");
    return result;
}

TTree* BTSMerge(TTree *tr1, TTree *tr2) {
    int n1 = size(tr1), n2 = size(tr2);
    int total = n1 + n2;
    if (total == 0) return NULL;

    TTree **arr1 = (TTree**)malloc(n1 * sizeof(TTree*));
    TTree **arr2 = (TTree**)malloc(n2 * sizeof(TTree*));
    TTree **arr  = (TTree**)malloc(total * sizeof(TTree*));

    int i1 = 0, i2 = 0;
    collectInOrder(tr1, arr1, &i1);
    collectInOrder(tr2, arr2, &i2);

    int a = 0, b = 0, c = 0;
    while (a < n1 && b < n2) {
        if (strcmp(arr1[a]->name, arr2[b]->name) <= 0)
            arr[c++] = arr1[a++];
        else
            arr[c++] = arr2[b++];
    }
    while (a < n1) arr[c++] = arr1[a++];
    while (b < n2) arr[c++] = arr2[b++];

    TTree *merged = buildBalanced(arr, 0, total - 1);
    free(arr1); free(arr2); free(arr);
    return merged;
}
static void clearInputBST(void) { int c; while ((c = getchar()) != '\n' && c != EOF); }
static void freeStackBST(TStack *s) { while (s) { TStack *t = s->next; free(s); s = t; } }

void runBST(FILE *f) {
    TTree *tree = fillTree(f);
    int choice;
    do {
        printf("\n+==========================================+\n");
        printf("|               BST MANAGER                |\n");
        printf("+==========================================+\n");
        printf("|  1.  Display tree (in-order)             |\n");
        printf("|  2.  Pre-order traversal                 |\n");
        printf("|  3.  Post-order traversal                |\n");
        printf("|  4.  Search by name                      |\n");
        printf("|  5.  Add a name                          |\n");
        printf("|  6.  Delete a name                       |\n");
        printf("|  7.  Update a name                       |\n");
        printf("|  8.  Height and size                     |\n");
        printf("|  9.  Lowest Common Ancestor              |\n");
        printf("|  10. Count nodes in birth-year range     |\n");
        printf("|  11. In-order successor                  |\n");
        printf("|  12. Mirror the tree                     |\n");
        printf("|  13. Check if tree is balanced           |\n");
        printf("|  14. Merge with a fresh copy of the tree |\n");
        printf("|  0.  Back to main menu                   |\n");
        printf("+==========================================+\n");
        printf("  Choice: ");
        scanf("%d", &choice);
        clearInputBST();

        switch (choice) {
            case 1:  traversalBSTinOrder(tree);   break;
            case 2:  traversalBSTpreOrder(tree);  break;
            case 3:  traversalBSTpostOrder(tree); break;

            case 4: {
                char name[150];
                printf("  Name to search: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0';
                getInfoNameTree(tree, name);
                break;
            }

            case 5: {
                char name[150], dob[30], dod[30];
                printf("  Name : "); fgets(name, sizeof(name), stdin); name[strcspn(name,"\n")]='\0';
                printf("  DoB  : "); scanf("%s", dob); clearInputBST();
                printf("  DoD  : "); scanf("%s", dod); clearInputBST();
                TStack *stk = addNameBST(tree, name, dob, dod);
                printf("  '%s' added to BST.\n", name);
                freeStackBST(stk);
                break;
            }

            case 6: {
                char name[150];
                printf("  Name to delete: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0';
                tree = deleteNameBST(tree, name);
                printf("  Done.\n");
                break;
            }

            case 7: {
                char name[150], def[2048], dob[30], dod[30];
                printf("  Name to update : "); fgets(name, sizeof(name), stdin); name[strcspn(name,"\n")]='\0';
                printf("  New definition : "); fgets(def,  sizeof(def),  stdin); def[strcspn(def,"\n")]='\0';
                printf("  New DoB        : "); scanf("%s", dob); clearInputBST();
                printf("  New DoD        : "); scanf("%s", dod); clearInputBST();
                tree = updateNameBST(tree, name, def, dob, dod);
                break;
            }

            case 8:  heightSizeBST(tree); break;

            case 9: {
                char w1[150], w2[150];
                printf("  First name  : "); fgets(w1, sizeof(w1), stdin); w1[strcspn(w1,"\n")]='\0';
                printf("  Second name : "); fgets(w2, sizeof(w2), stdin); w2[strcspn(w2,"\n")]='\0';
                lowestCommonAncestor(tree, w1, w2);
                break;
            }

            case 10: {
                int lo, hi;
                printf("  Low year  : "); scanf("%d", &lo); clearInputBST();
                printf("  High year : "); scanf("%d", &hi); clearInputBST();
                int cnt = countNodesRange(tree, lo, hi);
                printf("  Nodes with birth year in [%d, %d]: %d\n", lo, hi, cnt);
                break;
            }

            case 11: {
                char name[150];
                printf("  Name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0';
                inOrderSuccessor(tree, name);
                break;
            }

            case 12:
                tree = BSTMirror(tree);
                printf("  Tree mirrored.\n");
                break;

            case 13:
                isBalancedBST(tree);
                break;

            case 14: {
                TTree *tr2 = fillTree(f);   
                tree = BTSMerge(tree, tr2);
                printf("  Trees merged into a balanced BST.\n");
                break;
            }

            case 0:
                printf("  Returning to main menu.\n");
                break;

            default:
                printf("  Invalid choice.\n");
        }
    } while (choice != 0);

    freeTree(tree);
}
