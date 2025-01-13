#ifndef __P0_H__
#define __P0_H__

#include "Library.h"
#include "file_list.h"
#include "hist_list.h"
#include "proc_list.h"
#include "dir_list.h"
#include "mem_list.h"




int TrocearCadena(char* cadena, char *tr[]);

void authors(char *tr[]);
void pid();
void ppid();
void cd(char *tr[]);
void date(char *tr[]);
void infosys();
void historic(tList *L, char *tr[],int numtrozos, char * envp[]);
void help(char *tr[]);
void Cmd_open (char * tr[],fileList *F);
void Cmd_dup (char *tr[],fileList *F);

void procesarEntrada(char **trozos, bool *terminado, tList *L,fileList *F,mList *M,listProc *P, dirList *D, int numtrozos, char * envp[]);
void imprimirPrompt();


#endif
