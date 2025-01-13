#ifndef PROC_LIST_H__
#define PROC_LIST_H__

#include "Library.h"

typedef int pos;

typedef struct{
    int pid;
    char command[300];
    char inicio[50];
    char estado[50];
    int terminado;
}proceso;

typedef struct{
    proceso data[MAX];
    pos lastPos;
}listProc;

void createEmptyListProc(listProc *list);

bool isEmptyListProc(listProc *list);

void insertItemPri(proceso *n, listProc *list);

int searchPidPri(int pid, listProc l);

void deleteNodePri(pos p, listProc *l);

void clearListaP(listProc *l);

void UpdateJobs(listProc *P);

#endif 
