#pragma once
#include "stdbool.h"

//int array null terminator = maximum integer value(arbitrary)
#define NULL_TERMINATOR 2147483647;
#define nullptr ((void*)0)

//TODO Add sortListDescending, mergeListsAt

typedef struct node node;
typedef node* list;

node* createList(char* data);

void pushToList(node* head, char* data);

void printList(node* head);

void printListReverse(node*head);

void popFromList(node* head);

int getListLength(node* head);

bool searchList(node* head, char* data);

void pushToListAt(node* head, char* data, int index);

//int* copyListToArray(node* head);

void deleteList(node* head, node** headPtr);

void popFromListAt(node* head, int index);

//appends head2 to the end of head1
void mergeLists(node* head1, node* head2);

char* getDataFromListIndex(node* head, int index);

void replaceDataAtIndex(node* head, int index, char* newData);