#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "red-black.h"

//create a tree
rbTree* createTree(){

    rbTree* tree = NULL;
    tree = malloc(sizeof(rbTree));
    //error check
    if(tree == NULL){
        perror("malloc");
        exit(-1);
    }

    //init
    tree->root = NULL;
    tree->counter = 0;
    //return
    return tree;

}

//creates and intializes a treeNode
treeNode* createTreeNode(transitivityPair* pair){

    treeNode* temp = NULL;
    //allocate
    temp = (treeNode*) malloc(sizeof(treeNode));
    //error check
    if(temp == NULL){
        perror("malloc");
        exit(-1);
    }

    //set piority
    //depends on the actual result
    if(pair->result == 0){
        temp->priority = pair->prediction;
    }else{
        temp->priority = abs(1 - pair->prediction);
    }
    //set pointers
    temp->pair = pair;
    temp->right = NULL;
    temp->left = NULL;
    temp->parent = NULL;
    temp->next = NULL;
    //set height
    temp->color = 'R';

    //return it
    return temp;

}

treeNode* insertTreeNodeList(treeNode* current, treeNode* node){

    treeNode* cur = current;

    //go to the last node
    while(cur->next != NULL)
        cur = cur->next;

    //inset the patient at the end
    cur->next = node;
    //fix the pointers
    node->next = NULL;

    //return the start
    return current;
}

//right rotation
treeNode* rightRotate(treeNode** root, treeNode* node){

    treeNode* node_left = node->left;

    node->left = node_left->right;

    if (node->left != NULL) 
        node->left->parent = node; 

    node_left->parent = node->parent;

    if (node->parent == NULL) 
        (*root) = node_left; 
    
    else if (node == node->parent->left) 
        node->parent->left = node_left; 
  
    else
        node->parent->right = node_left; 
  
    node_left->right = node; 
    node->parent = node_left; 

    return node;
}


//left rotation
treeNode* leftRotate(treeNode** root, treeNode* node){

    treeNode* node_right = node->right;

	node->right = node_right->left;

	if(node->right != NULL)
		node->right->parent = node;

	node_right->parent = node->parent;

	if(node->parent == NULL)
		(*root) = node_right;
	else if (node == node->parent->left)
		node->parent->left = node_right;
	else
		node->parent->right = node_right;

	node_right->left = node;
	node->parent = node_right;

    return node;

}

//fixes the tree after a normal BST insertion
void rebalance(treeNode** root, treeNode* node){

    treeNode* parentNode = NULL; 
    treeNode* grandParentNode = NULL; 

    //while there is a problem
    while ((node != (*root)) && (node->color != 'B') && (node->parent->color == 'R')){

        //fix pointers
        parentNode = node->parent;
        grandParentNode = parentNode->parent;

        //CASE A
        //parent of node is the left child of Grand-Parent
        if(parentNode == grandParentNode->left){

            //check the aunt
            treeNode* aunt = grandParentNode->right;

            //Red aunt
            //only recoloring is required
            if(aunt != NULL && aunt->color == 'R'){
                
                grandParentNode->color = 'R';
                parentNode->color = 'B';
                aunt->color = 'B';
                node = grandParentNode;
            
            //aunt is black
            }else
            {
                //Case 1
                //node is right child of it's parent
                //left rotation required
                if(node == parentNode->right){

                    leftRotate(root, parentNode);
                    node = parentNode;
                    parentNode = node->parent;
                }
            }
                //Case 2
                //node is the left child of it's parent
                //right rotation required
                rightRotate(root, grandParentNode);
                //fix colors
                char temp = parentNode->color;
                parentNode->color = grandParentNode->color;
                grandParentNode->color = temp;
                node = parentNode;
            
            

                
        
        //CASE B
        //parent of node if right child of Grand-parent
        }else{

            treeNode* aunt = grandParentNode->right;

            //Case 1
            //aunt is red
            //only recoloring is required
            if((aunt != NULL) && (aunt->color == 'R')){

                //recolor
                grandParentNode->color = 'R';
                parentNode->color = 'B';
                aunt->color = 'B';
                node = grandParentNode;
            }else{
                //Case 2
                //node is left child of parent
                //right rotation required
                if(node == parentNode->left){

                    rightRotate(root, parentNode);
                    node = parentNode;
                    parentNode = grandParentNode;
                }

                //Case 3
                //node is right child of parent
                //left rotation required
                leftRotate(root, grandParentNode);
                //swap collors
                char temp = parentNode->color;
                parentNode->color = grandParentNode->color;
                grandParentNode->color = temp;
            //if end
            }
            
        //outer if end
        }

    //while end
    }

    (*root)->color = 'B';

}

//insertion
void insertTree(treeNode** root, transitivityPair* pair){

    //create the new node
    treeNode* newNode = createTreeNode(pair);

    //if there isn't a root make it a root
    if (*root == NULL)
	{
		(*root) = newNode;
        //fix color
        newNode->color = 'B';
    
    //there is a root
	}else{

        treeNode* newParent = NULL;
        //do not mess with the root
        treeNode* curNode = (*root);

        //follow staandart BST tree insertion
        while (curNode != NULL)
        {
            //update parent ptr
            newParent = curNode;
            //if the key is smaller
            if(pair->prediction < curNode->priority)
                //go left
                curNode = curNode->left;
            else if(pair->prediction > curNode->priority)
                curNode = curNode->right;
            
            //it's equal
            //add it to the node's list
            else{

                //printf("adding to the treeNode list\n");

                //to update the parent
                newNode->parent = NULL;
                //add it
                curNode = insertTreeNodeList(curNode, newNode);
                return;
            }
        }
        //add it to the tree
        newNode->parent = newParent;
        //left or right from the parent
        if(newNode->priority < newParent->priority )
            newParent->left = newNode;
        else
            newParent->right = newNode;

        //function that fixer iregularities in the tree
        //caused by the new node
        rebalance(root, newNode);

    }

}

//function to print a node's list of equal keys (in this case dates)
void treeNodeListPrint(treeNode* node){

    treeNode* temp = node;

    //while there are nodes
    while(temp != NULL){
        printf("%11.10f %s____%s ",temp->priority,temp->pair->leftJson->site,temp->pair->rightJson->site);
        printf("\n");

        //go to the next one
        temp = temp->next;
    }
   
}

//standard rec tree print
void printTree(treeNode* root){

    if(root == NULL)
        return;

    printTree(root->left);
    //printf("%s - %d\n", root->patient->lastName, root->entryDate.tm_year);
    //print the node and his list
    treeNodeListPrint(root);

    printTree(root->right);
    
}

void freeTreeNodes(treeNode* n){

    treeNode* temp = NULL;
    treeNode* toFree = NULL;

    //if it's null do nothing
    if(n == NULL){
        return;
    }

    //for the left path
    freeTreeNodes(n->left);
    //for the right path
    freeTreeNodes(n->right);

    //free the node and his chain
    temp = n;
    toFree = n;
    while(temp != NULL){

        //set tofree
        toFree= temp;
        //node to the next one
        temp = temp->next;
        //free pair
        deletePair(toFree->pair);
        free(toFree);
    }

}

// //basic tree-search function
// treeNode* search(treeNode*root, struct tm keydate)
// {
//     if(root==NULL || (dateCompare(root->entryDate, keydate) == 0)) //if root->data is x then the element is found
//         return root;
//     else if(dateCompare(keydate, root->entryDate) > 0) // x is greater, so we will search the right subtree
//         return search(root->right, keydate);
//     else //x is smaller than the data, so we will search the left subtree
//         return search(root->left, keydate);
// }

void freeTree(rbTree* tree){

    //free all the nodes
    freeTreeNodes(tree->root);

    //free the tree
    free(tree);

}

// void sickCountByDisease(treeNode* root, char* key, int * c, struct tm d1, struct tm d2){

//     treeNode* temp = root;
    
//     if(root == NULL)
//         return;

//     sickCountByDisease(root->left, key, c, d1, d2);
//     //check each node of the chain
//     while(temp != NULL){
//         //if it's the correct disease
//         if(!strcmp(temp->patient->diseaseID, key)){
//             //if it's between dates
//             if(dateCompare(temp->entryDate, d1) >= 0 && dateCompare(temp->entryDate, d2) <= 0){
//             //printf("%s %s %d-%d-%d\n", root->patient->diseaseID, root->patient->country, root->patient->entryDate.tm_mday, root->patient->entryDate.tm_mon, root->patient->entryDate.tm_year);
//                 *c = *c + 1;
//             }
//         }
//         //adjust temp
//         temp = temp->next;

//     }

//     sickCountByDisease(root->right, key, c, d1, d2);    

// }

// void sickCountByDiseaseEXIT(treeNode* root, char* key, int* c, struct tm d1, struct tm d2){

//     treeNode* temp = root;
//     if(root == NULL)
//         return;

//     sickCountByDiseaseEXIT(root->left, key, c, d1, d2);
//     //check each node of the chain
//     while(temp != NULL){
//         //if it's the correct disease
//         if(!strcmp(temp->patient->diseaseID, key)){

//             //if there is an  exit date
//             if(temp->patient->releaseDate.tm_year != -1){
//                 //printf("YES\n");
//                 //printf("%d-%d-%d\n", temp->patient->releaseDate.tm_mday, temp->patient->releaseDate.tm_mon, temp->patient->releaseDate.tm_year);
//                 //if it's between dates
//                 if(dateCompare(temp->patient->releaseDate, d1) >= 0 && dateCompare(temp->patient->releaseDate, d2) <= 0){
//                 //printf("%s %s %d-%d-%d\n", root->patient->diseaseID, root->patient->country, root->patient->entryDate.tm_mday, root->patient->entryDate.tm_mon, root->patient->entryDate.tm_year);
//                     *c = *c + 1;
//                 }
//             }
//         }
//         //adjust temp
//         temp = temp->next;

//     }

//     sickCountByDiseaseEXIT(root->right, key, c, d1, d2);    

// }

// void ageNodeCount(treeNode* root, char* key, int* r20, int* r40, int* r60, int* rPLus, struct tm d1, struct tm d2){

//     treeNode* temp = root;
//     int age = 0;
//     if(root == NULL)
//         return;

//     ageNodeCount(root->left, key, r20, r40, r60, rPLus, d1, d2);
//     //check each node of the chain
//     while(temp != NULL){
//         //if it's the correct disease
//         if(!strcmp(temp->patient->diseaseID, key)){

//             //if it's between dates
//             if(dateCompare(temp->patient->entryDate, d1) >= 0 && dateCompare(temp->patient->entryDate, d2) <= 0){
//             //printf("%s %s %d-%d-%d\n", root->patient->diseaseID, root->patient->country, root->patient->entryDate.tm_mday, root->patient->entryDate.tm_mon, root->patient->entryDate.tm_year);
                
//                 //get the age
//                 age = atoi(temp->patient->age);

//                 //adjust counters
//                 //count it
//                 if(age <= 20){
//                     *r20 = *r20 + 1;
//                 }else if(age <= 40){
//                     *r40 = *r40 + 1;
//                 }else if(age <= 60){
//                     *r60 = *r60 + 1;
//                 }else{
//                     *rPLus =  *rPLus + 1;
//                 }
//             }
//         }
//         //adjust temp
//         temp = temp->next;

//     }

//     ageNodeCount(root->right, key, r20, r40, r60, rPLus, d1, d2);    

// }