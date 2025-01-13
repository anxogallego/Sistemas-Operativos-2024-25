#ifndef __FILE_LIST_H__
#define __FILE_LIST_H__

#include "Library.h"

typedef struct OpenFileInfo {
    int descriptor;
    char nombre[255];
    char mode[63];
}OpenFileInfo;

typedef struct fileList{
    OpenFileInfo data[255];
    int last;
}fileList;


void createFileList(fileList *F);
void insertFile(fileList *F, int desc, char *file, char *filemode);
void printFileList(fileList F);
void deleteFile(fileList *F, int desc);
char* getFileName(fileList F, int descriptor);
char* getFileMode(fileList F, int descriptor);




#endif
