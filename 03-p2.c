/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * CREADO POR: * * * * * * * * * * *
 * * * * 										 * * * *
 * * * 											   * * *
 * * * *  									   * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Library.h"
#include "hist_list.h"
#include "file_list.h"
#include "mem_list.h"
#include "p2.h"
#include "p0.h"



void* cadtop(const char* str) {
    return (void*) strtoull(str, NULL, 16);
}
void LlenarMemoria (void *p, size_t cont, unsigned char byte)
{
  unsigned char *arr=(unsigned char *) p;
  size_t i;

  for (i=0; i<cont;i++)
		arr[i]=byte;
}
char * formatoFecha(){
    time_t t;
    struct tm *tm;
    char * buff, *aux;
    buff = (char *) malloc(50 * sizeof(char));
    t = time(NULL);
    tm = localtime(&t);
    strftime(buff, 50, "%02d/%02m/%y  %02H:%02M:%02S ",tm);
    aux=buff;
    return aux;


}
void * MapearFichero(char * fichero, int protection, mList* M)
{
    int df, map = MAP_PRIVATE, modo = O_RDONLY;
    struct stat s;
    void *p;

    // Determinar el modo de apertura
    if (protection & PROT_WRITE)
        modo = O_RDWR;

    // Verificar el estado del archivo y abrirlo
    if (stat(fichero, &s) == -1 || (df = open(fichero, modo)) == -1)
        return NULL;

    // Mapear el archivo
    if ((p = mmap(NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED)
        return NULL;

    // Verificar si hay espacio en la lista
    if (M->lastPos + 1 >= LIMIT) {
        printf("Error: Lista estática llena.\n");
        return NULL;
    }

    // Insertar el elemento en la lista
    mPosl pos = ++(M->lastPos); // Incrementar la posición y usarla
    strcpy(M->data[pos].id, "mmap");
    M->data[pos].address = p;
    sprintf(M->data[pos].address2, "%p", p);
    M->data[pos].size = s.st_size;
    strcpy(M->data[pos].file, fichero);
    M->data[pos].fd = protection;
    
    // Formatear la fecha
    char *aux = formatoFecha();
    strcpy(M->data[pos].fecha, aux);

    // Liberar memoria temporal
    free(aux);

    return p;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int do_AllocateMalloc(char *tr[], mList *M){
    if(tr[2]==NULL){
        printf("****Lista de bloques asignados para el proceso %d\n",getpid());
        listedMem("malloc", M);
        return -1;
    }else{

    void * address;
    struct memData * n =(struct memData *) malloc(sizeof(struct memData));
    char * buff,*aux;
    buff = (char *) malloc(40 * sizeof(char));


    if( (address = malloc(atoi(tr[2])))==NULL){
        perror("Imposible obtener memoria con malloc:\n");
        return -1;
    }
    printf("asignados %s at %p\n", tr[2], address);

    strcpy(n->id, "malloc");
    n->address = address;
    sprintf(buff, "%p", address);
    strcpy(n->address2, buff);
    n->size = atoi(tr[2]);
    aux=formatoFecha();
    strcpy(n->fecha, aux);
    free(aux);
    insertItemMem(n, M);
    free(buff);
    free(n);
    }
    return 0;
}

void do_AllocateMmap(char *tr[],mList *M)
{
    char *perm;
    void *p;
    int protection=0;


    if (tr[2]==NULL)
    {    printf("**Lista de bloques asignados para el proceso %d\n",getpid());
        listedMem("mmap", M);
        return;}
    if ((perm=tr[3])!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    if ((p=MapearFichero(tr[2],protection,M))==NULL)
        perror ("Imposible mapear fichero");
    else
        printf ("fichero %s mapeado en %p\n", tr[2], p);
}

void do_AllocateAttachShared(char *tr[], mList *M) {
    if (tr[2] == NULL) {
        printf("Uso: allocate -shared <key>\n");
        return;
    }

    key_t key;
    int shmid;
    void *shmaddr;
    struct shmid_ds shminfo;
    struct memData *n;

    // Obtener la clave desde los parámetros
    key = (key_t)strtoul(tr[2], NULL, 10);

    // Verificar si el segmento ya está creado
    if ((shmid = shmget(key, 0, 0666)) == -1) {
        perror("Error: Segmento de memoria compartida no encontrado");
        return;
    }
    
    if(key<=0){
        printf("Error al usar una clave 0 o negativa: Invalid argument\n");
        return;
    }
    
    // Adjuntar el segmento al espacio de memoria del proceso
    if ((shmaddr = shmat(shmid, NULL, 0)) == (void *)-1) {
        perror("Error al adjuntar segmento de memoria compartida");
        return;
    }


    if (shmctl(shmid, IPC_STAT, &shminfo) == -1) {
        perror("Error al obtener información del segmento");
        shmdt(shmaddr); // Desasociar el segmento si falla
        return;
    }

    printf("Memoria compartida adjuntada con clave %d y dirección %p\n", key, shmaddr);
    if (M->lastPos + 1 >= LIMIT) {
        printf("Error: Lista de memoria estática llena.\n");
        shmdt(shmaddr); 
        return;
    }
    n = (struct memData *)malloc(sizeof(struct memData));
    if (n == NULL) {
        perror("Error al asignar memoria para el nodo");
        shmdt(shmaddr);
        return;
    }
    strcpy(n->id, "shared");
    n->address = shmaddr;
    sprintf(n->address2, "%p", shmaddr);
    n->size = shminfo.shm_segsz;
    n->key = key;
    char *date = formatoFecha();
    strcpy(n->fecha, date);
    free(date);
    insertItemMem(n, M);
    free(n);
}

void do_AllocateCreateShared(char *tr[], mList *M) {
    if (tr[2] == NULL || tr[3] == NULL) {
        printf("Uso: allocate createshared <key> <size>\n");
        return;
    }
    

    key_t key;
    int size;
    int shmid;
    void *shmaddr;
    struct memData *n;

    // Obtener la clave y el tamaño desde los parámetros
    key = (key_t)strtoul(tr[2], NULL, 10); // Convertir clave a tipo key_t
    if(key<=0){
        printf("Imposible asignar memoria compartida clave 0 o negativa: Invalid argument\n");
        return;
    }
    size = atoi(tr[3]); // Convertir tamaño a entero

    if (size <= 0) {
        printf("Error: Tamaño inválido.\n");
        return;
    }

    // Crear segmento de memoria compartida
    if ((shmid = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
        perror("Error al crear segmento de memoria compartida");
        return;
    }

    // Asociar el segmento al espacio de memoria del proceso
    if ((shmaddr = shmat(shmid, NULL, 0)) == (void *)-1) {
        perror("Error al asociar segmento de memoria compartida");
        // Intentar eliminar el segmento recién creado si falló el shmat
        shmctl(shmid, IPC_RMID, NULL);
        return;
    }

    printf("Memoria compartida creada con clave %d y asociada en %p\n", key, shmaddr);

    // Verificar si hay espacio en la lista
    if (M->lastPos + 1 >= LIMIT) {
        printf("Error: Lista de memoria estática llena.\n");
        shmdt(shmaddr); // Desasociar memoria compartida
        shmctl(shmid, IPC_RMID, NULL); // Eliminar segmento
        return;
    }

    // Crear un nuevo nodo para la lista
    n = (struct memData *)malloc(sizeof(struct memData));
    if (n == NULL) {
        perror("Error al asignar memoria para el nodo");
        shmdt(shmaddr); // Desasociar memoria compartida
        shmctl(shmid, IPC_RMID, NULL);        return;
    }
    strcpy(n->id, "shared");
    n->address = shmaddr;
    sprintf(n->address2, "%p", shmaddr);
    n->size = size;
    n->key = key;
    char *date = formatoFecha();
    strcpy(n->fecha, date);
    free(date);

    insertItemMem(n, M);

    free(n);

}



void do_DeallocateMalloc(char *tr[],mList *M){

    if(tr[2]==NULL){
         printf("******Lista de bloques asignados para el proceso %d\n",getpid());
        listedMem("malloc", M);
        return;

    }else{
        int p=0;
        p=searchSizeMem(atoi(tr[2]), *M);
        if (p!=-1){
            free(M->data[p].address);
            deleteNodeMem(p,M);
        }else{
            printf("No se asignan bloques de 0 bytes");    
        }
    }

}

void do_DeallocateMmap(char *tr[],mList *M){
    if(tr[2]==NULL){
        printf("******Lista de bloques asignados para el proceso %d\n",getpid());
        listedMem("mmap", M);
        return;

    }else{
        int p;
        if (-1!= (p=searchFileMem(tr[2],*M))){
            munmap(M->data[p].address, M->data[p].size);
            deleteNodeMem(p, M);
        }else{
            printf("******Lista de bloques asignados para el proceso %d\n",getpid());
            listedMem("mmap", M);        }
    }
}

void do_DeallocateShared(char *tr[], mList *M) {
    if (tr[2] == NULL) {
        printf("** Lista de bloques asignados para el proceso %d **\n", getpid());
        listedMem("shared", M);
        return;
    }
 
    // Convertir clave de texto a entero
    int cl = strtoul(tr[2], NULL, 10);
    int p = searchKeyMem(cl, *M);
 
    if (p == -1) { // No se encontró la clave
        printf("No hay bloque de esa clave mapeado en el proceso\n");
        printf("** Lista de bloques asignados para el proceso %d **\n", getpid());
        listedMem("shared", M);
        return;
    }
 
    // Desasociar memoria compartida
    if (shmdt(M->data[p].address) == -1) {
        perror("Error al desasociar el bloque de memoria compartida");
        return; // Detener en caso de error
    }
 
    // Eliminar nodo de la lista
    deleteNodeMemsh(p, M);
    printf("Bloque de memoria compartida con clave %d desasociado correctamente.\n", cl);
}

void do_DeallocateDelkey (char *tr[],mList* M){

    if(tr[2]==NULL){
        printf("****Lista de bloques asignados para el proceso %d\n",getpid());
        listedMem("shared", M);
        return;
    }else{
    key_t clave;
    int id;
    char *key=tr[2];

    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("      delkey necesita clave_valida\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1){
        perror ("shmctl: imposible eliminar memoria compartida\n"); 
    
    }
    }
}

void do_DeallocateAddr(char *tr[],mList * M){

    int p = searchAddressMem(tr[1], *M);
    if (p==-1){
            printf("direction %s not assigned neither malloc, shared or mmap\n", tr[1]);
            return;
    }else{
        if (strcmp(M->data[p].id, "malloc")==0){
                free(M->data[p].address);
                deleteNodeMem(p, M);
        }else if (strcmp(M->data[p].id, "shared")==0){
                shmdt(M->data[p].address);
                deleteNodeMemsh(p, M);
        }else if (strcmp(M->data[p].id, "mmap")==0){
                munmap(M->data[p].address, M->data[p].size);
                deleteNodeMemsh(p, M);
        }
    }

}



ssize_t ReadFD (int df, void *p, size_t cont) {
    ssize_t tam;
    if ((tam = read(df, p, cont)) == -1) {
        perror("Error leyendo desde descriptor de archivo");
        return -1;
    }
    return tam;
}

void Cmd_Read(char *tr[]) {
    if (tr[1] == NULL || tr[2] == NULL || tr[3] == NULL) {
        printf("Uso: read <df> <direccion> <cantidad_bytes>\n");
        return;
    }

    int df = atoi(tr[1]); // Descriptor de archivo
    void *addr = (void *) strtoul(tr[2], NULL, 16);
    size_t cont = (size_t) strtoul(tr[3], NULL, 10);

    ssize_t n = read(df, addr, cont);
    if (n == -1) {
        perror("Error al leer desde el descriptor");
    } else {
        printf("Leídos %lld bytes desde descriptor %d a la dirección %p\n", (long long) n, df, addr);
    }
}

ssize_t readfile(const char *file, void *addr, size_t cont) {
    int fd;
    ssize_t tam;
    fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el archivo");
        return -1;
    }
    tam = read(fd, addr, cont);
    if (tam == -1) {
        perror("Error al leer el archivo");
    }
    close(fd);
    return tam;
}

void Cmd_Readfile(char *tr[]) {
    void *addr;
    size_t cont = (size_t)-1;
    ssize_t tam;

    if (tr[1] == NULL || tr[2] == NULL) {
        printf("Faltan parámetros: archivo y dirección\n");
        return;
    }

    addr = cadtop(tr[2]);
    if (addr == NULL) {
        fprintf(stderr, "Dirección inválida: %s\n", tr[2]);
        return;
    }

    if (tr[3] != NULL) {
        cont = (size_t)atoll(tr[3]);
    }

    tam = readfile(tr[1], addr, cont);
    if (tam != -1) {
        printf("Leídos %lld bytes desde '%s' en %p\n", (long long)tam, tr[1], addr);
    }
}



//Funcion auxiliar para abrir fd
ssize_t EscribirFichero (char *f, void *p, size_t cont,int overwrite)
{
    ssize_t  n;
    int df,aux, flags=O_CREAT | O_EXCL | O_WRONLY;

    if (overwrite)
        flags=O_CREAT | O_WRONLY | O_TRUNC;

    if ((df=open(f,flags,0777))==-1)
        return -1;

    if ((n=write(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return n;
}

//fd no abierto
int writefile(char *tr[]){
    if (tr[2] != NULL){
        if (0==strcmp(tr[1], "-o")){
            if (tr[2] != NULL && tr[3] != NULL && tr[4] != NULL){
                EscribirFichero(tr[2], (void *) strtol(tr[3], NULL, 16), strtol(tr[4],NULL,10), 1);
                printf("Escritos %ld bytes de %s en %s\n",strtol(tr[4],NULL,10), tr[3],tr[2]);
            }else{        printf("Error de parametros\n");}
        }else{
        // Verificar si el archivo existe
            if (access(tr[1], F_OK) == 0) {
                printf("Imposible escribir fichero: File exists\n");
            }else{
             if (tr[2] != NULL && tr[3] != NULL){
                EscribirFichero(tr[1], (void *) strtol(tr[2], NULL, 16), strtol(tr[3],NULL,10), 0);
                printf("Escritos %ld bytes de %s en %s\n",strtol(tr[3],NULL,10), tr[2],tr[1]);

            }else{        
                printf("Error de parametros\n");
            }
            }
            }
    }else{
        printf("Error de parametros\n");
    }
    return 0;
}

//fd ya abierto
int write_fd(char *tr[]) {
    if (tr[1] != NULL && tr[2] != NULL && tr[3] != NULL) {
        // Convertir el descriptor de archivo desde trozos[1]
        int fd = (int)strtol(tr[1], NULL, 10);
        if (fd < 0) {
            printf("Error: Descriptor de archivo inválido.\n");
            return -1;
        }

        // Obtener dirección de memoria y numero de bytes
        void *addr = (void *)strtol(tr[2], NULL, 16);
        size_t cont = (size_t)strtol(tr[3], NULL, 10);

        // Implementación de EscribirFichero directamente aquí
        ssize_t bytes_written = write(fd, addr, cont);
        if (bytes_written == -1) {
            perror("Error al escribir en el archivo");
            return -1;
        }

        printf("Escritos %zd bytes desde %p en descriptor de archivo %d\n", bytes_written, addr, fd);
        return 0;
    } else {
        printf("Error de parámetros\n");
        return -1;
    }
}



void memfill(char *tr[], int numtrozos, mList *M) {
    char addrStr[40];
    int pos, tam_bloque;
    
    
    if (numtrozos != 4) {
        perror("numero de parametros incorrecto(memfill)\n");
        return;
    }

    char *addr = (char *) strtol(tr[1], NULL, 16);
    char ch = '\0';
    if (tr[3][0] == '\'' && tr[3][2] == '\'') ch = tr[3][1];
    else if (strlen(tr[3]) == 1) ch = tr[3][0];
    else {
        perror("caracter invalido\n");
        return;
    }
    int num = atoi(tr[2]);
    if (addr == NULL) {
        perror("Direccion invalida\n");
        return;
    }
    if (num <= 0) {
        perror("Numero invalido\n");
        return;
    }
    
    sprintf(addrStr, "%p", addr);
    pos = searchAddressMem(addrStr, *M);
    if (pos == -1) {
        perror("La dirección no pertenece a un bloque de memoria válido\n");
        return;
    }
    
    tam_bloque = M->data[pos].size;
    if (num > tam_bloque) {
        perror("El tamaño solicitado excede el tamaño del bloque de memoria\n");
        return;
    }
    
    LlenarMemoria(addr,num,ch);
    printf("Llenando %d bytes de memoria con el byte  %c(%X) a partir de la direccion %p\n", num, ch, ch, addr);
}


void memdump(char *tr[], int numtrozos, mList *M) {
    char addrStr[40];
    int pos, tam_bloque;
    if (numtrozos != 3) {
        perror("numero de parametros incorrecto(memdump)\n");
        return;
    }

    void *addr = (void *) strtol(tr[1], NULL, 16);
    unsigned int num = atoi(tr[2]);
    if (addr == NULL) {
        perror("Direccion invalida\n");
        return;
    }
    if (num <= 0) {
        perror("Numero invalido\n");
        return;
    }
    
    sprintf(addrStr, "%p", addr);
    pos = searchAddressMem(addrStr, *M);
    if (pos == -1) {
        perror("La dirección no pertenece a un bloque de memoria válido\n");
        return;
    }
    
    tam_bloque = M->data[pos].size;
    if (num > tam_bloque) {
        perror("El tamaño solicitado excede el tamaño del bloque de memoria\n");
        return;
    }
    
    printf("Volcando %d bytes desde la direccion %p\n", num, addr);

    unsigned char *recorrer = (unsigned char *) addr;
    for (int i = 0; i < num; i++) {
        printf("%2c ", recorrer[i]);
  
    }
    printf("\n");
    for (int i = 0; i < num && recorrer[i] != 0; i++) {
        printf("%2X ", (int)recorrer[i]);
    }
    printf("\n");
}


void Do_pmap () { 
   pid_t pid;       //hace el pmap (o equivalente) del proceso actual
   char elpid[32];
   char *argv[4]={"pmap",elpid,NULL};
   
   sprintf (elpid,"%d", (int) getpid());
   if ((pid=fork())==-1){
      perror ("Imposible crear proceso");
      return;
      }
   if (pid==0){
      if (execvp(argv[0],argv)==-1)
         perror("cannot execute pmap (linux, solaris)");
         
      argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;   
      if (execvp(argv[0],argv)==-1)/*No hay pmap, probamos procstat FreeBSD */
         perror("cannot execute procstat (FreeBSD)");
         
      argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;    
            if (execvp(argv[0],argv)==-1)  //probamos procmap OpenBSD/
         perror("cannot execute procmap (OpenBSD)");
         
      argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
      if (execvp(argv[0],argv)==-1) //probamos vmmap Mac-OS/
         perror("cannot execute vmmap (Mac-OS)");      
      exit(1);
  }
  waitpid (pid,NULL,0);
}

int global1,global2,global3;
int global_ini1 = 1,global_ini2 = 2, global_ini3 = 3;

void memory(char *tr[], int numtrozos, mList *M) {
    int static estatica1;
    int static estatica2;
    int static estatica3;

    int static estatica_ini1 = 1;
    int static estatica_ini2 = 2;
    int static estatica_ini3 = 3;

    int local1;
    int local2;
    int local3;
    
    if (numtrozos != 2) {
        perror("Numero de argumentos incorrecto");
        return;
    }

    if (strcmp(tr[1], "-funcs") == 0) {
        printf("Funciones de programa:\n");
        printf(" - memory: %p\n", (void*) &memory);
        printf(" - procesarEntrada: %p\n", (void*) &procesarEntrada);
        printf(" - memdump: %p\n", (void*) &memdump);

        printf("\nFunciones de libreria:\n");
        printf(" - printf: %p\n", (void*) &printf);
        printf(" - strcmp: %p\n", (void*) &strcmp);
        printf(" - strtok: %p\n", (void*) &strtok);
    }

    else if (strcmp(tr[1], "-vars") == 0) {
        printf("Variables  locales: %p,  %p,  %p\n" ,(void*)&local1, (void*)&local2, (void*)&local3);
        printf("Variables globales: %p,  %p,  %p\n" ,(void*)&global1, (void*)&global2, (void*)&global3);
        printf("Var (N.I) globales: %p,  %p,  %p\n" ,(void*)&global_ini1, (void*)&global_ini2, (void*)&global_ini3);
        printf("Variables staticas: %p,  %p,  %p\n" ,(void*)&estatica1, (void*)&estatica2, (void*)&estatica3);
        printf("Var (N.I) staticas: %p,  %p,  %p\n" ,(void*)&estatica_ini1, (void*)&estatica_ini2, (void*)&estatica_ini3);
        
    }
    
    else if (strcmp(tr[1], "-blocks") == 0){
        pid_t pid = getpid();
        if (isEmptyListMem(M)) {
           printf("**Lista de bloques asignados para el proceso %d\n", pid);
        return;
    }
        printf("**Lista de bloques asignados para el proceso %d\n", pid);

        for (int i = 0; i <= M->lastPos; i++) {
            printf("%p\t%d\t%s\t%s\n",M->data[i].address,M->data[i].size, M->data[i].fecha, M->data[i].id);
        }
    }
    
    else if (strcmp(tr[1], "-pmap") == 0){
        Do_pmap();
    }
    
    else perror("Incorrecto\n");

}
void Recursiva (int n)
{
  char automatico[TAMANO];
  static char estatico[TAMANO];
  

  printf ("parametro:%3d(%p) array %p, arr estatico %p\n",n,&n,automatico, estatico);

  if (n>0)
    Recursiva(n-1);
}



