#ifndef __P2_H__
#define __P2_H__

#include "Library.h"
#include "mem_list.h"



int  do_AllocateMalloc(char *tr[], mList *M);
void do_AllocateMmap(char *tr[],mList *M);
void do_AllocateAttachShared(char *tr[], mList *M);
void do_AllocateCreateShared(char *tr[], mList *M);

void do_DeallocateMalloc(char *trozos[],mList *M);
void do_DeallocateMmap(char *tr[],mList *M);
void do_DeallocateShared(char *trozos[], mList *M);
void do_DeallocateDelkey (char *tr[],mList* M);
void do_DeallocateAddr(char *trozos[],mList * M);

int writefile(char *tr[]);
int write_fd(char *tr[]);

void Cmd_Readfile(char *tr[]);
void Cmd_Read(char *tr[]);

void memfill(char *tr[], int numtrozos, mList *M);
void memdump(char *tr[],int numtrozos, mList *M);

void memory(char *trozos[], int numtrozos, mList *M);

void Recursiva (int n);






#endif
