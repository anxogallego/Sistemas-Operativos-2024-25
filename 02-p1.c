/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * CREADO POR: * * * * * * * * * * *
 * * * *                                         * * * *
 * * *                                             * * *
 * * * *                                       * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "Library.h"
#include "hist_list.h"
#include "file_list.h"
#include "p1.h"


void liberar(char **a, int cont){
    for (int p = 0; p < cont; p++) {
        free(a[p]);
    }
    free(a);
}
char LetraTF (mode_t m)
{
     switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */ 
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
     }
}
/*las siguientes funciones devuelven los permisos de un fichero en formato rwx----*/
/*a partir del campo st_mode de la estructura stat */
/*las tres son correctas pero usan distintas estrategias de asignación de memoria*/

char * ConvierteModo2 (mode_t m)
{
    static char permisos[12];
    strcpy (permisos,"---------- ");
    
    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t';
    
    return permisos;
}

void cwd(char *tr[]){
if (tr[1]==NULL){
        char buffer[100];
        printf("%s \n", getcwd(buffer, 100));
    }
else perror("comando incorrecto\n");
}


void cmd_makefile(char *tr[]) {
    FILE* file;

    // Abre el archivo en modo escritura
    file = fopen(tr[1], "w");
    if (file == NULL) {
        perror("Error al crear el archivo");
        return;
    }

    // Mensaje de éxito
    printf("The file '%s' has been created successfully\n", tr[1]);

    // Cierra el archivo para liberar el descriptor
    fclose(file);
}

void cmd_makedir(char *tr[]){
int directory;
    directory= mkdir(tr[1],0777);
    if (directory!=-1) {
        printf("The directory has been created successfully\n");
    } else {
        perror("Error");
    }
}

int erase (char * tr[],int numtrozos) {
    struct stat buffer;
    if (tr[1]==NULL){
        char cwd[1024];
        printf("%s\n",getcwd(cwd,1024));
    } else {
        for (int i=1; i<numtrozos ; i++) {
            int checkstat = lstat(tr[i],&buffer);
            if (checkstat != -1) {
                char IsDirectory = LetraTF(buffer.st_mode);
                if (IsDirectory == 'd') {
                    if (rmdir(tr[i])==-1) {
                        perror("Error");
                    }
                } else {
                    if (unlink(tr[i])==-1) {
                        perror("Error");
                    }
                }
            } else {
                perror("Error");
            }
        }
    }
    return 0;
}


void listfile(char *tr[], int numtrozos) {
    int nlong = 0; int nacc = 0; int nlink = 0;
    char linkName[100]="";
    struct stat st1;
    struct tm *tm_info ;
    char time[100];
    for(int i = 1; i < numtrozos; i++) {
        if (strcmp(tr[i], "-long")==0) nlong++;
        if (strcmp(tr[i], "-acc")==0) nacc++;
        if (strcmp(tr[i], "-link")==0) nlink++;
    }

    if (nlong>=1 && nacc>=1 && nlink>=1) {
       for (int i = 4; i < numtrozos; i++){
       if(stat(tr[i], &st1)==0){
            tm_info = localtime(&st1.st_atime);
            if (tm_info != NULL) {
            strftime(time, sizeof(time), "%Y/%m/%d-%H:%M ", localtime(&st1.st_atime));
            }
            unsigned long links = st1.st_nlink;
            unsigned long inode = st1.st_ino;
            struct group *group = getgrgid(st1.st_gid);
            struct passwd *user = getpwuid(st1.st_uid);
            char *permisos = ConvierteModo2(st1.st_mode);
            long size1 = st1.st_size;
            printf("%s %ld ( %ld)\t%s\t%s %s\t %ld %s\n", time,links, inode, group->gr_name, user->pw_name, permisos, size1, tr[i]);
            if (readlink(tr[i], linkName, sizeof(linkName) - 1) != -1)
                printf("%s -> %s\n", tr[i], linkName); // Si es un link simbólico, muestra el destino
            }else{
                perror("Error al obtener información del archivo");
                continue;
            }
       }
    } else if (nlong==1 && nacc==1 && nlink==0) {
        for (int i = 3; i < numtrozos; i++) {
            
            if(stat(tr[i], &st1)==0){
            tm_info = localtime(&st1.st_atime);
            if (tm_info != NULL) {
            strftime(time, sizeof(time), "%Y/%m/%d-%H:%M ", localtime(&st1.st_atime));
            }
            unsigned long links = st1.st_nlink;
            unsigned long inode = st1.st_ino;
            struct group *group = getgrgid(st1.st_gid);
            struct passwd *user = getpwuid(st1.st_uid);
            char *permisos = ConvierteModo2(st1.st_mode);
            long size1 = st1.st_size;
            printf("%s %ld ( %ld)\t%s\t%s %s\t %ld %s\n", time, links, inode, group->gr_name, user->pw_name, permisos, size1, tr[i]);
            }else{
                perror("Error al obtener información del archivo");
                continue;
            }
        }
    } else if (nlong>=1 && nacc==0 && nlink>=1) {
        for (int i = 3; i < numtrozos; i++) {
            
            if(stat(tr[i], &st1)==0){
            tm_info = localtime(&st1.st_atime);
            if (tm_info != NULL) {
            strftime(time, sizeof(time), "%Y/%m/%d-%H:%M ", localtime(&st1.st_ctime));
            }
            unsigned long links = st1.st_nlink;
            unsigned long inode = st1.st_ino;
            struct group *group = getgrgid(st1.st_gid);
            struct passwd *user = getpwuid(st1.st_uid);
            char *permisos = ConvierteModo2(st1.st_mode);
            long size1 = st1.st_size;
            printf("%s %ld ( %ld)\t%s\t%s %s\t %ld %s\n", time,links, inode, group->gr_name, user->pw_name, permisos, size1, tr[i]);
            if (readlink(tr[i], linkName, sizeof(linkName) - 1) != -1)
                printf("%s -> %s\n", tr[i], linkName); // Si es un link simbólico, muestra el destino
            }else{
                perror("Error al obtener información del archivo");
                continue;
            }
        }
    } else if (nlong>=1 && nacc==0 && nlink==0) {
        for (int i = 2; i < numtrozos; i++) {
            
            if(stat(tr[i], &st1)==0){
            tm_info = localtime(&st1.st_atime);
            if (tm_info != NULL) {
            strftime(time, sizeof(time), "%Y/%m/%d-%H:%M ", localtime(&st1.st_ctime));
            }
            unsigned long links = st1.st_nlink;
            unsigned long inode = st1.st_ino;
            struct group *group = getgrgid(st1.st_gid);
            struct passwd *user = getpwuid(st1.st_uid);
            char *permisos = ConvierteModo2(st1.st_mode);
            long size1 = st1.st_size;
            printf("%s %ld ( %ld)\t%s\t%s %s\t %ld %s\n",time, links, inode, group->gr_name, user->pw_name, permisos, size1, tr[i]);
            }else{
                perror("Error al obtener información del archivo");
                continue;
            }
        }
    } else if (nlong==0 && nacc>=1 && nlink>=1) {
        for (int i = 3; i < numtrozos; i++) {

            if(stat(tr[i], &st1)==0){
            long size = st1.st_size;
            tm_info = localtime(&st1.st_atime);
            if (tm_info != NULL) {
            strftime(time, sizeof(time), "%Y/%m/%d-%H:%M ", localtime(&st1.st_atime));
            }
            printf("%s %ld\t %s\n",time,size,tr[i]);
            if (readlink(tr[i], linkName, sizeof(linkName) - 1) != -1)
                printf("%s -> %s\n", tr[i], linkName); // Si es un link simbólico, muestra el destino
            }else{
                perror("Error al obtener información del archivo");
                continue;
            }
        }
    } else if (nlong==0 && nacc>=1 && nlink==0) {
        for (int i = 2; i < numtrozos; i++) {
            
            if(stat(tr[i], &st1)==0){
            long size = st1.st_size;
            tm_info = localtime(&st1.st_atime);
            if (tm_info != NULL) {
            strftime(time, sizeof(time), "%Y/%m/%d-%H:%M ", localtime(&st1.st_atime));
            }
            printf("%ld\t %s\t %s\n",size,time,tr[i]);
            }else{
                perror("Error al obtener información del archivo");
                continue;
            }
        }
    } else if (nlong==0 && nacc==0 && nlink>=1) {
        for (int i = 2; i < numtrozos; i++) {
            if(stat(tr[i], &st1)==0){
            long size = st1.st_size;
            printf("%ld\t %s\n",size,tr[i]);
            if (readlink(tr[i], linkName, sizeof(linkName) - 1) != -1)
                printf("%s -> %s\n", tr[i], linkName); // Si es un link simbólico, muestra el destino
            }else{
                perror("Error al obtener información del archivo");
                continue;
            }    
        }
    } else {
        for (int i = 1; i < numtrozos; i++) {
            if(stat(tr[i], &st1)==0){
            long size = st1.st_size;
            printf("%ld\t %s\n",size,tr[i]);
        }else{
            perror("Error al obtener información del archivo");
            continue;
        }

        }
 }
 }



bool isDirectory(const char *path){
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
        return true;
    }
    return false;
}

//Auxiliar para delrec
int deleteDir(char *dir){  //Deletes a directory
    DIR *dirp;
    struct dirent *flist;
    char aux[MAX];

    if((dirp=opendir(dir)) ==NULL)return -1;

    while ((flist=readdir(dirp))!=NULL) { //recorre los archivos del directorio
        strcpy(aux, dir);
        strcat(strcat(aux, "/"),flist->d_name);

        if(strcmp(flist->d_name, "..") == 0 ||
                strcmp(flist->d_name, ".") == 0)continue;

        if(isDirectory(aux)){ //si es un directorio repetir recursivamente
            deleteDir(aux);
        }
        if(remove(aux))return -1; //borra el directorio
    }
    closedir(dirp);

    return 0;
}


int delrec(char *tr[], int numtrozos) {

    if(numtrozos != 0){
        for(int i=1; i< numtrozos; i++){
            if(isDirectory(tr[i]) ){
                if(deleteDir(tr[i])==-1 || remove(tr[i]))
                    perror("Imposible borrar");
            }else if(remove(tr[i])){
                perror("Imposible borrar");
            }
        }
    }else { //muestra el directorio actual
        cwd(0);
    }
    return 0;
}


void listdir(char *tr[], int numtrozos) {
    int nlong = 0, nacc = 0, nlink = 0, nhid = 0;
    struct stat st;
    DIR *dir;
    struct dirent *entry;
    char fecha[100];

    
    for (int i = 1; i < numtrozos; i++) {
        if (strcmp(tr[i], "-long") == 0) nlong = 1;
        if (strcmp(tr[i], "-acc") == 0) nacc = 1;
        if (strcmp(tr[i], "-link") == 0) nlink = 1;
        if (strcmp(tr[i], "-hid") == 0) nhid = 1;
    }
    
    for (int i = 1; i < numtrozos; i++) {
        if (tr[i][0] == '-') {
            continue; 
        }

        char *directory = tr[i];
        dir = opendir(directory);

        if (dir == NULL) {
            perror("No se puede abrir el directorio");
            continue;  
        }

        printf("************%s\n", directory);

        
        while ((entry = readdir(dir)) != NULL) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);

            
            if (!nhid && entry->d_name[0] == '.') {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }
            }

            if (stat(path, &st) == -1) {
                printf("Error al obtener información del archivo: %s\n", entry->d_name);
                continue;
            }
            if(nlong == 0 && nacc == 0 && nlink == 0) {
                    if(stat(tr[i], &st)==0) {
                        if(stat(path, &st)==0){
                            long size = st.st_size;
                            printf("%ld\t %s\n",size,entry->d_name);
                        }
                    }else{
                        perror("Error al obtener información del archivo");
                    }

            }

            
            if (nlong == 0 && nacc == 0 && nlink == 1) {


                    if(stat(tr[i], &st)==0) {
                        if(stat(path, &st)==0){
                            long size = st.st_size;
                            printf("%ld\t %s\n",size,tr[i]);
                            if (S_ISLNK(st.st_mode)) {
                                char linkname[1024];
                                ssize_t len = readlink(path, linkname, sizeof(linkname) - 1);
                                if (len != -1) {
                                    linkname[len] = '\0';
                                    printf(" -> %s\n", linkname);
                                }
                            }
                        }
                    }else{
                        perror("Error al obtener información del archivo");
                    }

            }

            if (nlong == 0 && nacc == 1 && nlink == 0) {



                    if(stat(tr[i], &st)==0) {
                        if(stat(path, &st)==0){
                            long size = st.st_size;
                            struct tm *tiempo = localtime(&st.st_atime);
                            if (tiempo != NULL) {
                                strftime(fecha, sizeof(fecha), "%Y/%m/%d-%H:%M", tiempo);
                            }
                            printf("%ld\t %s\t %s\n",size,fecha,entry->d_name);
                        }
                    }else{
                        perror("Error al obtener información del archivo");
                    }

            }

            if (nlong == 0 && nacc == 1 && nlink == 1) {


                    if(stat(tr[i], &st)==0){
                        if(stat(path, &st)==0) {
                            long size = st.st_size;
                            struct tm *tiempo = localtime(&st.st_atime);
                            if (tiempo != NULL) {
                                strftime(fecha, sizeof(fecha), "%Y/%m/%d-%H:%M", tiempo);
                            }
                            printf("%ld\t %s\t %s\n",size,fecha,entry->d_name);
                            if (S_ISLNK(st.st_mode)) {
                                char linkname[1024];
                                ssize_t len = readlink(path, linkname, sizeof(linkname) - 1);
                                if (len != -1) {
                                    linkname[len] = '\0';
                                    printf(" -> %s\n", linkname);
                                }
                            }
                        }

                    }else{
                        perror("Error al obtener información del archivo");
                    }

            }

            if (nlong == 1 && nacc == 0 && nlink == 0) {
                    if(stat(tr[i], &st)==0){
                        if(stat(path, &st)==0) {
                            struct tm *tiempo = localtime(&st.st_ctime);
                            if (tiempo != NULL) {
                                strftime(fecha, sizeof(fecha), "%Y/%m/%d-%H:%M", tiempo);
                            }
                            long size = st.st_size;
                            struct passwd *pwd = getpwuid(st.st_uid);
                            struct group *grp = getgrgid(st.st_gid);
                            char *permisos = ConvierteModo2(st.st_mode);
                            printf("%s  %ld (%lu) %s %s %s %ld %s\n", fecha, st.st_nlink,(unsigned long)st.st_ino, pwd->pw_name, grp->gr_name,
                                   permisos, size, entry->d_name);

                        }
                    }else{
                        perror("Error al obtener información del archivo");
                    }

            }
            if (nlong == 1 && nacc == 0 && nlink == 1) {
                    if(stat(tr[i], &st)==0){
                        if(stat(path, &st)==0) {
                            long size = st.st_size;
                            struct tm *tiempo = localtime(&st.st_ctime);
                            if (tiempo != NULL) {
                                strftime(fecha, sizeof(fecha), "%Y/%m/%d-%H:%M", tiempo);
                            }
                            struct passwd *pwd = getpwuid(st.st_uid);
                            struct group *grp = getgrgid(st.st_gid);
                            char *permisos = ConvierteModo2(st.st_mode);
                            printf("%s  %ld (%lu) %s %s %s %ld %s\n", fecha, st.st_nlink,(unsigned long)st.st_ino, pwd->pw_name, grp->gr_name,
                                   permisos, size, entry->d_name);
                            if (S_ISLNK(st.st_mode)) {
                                char linkname[1024];
                                ssize_t len = readlink(path, linkname, sizeof(linkname) - 1);
                                if (len != -1) {
                                    linkname[len] = '\0';
                                    printf(" -> %s\n", linkname);
                                }
                            }
                        }
                    }else{
                        perror("Error al obtener información del archivo");
                    }

            }

            if (nlong == 1 && nacc == 1 && nlink == 0) {
                    if(stat(tr[i], &st)==0){
                        if(stat(path, &st)==0) {
                            long size = st.st_size;
                            struct tm *tiempo = localtime(&st.st_atime);
                            if (tiempo != NULL) {
                                strftime(fecha, sizeof(fecha), "%Y/%m/%d-%H:%M", tiempo);
                            }
                            struct passwd *pwd = getpwuid(st.st_uid);
                            struct group *grp = getgrgid(st.st_gid);
                            char *permisos = ConvierteModo2(st.st_mode);
                            printf("%s %ld (%lu) %s %s %s %ld %s\n", fecha, st.st_nlink,(unsigned long)st.st_ino, pwd->pw_name, grp->gr_name,
                                   permisos, size, entry->d_name);
                        }

                    }else{
                        perror("Error al obtener información del archivo");
                    }
            }

            if (nlong == 1 && nacc == 1 && nlink == 1) {

                    if(stat(tr[i], &st)==0){
                        if(stat(path, &st)==0) {
                            struct tm *tiempo = localtime(&st.st_atime);
                            long size = st.st_size;
                            if (tiempo != NULL) {
                                strftime(fecha, sizeof(fecha), "%Y/%m/%d-%H:%M", tiempo);
                            }
                            struct passwd *pwd = getpwuid(st.st_uid);
                            struct group *grp = getgrgid(st.st_gid);
                            char *permisos = ConvierteModo2(st.st_mode);
                            printf("%s  %ld (%lu) %s %s %s %ld %s\n", fecha, st.st_nlink,(unsigned long)st.st_ino, pwd->pw_name, grp->gr_name,
                                   permisos, size, entry->d_name);
                            if (S_ISLNK(st.st_mode)) {
                                char linkname[1024];
                                ssize_t len = readlink(path, linkname, sizeof(linkname) - 1);
                                if (len != -1) {
                                    linkname[len] = '\0';
                                    printf(" -> %s\n", linkname);
                                }
                            }
                        }
                    }else{
                        perror("Error al obtener información del archivo");
                    }
            }
        }

        if (closedir(dir) == -1) {
            perror("Imposible cerrar el directorio");
        }
    }
        printf("\n"); 
    }

void show_file_info(const char *path, const struct stat st, int nlong, int nacc, int nlink) {
    char fecha[100];
    struct tm *tiempo;

    if (nlong) {
        // Mostrar información detallada del archivo (similar a listdir)
        struct passwd *pwd = getpwuid(st.st_uid);
        struct group *grp = getgrgid(st.st_gid);
        char *permisos = ConvierteModo2(st.st_mode);
        tiempo = localtime(nacc ? &st.st_atime : &st.st_ctime); // Usar st_atime si -acc está presente
        strftime(fecha, sizeof(fecha), "%Y/%m/%d-%H:%M", tiempo);
        printf("%s  %ld (%lu) %s %s %s %ld %s\n", fecha, st.st_nlink, (unsigned long)st.st_ino, pwd->pw_name, grp->gr_name, permisos, st.st_size, path);

        if (nlink && S_ISLNK(st.st_mode)) {
            char linkname[1024];
            ssize_t len = readlink(path, linkname, sizeof(linkname) - 1);
            if (len != -1) {
                linkname[len] = '\0';
                printf(" -> %s\n", linkname);
            }
        }
    } else {
        // Modo simple, sin detalles largos
        tiempo = localtime(nacc ? &st.st_atime : &st.st_ctime); // Usar st_atime si -acc está presente
        strftime(fecha, sizeof(fecha), "%Y/%m/%d-%H:%M", tiempo);
        printf("%ld\t%s\n", st.st_size, path);

        if (nlink && S_ISLNK(st.st_mode)) {
            char linkname[1024];
            ssize_t len = readlink(path, linkname, sizeof(linkname) - 1);
            if (len != -1) {
                linkname[len] = '\0';
                printf(" -> %s\n", linkname);
            }
        }
    }
}

void listdir_recursively(char *tr[], int numtrozos, int nlong,int nacc, int nlink, int nhid) {
    DIR *dir = opendir(tr[numtrozos]);
    char *aux[15];
    struct dirent *flist;
    struct stat st;
    


    if (dir == NULL) {
        listfile(tr, numtrozos);
    }

    printf("************%s\n", tr[numtrozos]); // Imprime el directorio actual

    // Primero listar todos los archivos en el directorio actual
    while ((flist = readdir(dir)) != NULL) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", tr[numtrozos], flist->d_name);

        if (!nhid && flist->d_name[0] == '.') {
            continue;
        }

        if (lstat(path, &st) == -1) {
            perror("Error al obtener información del archivo");
            continue;
        }

        // Si no es un directorio, mostrar el archivo

            // Mostrar el archivo según las opciones (similar a listdir o listfile)
            show_file_info(path, st, nlong, nacc, nlink);
    }

    closedir(dir);// Volver al inicio del directorio para procesar subdirectorios
    if((dir = opendir(tr[numtrozos]))!=NULL) {
        // Luego listar recursivamente los subdirectorios
        while ((flist = readdir(dir)) != NULL) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", tr[numtrozos], flist->d_name);

            // Evitar "." y ".."
            if (strcmp(flist->d_name, ".") == 0 || strcmp(flist->d_name, "..") == 0) {
                continue;
            }

            // Omitir archivos ocultos si no se ha especificado la opción -hid
            if (!nhid && flist->d_name[0] == '.') {
                continue;
            }
            if (lstat(path, &st) == -1) {
                perror("Error al obtener información del archivo");
                continue;
            }

            // Si es un subdirectorio, llamar recursivamente
            if (S_ISDIR(st.st_mode)) {
                for(int i = 0; i < numtrozos; i++) {
                    aux[i] = strdup(tr[i]);
                }
                aux[numtrozos] = strdup(path);
                listdir_recursively(aux, numtrozos, nlong, nacc, nlink, nhid);
                // Liberar los duplicados creados con strdup
                for (int i = 0; i <= numtrozos; i++) {
                    free(aux[i]);
                }

            }
        }
    }

    closedir(dir); // Cerrar el directorio
}


int PosicionInicio(char *tr[], int numtrozos) {
    int i, n = 0;
    for (i = 1; i < numtrozos; i++) {
        if (strcmp(tr[i], "-long") == 0 || strcmp(tr[i], "-acc") == 0 || strcmp(tr[i], "-link")==0 || strcmp(tr[i], "-hid")==0){
            n++;
        }
    }
    return n+1;
}

void reclist(char *tr[], int numtrozos) {
    struct stat st;
    int nlong = 0, nacc = 0, nlink = 0, nhid = 0;

    // Verificar las opciones
    for (int i = 1; i < numtrozos; i++) {
        if (strcmp(tr[i], "-long") == 0) nlong = 1;
        if (strcmp(tr[i], "-acc") == 0) nacc = 1;
        if (strcmp(tr[i], "-link") == 0) nlink = 1;
        if (strcmp(tr[i], "-hid") == 0) nhid = 1;
    }

    // Obtener los directorios a listar
    for (int i = PosicionInicio(tr, numtrozos); i < numtrozos; i++) {
        if (stat(tr[i], &st) == -1) {
            perror("Error al obtener información del archivo");
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            listdir_recursively(tr, i, nlong, nacc, nlink, nhid);
        }
    }
}
void auxlistdir_recursively(char *tr[], int numtrozos, int nlong,int nacc, int nlink, int nhid) {
    DIR *dir = opendir(tr[numtrozos]);
    char *aux[15];
    struct dirent *flist;
    struct stat st;
    

    if (dir == NULL) {
        listfile(tr, numtrozos);
    }

  // Imprime el directorio actual
    if(dir!=NULL) {
        // Luego listar recursivamente los subdirectorios
        while ((flist = readdir(dir)) != NULL) {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", tr[numtrozos], flist->d_name);
        

            // Evitar "." y ".."
            if (strcmp(flist->d_name, ".") == 0 || strcmp(flist->d_name, "..") == 0) {
                continue;
            }

            // Omitir archivos ocultos si no se ha especificado la opción -hid
            if (!nhid && flist->d_name[0] == '.') {
                continue;
            }
            if (lstat(path, &st) == -1) {
                perror("Error al obtener información del archivo");
                continue;
            }

            // Si es un subdirectorio, llamar recursivamente
            if (S_ISDIR(st.st_mode)) {
                for(int i = 0; i < numtrozos; i++) {
                    aux[i] = strdup(tr[i]);
                }
                aux[numtrozos] = strdup(path);
                auxlistdir_recursively(aux, numtrozos, nlong, nacc, nlink, nhid);
                
                // Liberar los duplicados creados con strdup
                for (int i = 0; i <= numtrozos; i++) {
                    free(aux[i]);
                }
            }
            
        }
    }
    closedir(dir);
    dir = opendir(tr[numtrozos]);
    printf("**%s\n", tr[numtrozos]);
    


    // Primero listar todos los archivos en el directorio actual
    while ((flist = readdir(dir)) != NULL) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", tr[numtrozos], flist->d_name);

        

        if (!nhid && flist->d_name[0] == '.') {
            continue;
        }

        if (lstat(path, &st) == -1) {
            perror("Error al obtener información del archivo");
            continue;
        }

        // Si no es un directorio, mostrar el archivo

            // Mostrar el archivo según las opciones (similar a listdir o listfile)
            show_file_info(path, st, nlong, nacc, nlink);
    }

 // Volver al inicio del directorio para procesar subdirectorios


    closedir(dir); // Cerrar el directorio
}

void revlist(char *tr[], int numtrozos) {
    struct stat st;
    int nlong = 0, nacc = 0, nlink = 0, nhid = 0;

    // Verificar las opciones
    for (int i = 1; i < numtrozos; i++) {
        if (strcmp(tr[i], "-long") == 0) nlong = 1;
        if (strcmp(tr[i], "-acc") == 0) nacc = 1;
        if (strcmp(tr[i], "-link") == 0) nlink = 1;
        if (strcmp(tr[i], "-hid") == 0) nhid = 1;
    }

    // Obtener los directorios a listar
    for (int i = PosicionInicio(tr, numtrozos); i < numtrozos; i++) {
        if (stat(tr[i], &st) == -1) {
            perror("Error al obtener información del archivo");
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            auxlistdir_recursively(tr, i, nlong, nacc, nlink, nhid);
        }
    }
}
