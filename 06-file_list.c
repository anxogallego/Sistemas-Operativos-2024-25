#include "file_list.h"


void createFileList(fileList *F){
    F->last = 0;
    OpenFileInfo D;
    int flags = fcntl(0, F_GETFL);
    char *str = (char *)malloc(256);
    if (str == NULL) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }
    strcpy(str, "");
    if(flags & O_RDWR) {
        strcpy(D.mode, "O_RDWR");
    }
    D.descriptor = 0;
    strcpy(D.nombre,"entrada estandar");
    F->data[D.descriptor] = D;
    D.descriptor++;
    strcpy(D.nombre,"salida estandar");
    F->data[D.descriptor] = D;
    D.descriptor++;
    strcpy(D.nombre,"error estandar");
    F->data[D.descriptor] = D;
    F->last = 2;

    free(str);
}

void insertFile(fileList *F, int desc, char *file, char *filemode) {
    OpenFileInfo D;
    strcpy(D.nombre, file);
    strcpy(D.mode, filemode);
    D.descriptor = desc;
    int insertPos = 0;
    while (insertPos <= F->last && F->data[insertPos].descriptor < desc) {
        insertPos++;
    }
    for (int i = F->last; i >= insertPos; i--) {
        F->data[i + 1] = F->data[i];
    }
    F->data[insertPos] = D;
    F->last++;
    printf("Añadida entrada %d a la tabla de ficheros abiertos\n",F->data[insertPos].descriptor);
}

void printFileList(fileList F) {
    printf("Lista de archivos abiertos:\n");
    for (int i = 0; i <= F.last; i++) {
        printf("Descriptor: %d -> %s %s\n", F.data[i].descriptor, F.data[i].nombre, F.data[i].mode);
    }
}

void deleteFile(fileList *F, int desc) {
    int indiceEliminar = -1;
    int aux;

    for (int i = 0; i <= F->last; i++) {
        if (F->data[i].descriptor == desc) {
            indiceEliminar = i;
            break;
        }
    }
    if (indiceEliminar >= 0) {
        for (int i = indiceEliminar; i < F->last; i++) {
            aux = F->data[i].descriptor;
            F->data[i] = F->data[i + 1];
            F->data[i].descriptor = aux;
        }
        F->last--;
    }
}

char* getFileName(fileList F, int descriptor) {
    char *return_name = malloc(255 * sizeof (char));
    for (int i = 0; i <= F.last; i++) {
        if (F.data[i].descriptor == descriptor) {
            strcpy(return_name,F.data[i].nombre);
            return return_name;
        }
    }
    return NULL;
}

char* getFileMode(fileList F, int descriptor) {
    char *return_mode = malloc(255* sizeof (char));
    for (int i = 0; i <= F.last; i++) {
        if (F.data[i].descriptor == descriptor) {
            strcpy(return_mode,F.data[i].mode);
            return return_mode;
        }
    }
    return NULL;
}
