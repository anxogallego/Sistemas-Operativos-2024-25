#ifndef __HIST_LIST_H__
#define __HIST_LIST_H__

#include "Library.h"


typedef int tPosL;

typedef char* tItemL;

typedef struct{
    tItemL data[MAX];
    tPosL lastPos;
}tList;



void createEmptyList(tList *L);
void saveOnList(tItemL a, tList *L);
void delete_H(tList *L);
bool isEmptyList(tList *L);

#endif
