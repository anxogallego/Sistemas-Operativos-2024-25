#include "proc_list.h"


void createEmptyListProc(listProc *list) {
    list->lastPos = -1; 
}


bool isEmptyListProc(listProc *list){
    return list->lastPos == LNULL;
}


void insertItemPri(proceso *n, listProc *l) {
    if (l->lastPos < MAX - 1) {  // Verificamos que haya espacio en la lista
        l->lastPos++;
        l->data[l->lastPos] = *n; // Copiamos los datos directamente (no usamos malloc)
    } else {
        printf("Error: lista llena\n");
    }
}

int searchPidPri(int pid, listProc l){
    int p = 0;

    while (p < l.lastPos && (pid != l.data[p].pid)){
        p++;
    }
    if (pid==(l.data[p].pid)){
        return p;
    }else{
        return -1;
    }
}

void deleteNodePri(int p, listProc *l) {
    if (p <= l->lastPos && p < MAX) { 
        // Desplazamos los elementos para "eliminar" el nodo en la posición p
        while (p < l->lastPos) {
            l->data[p] = l->data[p + 1];
            p++;
        }
        l->lastPos--;  // Reducimos la última posición válida
    } else {
        printf("Error: índice fuera de rango.\n");
    }
}

void clearListaP(listProc *l) {
    l->lastPos = -1;  // Restablecemos el último índice válido a -1, lo que vacía la lista
}

void UpdateJobs(listProc *P) {
    int status;
    pid_t result;

    for (int i = 0; i <= P->lastPos; i++) {
        if (P->data[i].terminado == 0) { 
            result = waitpid(P->data[i].pid, &status, WNOHANG);
            if (result > 0) {
                if (WIFEXITED(status)) {
                    snprintf(P->data[i].estado, sizeof(P->data[i].estado), "TERMINADO (%d)", WEXITSTATUS(status));
                    P->data[i].terminado = WEXITSTATUS(status); 
                } else if (WIFSIGNALED(status)) {
                    snprintf(P->data[i].estado, sizeof(P->data[i].estado), "SEÑAL (%d)", WTERMSIG(status));
                    P->data[i].terminado = -1; 
                }
            }
        }
    }
}
