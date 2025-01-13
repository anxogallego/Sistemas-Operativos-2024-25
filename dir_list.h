#ifndef DIR_LIST_H
#define DIR_LIST_H

#include "Library.h"

typedef struct {
    char paths[MAX_DIRECTORIOS][MAX_NOMBRE];
    int count;
    int iterator;
} dirList;

void InitPathList(dirList *list);
void AddPath(dirList *list, const char *path);
void RemovePath(dirList *list, const char *path);
void ShowPaths(const dirList *list);
void ClearPaths(dirList *list);
char *GetFirstPath(dirList *list);
char *GetNextPath(dirList *list);
void ImportFromEnvPATH(dirList *list);
bool IsPathListEmpty(dirList list);


#endif