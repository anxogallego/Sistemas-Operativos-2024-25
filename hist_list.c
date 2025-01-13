#include "hist_list.h"


void createEmptyList(tList *L){
    for (int i = 0; i < 4096; i++) {
    L->data[i] = NULL;
}
    L->lastPos = LNULL;
}


void saveOnList(tItemL a, tList *L){
    if(L->lastPos!=MAX-1) {
        strtok(a, "\n");
        (*L).lastPos++;
        L->data[L->lastPos]=malloc(1024 * sizeof(char *));
        strcpy(L->data[L->lastPos],a);
    
    }
    else
        printf("non hai espacio");
}


void delete_H(tList *L){
    int i=0;
    for(i = 0; i<=L->lastPos;i++)
        free(L->data[i]);
    L->lastPos=LNULL;
}



bool isEmptyList(tList *L){
    return L->lastPos == LNULL;
}
