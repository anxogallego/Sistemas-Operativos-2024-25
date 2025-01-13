#ifndef __P3_H__
#define __P3_H__

#include "Library.h"



void Cmd_getuid();
void Cmd_setuid(char *tr[]);
void listjobs(listProc *P);
void deljobs(char * trozos[], listProc *P);
void show_environ(char * tr[], int numtrozos, char * envp[]);
void search(dirList *lista, char *tr[]);
void fg(char **tr, char **envp, dirList *listaBusqueda,int numtrozos);
void fgpri(char **tr, char **envp, dirList *listaBusqueda,int numtrozos);
void programa_externo(char **tr, char **envp, dirList *listaBusqueda);
void showvar(char *tr[], int numtrozos, char *envp[]);
void changevar(char *trozos[], int numtrozos, char *envp[]);
void subsvar(char *tr[], char *envp[]);
void Cmd_fork (char *tr[], listProc *P);
void Back(char *tr[], listProc *P, dirList *listaBusqueda, char *envp[]);
void Backpri(char *tr[], listProc *P, dirList *listaBusqueda, char *envp[]);
void exec(char *tr[], dirList *D);
void execpri(char *tr[], dirList *D);



#endif
