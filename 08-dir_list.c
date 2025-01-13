      
#include "dir_list.h"


void InitPathList(dirList *list) {
    list->count = 0;
}

bool IsPathListEmpty(dirList list) {
    return list.count == 0;
}

void AddPath(dirList *list, const char *path) {
    if (list->count >= MAX_DIRECTORIOS) {
        printf("Error: La lista de rutas está llena.\n");
        return;
    }

    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->paths[i], path) == 0) {
            printf("La ruta '%s' ya está en la lista.\n", path);
            return;
        }
    }

    strncpy(list->paths[list->count], path, MAX_NOMBRE - 1);
    list->paths[list->count][MAX_NOMBRE - 1] = '\0';
    list->count++;
    printf("Ruta '%s' agregada a la lista.\n", path);
}

void RemovePath(dirList *list, const char *path) {
    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->paths[i], path) == 0) {
            for (int j = i; j < list->count - 1; j++) {
                strncpy(list->paths[j], list->paths[j + 1], MAX_NOMBRE);
            }
            list->count--;
            printf("Ruta '%s' eliminada de la lista.\n", path);
            return;
        }
    }
    printf("Ruta '%s' no encontrada en la lista.\n", path);
}

void ShowPaths(const dirList *list) {
    if (list->count == 0) {
        printf("La lista de rutas está vacía.\n");
        return;
    }

    printf("Lista de rutas:\n");
    for (int i = 0; i < list->count; i++) {
        printf("- %s\n", list->paths[i]);
    }
}

void ClearPaths(dirList *list) {
    list->count = 0;
    printf("Lista de rutas limpiada.\n");
}


char *GetFirstPath(dirList *list) {
    if (list->count == 0) {
        return NULL;
    }
    list -> iterator = 0;
    return list->paths[list -> iterator];
}

char *GetNextPath(dirList *list) {
    list -> iterator++;
    if (list -> iterator < list->count) {
        return list->paths[list -> iterator];
    }
    return NULL;
}


void ImportFromEnvPATH(dirList *list) {
    ClearPaths(list);

    const char *env_path = getenv("PATH");
    if (env_path == NULL) {
        printf("Error: No se pudo obtener la variable PATH.\n");
        return;
    }

    // Procesar directamente sin duplicar la cadena
    const char *start = env_path;
    const char *end;

    while ((end = strchr(start, ':')) != NULL) {
        size_t length = end - start;

        // Verificamos que la longitud no exceda el tamaño máximo permitido
        if (length > 0 && length < MAX_NOMBRE) {
            char path[MAX_NOMBRE];
            strncpy(path, start, length);
            path[length] = '\0'; // Aseguramos que esté null-terminated
            AddPath(list, path);
        }

        start = end + 1; // Avanzamos al siguiente segmento
    }

    // Procesar el último directorio (o único, si no hay ':')
    if (*start != '\0' && strlen(start) < MAX_NOMBRE) {
        AddPath(list, start);
    }

    printf("Lista de rutas actualizada desde PATH.\n");
}

