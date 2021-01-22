#ifndef REDBLACK
#define REDBLACK

#include "hashTable.h"


typedef struct treeNode
{
    //primary key
    double priority ;
    transitivityPair* pair;
    struct treeNode* left;
    struct treeNode* right;
    struct treeNode* parent;
    struct treeNode* next;
    char color;

}treeNode;

typedef struct rbTree
{
    
    treeNode* root;
    int counter;

}rbTree;

rbTree* createTree();
treeNode* createTreeNode(transitivityPair* );
treeNode* insertTreeNodeList(treeNode* current, treeNode* node);
treeNode* rightRotate(treeNode** root, treeNode* node);
treeNode* leftRotate(treeNode** root, treeNode* node);
void rebalance(treeNode** root, treeNode* node);
treeNode* insertTree(treeNode* root, transitivityPair* );
void insertTree3(transitivityPair *pair, treeNode **root);
void treeNodeListPrint(treeNode* node);
//treeNode* search(treeNode*root, struct tm keydate);
void printTree(treeNode* root);
void freeTreeNodes(treeNode* n);
void freeTree(rbTree* tree);
//void sickCountByDisease(treeNode* root, char* key, int * c, struct tm d1, struct tm d2);
//void sickCountByDiseaseEXIT(treeNode* root, char* key, int* c, struct tm d1, struct tm d2);
//void ageNodeCount(treeNode* root, char* key, int* r20, int* r40, int* r60, int* rPLus,struct tm d1, struct tm d2);
#endif