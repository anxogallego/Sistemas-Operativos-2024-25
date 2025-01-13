/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * CREADO POR: * * * * * * * * * * *
 * * * * Anxo Gallego Mato: anxo.gallego@udc.es  * * * *
 * * Javier Armada Rodríguez: javier.armada@udc.es * * *
 * * * *  Hugo Dopazo Marta:hugo.dopazo@udc.es * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "Library.h"
#include "hist_list.h"
#include "file_list.h"
#include "dir_list.h"
#include "p0.h"
#include "p1.h"
#include "p2.h"
#include "p3.h"

//////////////////////////////////////////////////////////
void authors(char *trozos[]){

    if (trozos[1] == NULL){
        printf("Anxo Gallego Mato anxo.gallego@udc.es\nJavier Armada Rodrígez javier.armada@udc.es\nHugo Dopazo Marta hugo.dopazo@udc.es\n");
    }else if (strcmp(trozos[1], "-l") == 0){
        printf("Logins: anxo.gallego@udc.es javier.armada@udc.es hugo.dopazo@udc.es \n");
    }else if(strcmp(trozos[1], "-n") == 0)
        printf("Nombres: Anxo Gallego Mato Javier Armada Rodríguez Hugo Dopazo Marta \n");
    
}


void pid(){
    pid_t pid = getpid();
    printf("%d \n", pid);
}

void ppid(){
    pid_t ppid = getppid();
    printf("%d \n", ppid);
}


void cd(char *trozos[]){

    if(trozos[1] == NULL){
        char buf[100];
        printf("%s \n", getcwd(buf, 100));
    }else{
        chdir(trozos[1]);
    }
        

}
/*date [-t|-d] Prints the current date in the format DD/MM/YYYY and the current
time in the format hh:mm:ss.*/
struct tm getDateTime(){
    time_t actualTime;
    struct tm *struct_time;

    time(&actualTime);
    struct_time = localtime(&actualTime);
    return  *struct_time;
}


void date(char *trozos[]){
    struct tm struct_date = getDateTime();
    char date[15];
    char hour[15];
    if(trozos[1]==NULL){
        strftime(date, sizeof(date), "%d/%m/%Y", &struct_date);
        printf("%s ",date);
        strftime(hour, sizeof(hour), "%H:%M:%S", &struct_date);
        printf("%s\n",hour);
    }else if(strcmp("-t",trozos[1])==0){
        strftime(date, sizeof(date), "%H:%M:%S", &struct_date);
        printf("%s\n",date);
    }else if(strcmp("-d",trozos[1])==0){
         strftime(date, sizeof(date), "%d/%m/%Y", &struct_date);
        printf("%s\n",date);
    }
}


void infosys(){
    struct utsname unameData;
    if (uname(&unameData) < 0) {
      perror("uname");
      exit(EXIT_FAILURE);
    }
    printf("%s (%s) %s %s %s\n",unameData.nodename,unameData.machine, unameData.sysname, unameData.release, unameData.version );
}


//funciones auxiliares Historic
void printComandHist(tList L) {
    int commandNumber = 0;

    while (L.data[commandNumber] != NULL) {
        printf("%d: %s\n", commandNumber, L.data[commandNumber]);
        commandNumber++;
    }
}


void historic(tList *L, char *tr[],int numtrozos, char * envp[]) {
    //caso 1 : solo historic: mostrar historial
    if(numtrozos==1){
        printComandHist(*L);
        return;
    }
    if (tr[1][0] == '-'){
        //caso Historic -N: mostar ultimos N comandos
        int num;
        char *cadena;
        cadena = strtok(tr[1],"-");
        num = atoi(cadena);
        if(num == 0){
            printf("%s: Invalid parameter\n", tr[1]);
            return;
        }
        for (int i = L->lastPos-num; i < L->lastPos; i++) {
            printf("%d: %s\n", i, L->data[i]);
        }

    }else{
        int num=atoi(tr[1]);
        if(num<L->lastPos + 1 && num>=0){
            printf("Ejecutando hist(%d): %s\n",num,L->data[num]);
            int i=TrocearCadena(L->data[num],tr);
            procesarEntrada(tr, false, L,NULL,NULL,NULL,NULL,i, envp);  
        }else{
            perror("No se reconocio ese numero");
            return;
        }
        
    } 
} 


// Función para abrir archivos
void Cmd_open (char * tr[],fileList *F){
    int i,df, mode=0;
    
    if (tr[1]==NULL) { //no hay parametro
        printFileList(*F);
        return;
    }
    for (i=2; tr[i]!=NULL; i++)
      if (!strcmp(tr[i],"cr")){ 
        mode|=O_CREAT;
        strcpy(tr[i], "O_CREAT");
      }else if (!strcmp(tr[i],"ex")){
        mode|=O_EXCL;
        strcpy(tr[i], "O_EXCL");
      }else if (!strcmp(tr[i],"ro")){
        mode|=O_RDONLY; 
        strcpy(tr[i], "O_RDONLY");
      }else if (!strcmp(tr[i],"wo")){ 
        mode|=O_WRONLY;
        strcpy(tr[i], "O_WRONLY");
      }else if (!strcmp(tr[i],"rw")){ 
        mode|=O_RDWR;
        strcpy(tr[i], "O_RDWR");
      }else if (!strcmp(tr[i],"ap")){ 
        mode|=O_APPEND;
        strcpy(tr[i], "O_APPEND");
      }else if (!strcmp(tr[i],"tr")){ 
        mode|=O_TRUNC; 
        strcpy(tr[i], "O_TRUNC");
      }else break;
      
    if ((df=open(tr[1],mode,0777))==-1){
        perror ("Imposible abrir fichero");
        return;
    }else if(mode==0){
         insertFile(F,df,tr[1],"");
    }else{
        insertFile(F, df, tr[1], tr[i-1]);
    
    }
}


void Cmd_dup (char *tr[],fileList *F)
{
    int df, duplicado;
    char aux[BUFFER],*p;

    if (tr[1]==NULL || (df=atoi(tr[1]))<0 ) { 
        printFileList(*F);
        return;
    }
  
    if (strcmp(F->data[df].nombre,"")==0){
        printf("Imposible duplicar descriptor: Bad file descriptor\n");
        return;
    }

    if((duplicado = dup(df))==-1){
        perror("Acción inválida");
    }else{
        p=getFileName(*F,df);
        sprintf (aux,"dup %d (%s)",df, p);
        insertFile(F, duplicado, aux,getFileMode(*F,df));
    }
}


void Cmd_close (char *tr[],fileList *F)
{
    int df;

    if (tr[1]==NULL || (df=atoi(tr[1]))<0) { 
        printFileList(*F);
          return;
    }

    if (close(df)==-1){
        perror("Imposible cerrar descriptor");
    }else{
        deleteFile(F,df);

    }
 }



//////////////////////////////////////////////////////////
void help(char *tr[]) {
    if (tr[1] == NULL)
        printf("authors  pid  ppid cd[dir] date [-t|-d]  historic [N|-N]  open [file]mode  close[df]  dup[df]  infosys  help[cmd]  quit  exit  bye  makefile makedir listfile cwd listdir reclist revlist erase delrec allocate dellocate memfill memdump memory readfile writefile read write recurse "
               "getuid setuid showvar changevar subsvar environ fork search exec execpri fg fgpri back backpri listjobs deljobs\n");
    else if (strcmp("authors",tr[1]) == 0)
        printf("authors [-n|-l]\tMuestr los nombres y/o logins de los autores\n");
    else if (strcmp("pid",tr[1]) == 0)
        printf("pid [-p] Muestra\tel pid del shell o de su proceso padre\n");
    else if (strcmp("ppid",tr[1]) == 0)
        printf("ppid 	Muestra\tel pid del proceso padre del shell\n");
    else if (strcmp("date",tr[1]) == 0)
        printf("date [-d|-t]\tMuestra la fecha y/o la hora actual\n");
    else if (strcmp("historic",tr[1]) == 0)
        printf("historic [-c|-N|N]\t\tMuestra (o borra)el historico de comandos\n\t-N: muestra los N primeros\n\t-c: borra el historico\n\tN: repite el comando N");
    else if (strcmp("open",tr[1]) == 0)
        printf("open fich m1 m2...\t\tAbre el fichero fich\n\tlo anade a la lista de ficheros abierto del shell\n\tm1, m2..es el modo de apertura (or bit a bit de los siguientes)\n\tcr: O_CREAT\tap: O_APPEND\n\tex: O_EXCL\tro: O_RDONLY\n\trw: O_RDWR\two: O_WRONLY\n\ttr: O_TRUNC\n");
    else if (strcmp("close",tr[1]) == 0)
        printf("close df\tCierra el descriptor df y elimina el correspondiente fichero de la lista de ficheros abiertos\n");
    else if (strcmp("dup",tr[1]) == 0)
        printf("dup df\tDuplica el descriptor de fichero df y anade una nueva entrada a la lista ficheros abiertos\n");
    else if (strcmp("infosys",tr[1]) == 0)
        printf("infosys\tMuestra informacion de la maquina donde corre el shell\n");
    else if (strcmp("help",tr[1]) == 0)
        printf("help [cmd|-lt|-T]\t Muestra ayuda sobre los comando\n\t-lt: lista de topics de ayuda\n\t-T topic: lista comandos sobre este topic\n\tcmd: info sobre el comando cmd\n");
    else if (strcmp("quit",tr[1]) == 0)
        printf("quit\tTermina la ejecucion del shell\n");
    else if (strcmp("exit",tr[1]) == 0)
        printf("exit\tTermina la ejecucion del shell\n");
    else if (strcmp("bye",tr[1]) == 0)
        printf("bye\tTermina la ejecucion del shell\n");
    else if (strcmp("makefile", tr[1])==0) 
        printf("makefile [name]\tCrea un fichero de nombre name\n");
    else if (strcmp("makedir", tr[1])==0) 
        printf("makedir [name]\tCrea un directorio de nombre name\n");
    else if (strcmp("cwd", tr[1])==0) 
        printf("cwd\tMuestra el directorio actual del shell\n");
    else if (strcmp("listfile", tr[1])==0) 
        printf("listfile [-long][-link][-acc] name1 name2... \tlista ficheros\n-long: listado largo\n-acc: acesstime\n-link: si es enlace simbolico, el path contenido\n");
    else if (strcmp("listdir", tr[1])==0) 
        printf("listdir [-hid][-long][-link][-acc] n1 n2... \tlista contenidos de directorios\n-long: listado largo\n-hid: incluye los ficheros ocultos\n-acc: acesstime\n-link: si es enlace simbolico, el path contenido\n");    
    else if (strcmp("reclist", tr[1])==0)
        printf("reclist [-hid][-long][-link][-acc] n1 n2... \tlista recursivamente contenidos de directorios (subdirs despues)\n-hid: incluye los ficheros ocultos\n-long: listado largo\n-acc: acesstime\n-link: si es enlace simbolico, el path contenido\n");    
    else if (strcmp("revlist", tr[1])==0) 
        printf("revlist [-hid][-long][-link][-acc] n1 n2... \tlista recursivamente contenidos de directorios (subdirs antes)\n-hid: incluye los ficheros ocultos\n-long: listado largo\n-acc: acesstime\n-link: si es enlace simbolico, el path contenido\n");   
    else if (strcmp("erase",tr[1]) == 0)
        printf("erase [name1 name2 ..]\tBorra ficheros o directorios vacios\n");   
    else if (strcmp("delrec",tr[1]) == 0)
        printf("delrec [name1 name2 ..]\tBorra ficheros o directorios no vacios recursivamente\n");
    else if (strcmp("erase",tr[1]) == 0)
        printf("erase [name1 name2 ..]\tBorra ficheros o directorios vacios\n"); 
    else if (strcmp("allocate",tr[1]) == 0)
        printf("allocate [-malloc|-shared|-createshared|-mmap]... Asigna un bloque de memoria \n\t-malloc tam: asigna un bloque malloc de tamano tam\n\t-createshared cl tam: asigna (creando) el bloque de memoria compartida de clave cl y tamano tam \n\t-shared cl: asigna el bloque de memoria compartida (ya existente) de clave cl\n\t-mmap fich perm: mapea el fichero fich, perm son los permisos\n"); 
    else if (strcmp("deallocate",tr[1]) == 0)
        printf("deallocate [-malloc|-shared|-delkey|-mmap|addr]... Desasigna un bloque de memoria\n\t-malloc tam: desasigna el bloque malloc de tamano tam\n\t-shared cl: desasigna (desmapea) el bloque de memoria compartida de clave cl\n-delkey cl: elimina del sistema (sin desmapear) la clave de memoria cl\n\t-mmap fich: desmapea el fichero mapeado fich\naddr: desasigna el bloque de memoria en la direccion addr\n"); 
    else if (strcmp("memfill",tr[1]) == 0)
        printf("memfill addr cont byte 	Llena la memoria a partir de addr con byte\n"); 
    else if (strcmp("memdump",tr[1]) == 0)
        printf("memdump addr cont 	Vuelca en pantallas los contenidos (cont bytes) de la posicion de memoria addr\n"); 
    else if (strcmp("memory",tr[1]) == 0)
        printf("memory [-blocks|-funcs|-vars|-all|-pmap] ..	Muestra muestra detalles de la memoria del proceso\n\t\t-blocks: los bloques de memoria asignados\n\t\t-funcs: las direcciones de las funciones\n\t\t-vars: las direcciones de las variables\n\t\t:-all: todo\n\t\t-pmap: muestra la salida del comando pmap(o similar)\n"); 
    else if (strcmp("readfile",tr[1]) == 0)
        printf("readfile fiche addr cont 	Lee cont bytes desde fich a la direccion addr\n"); 
    else if (strcmp("writefile",tr[1]) == 0)
        printf("writefile [-o] fiche addr cont 	Escribe cont bytes desde la direccion addr a fich (-o sobreescribe)\n"); 
    else if (strcmp("read",tr[1]) == 0)
        printf("read df addr cont	Transfiere cont bytes del fichero descrito por df a la dirección addr\n"); 
    else if (strcmp("write",tr[1]) == 0)
        printf("write df addr cont	Transfiere cont bytes desde la dirección addr al fichero descrito por df\n"); 
    else if (strcmp("recurse",tr[1]) == 0)
        printf("recurse [n]	Invoca a la funcion recursiva n veces\n"); 
     else if (strcmp("setuid",tr[1]) == 0)
        printf("setuid [-l] id	Cambia las credenciales del proceso que ejecuta el shell\nid: establece la credencial al valor numerico id\n-l id: establece la credencial a login id\n");
    else if (strcmp("getuid",tr[1]) == 0)
        printf("Muestra las credenciales del proceso que ejecuta el shell\n");
    else if (strcmp("showvar",tr[1]) == 0)
        printf("showvar var	Muestra el valor y las direcciones de la variable de entorno var\n");
    else if (strcmp("changevar",tr[1]) == 0)
        printf("changevar [-a|-e|-p] var valor	Cambia el valor de una variable de entorno\n	-a: accede por el tercer arg de main\n	-e: accede mediante environ\n	-p: accede mediante putenv\n");
    else if (strcmp("subsvar",tr[1]) == 0)
        printf("subsvar [-a|-e] var1 var2 valor	Sustituye la variable de entorno var1\n	con var2=valor\n	-a: accede por el tercer arg de main\n	-e: accede mediante environ\n");    
    else if (strcmp("fg",tr[1]) == 0)
        printf("fg prog args...	Crea un proceso que ejecuta en primer plano prog con argumentos\n");
    else if (strcmp("fgpri",tr[1]) == 0)
        printf("fgpri prio prog args...	Crea un proceso que ejecuta en primer plano prog con argumentos  con la prioridad cambiada a prio\n");
    else if (strcmp("back",tr[1]) == 0)
        printf("back prog args...	Crea un proceso que ejecuta en segundo plano prog con argumentos\n");
    else if (strcmp("backpri",tr[1]) == 0)
        printf("backpri prio prog args...	Crea un proceso que ejecuta en segundo plano prog con argumentos con la prioridad cambiada a prio\n");
    else if (strcmp("fork",tr[1]) == 0)
        printf("fork 	El shell hace fork y queda en espera a que su hijo termine\n");
    else if (strcmp("exec",tr[1]) == 0)
        printf("exec VAR1 VAR2 ..prog args....[@pri]	Ejecuta, sin crear proceso,prog con argumentos\n	 en un entorno que contiene solo las variables VAR1, VAR2...\n");
    else if (strcmp("execpri",tr[1]) == 0)
        printf("execpri prio prog args....	Ejecuta, sin crear proceso, prog con argumentos con la prioridad cambiada a prio\n");  
    else if (strcmp("environ",tr[1]) == 0)
        printf("environ [-environ|-addr] 	 Muestra el entorno del proceso\n	-environ: accede usando environ (en lugar del tercer arg de main)\n	-addr: muestra el valor y donde se almacenan environ y el 3er arg main\n");
    else if (strcmp("listjobs",tr[1]) == 0)
        printf("listjobs 	Lista los procesos en segundo plano\n");
    else if (strcmp("deljobs",tr[1]) == 0)
        printf("deljobs [-term][-sig]	Elimina los procesos de la lista procesos en sp\n	-term: los terminados\n	-sig: los terminados por senal\n");
    else if (strcmp("search",tr[1]) == 0)
        printf("search [-add|-del|-clear|-path]..	Manipula o muestra la ruta de busqueda del shell (path)\n	-add dir: aniade dir a la ruta de busqueda(equiv +dir)\n	-del dir: elimina dir de la ruta de busqueda (equiv -dir)\n	-clear: vacia la ruta de busqueda\n	-path: importa el PATH en la ruta de busqueda\n"); 
}



int TrocearCadena(char* cadena, char* tr[]){
    int i=1;
    if((tr[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((tr[i]=strtok(NULL, " \n\t"))!=NULL)
        i++;
    return i;
}

void procesarEntrada(char **trozos, bool *terminado, tList *L,fileList *F,mList *M,listProc *P, dirList *D, int numtrozos, char * envp[]){
    
    if(strcmp(trozos[0], "quit") == 0 || strcmp(trozos[0], "exit") == 0 || strcmp(trozos[0], "bye") == 0){
        *terminado = true;

    }else if (strcmp(trozos[0], "authors") == 0)
        authors(trozos);

    else if (strcmp(trozos[0], "pid") == 0)
        pid();

    else if (strcmp(trozos[0], "ppid") == 0)
        ppid();

    else if (strcmp(trozos[0], "cd") == 0)
        cd(trozos);
    
    else if (strcmp(trozos[0], "date") == 0)
        date(trozos);

    else if (strcmp(trozos[0], "infosys") == 0)
        infosys();
    
    else if (strcmp(trozos[0], "historic") == 0)
        historic(L,trozos,numtrozos, envp);
    else if (strcmp(trozos[0], "help") == 0)
        help(trozos);
    else if (strcmp(trozos[0], "open") == 0)
        Cmd_open(trozos,F);
    else if (strcmp(trozos[0], "dup") == 0)
        Cmd_dup(trozos,F);
    else if (strcmp(trozos[0], "close") == 0)
        Cmd_close(trozos,F);
    else if (strcmp(trozos[0], "makefile") == 0)
        cmd_makefile(trozos); 
    else if (strcmp(trozos[0], "makedir") == 0)
        cmd_makedir(trozos); 
    else if (strcmp(trozos[0], "cwd") == 0)
        cwd(trozos);  
    else if (strcmp(trozos[0], "erase") == 0)
        erase(trozos,numtrozos);   
    else if (strcmp(trozos[0], "listfile") == 0)
        listfile(trozos,numtrozos); 
    else if (strcmp(trozos[0], "listdir") == 0)
        listdir(trozos,numtrozos); 
    else if (strcmp(trozos[0], "delrec") == 0)
        delrec(trozos,numtrozos); 
    else if (strcmp(trozos[0], "reclist") == 0)
        reclist(trozos,numtrozos); 
    else if (strcmp(trozos[0], "revlist") == 0)
        revlist(trozos,numtrozos); 
    else if (strcmp(trozos[0], "write") == 0)
        write_fd(trozos); 
    else if (strcmp(trozos[0], "writefile") == 0)
        writefile(trozos);
    else if (strcmp(trozos[0], "read") == 0)
        Cmd_Read(trozos); 
    else if (strcmp(trozos[0], "readfile") == 0)
        Cmd_Readfile(trozos);  
    else if (strcmp(trozos[0], "memfill") == 0)
        memfill(trozos,numtrozos,M);  
    else if (strcmp(trozos[0], "memdump") == 0)
        memdump(trozos,numtrozos,M);
    else if (strcmp(trozos[0], "memory") == 0)
        memory(trozos,numtrozos,M);
    else if (strcmp(trozos[0], "recurse") == 0)
        if(trozos[1]==NULL){
            printf("Introduzca un número\n");
        }else{
        Recursiva(atoi(trozos[1]));  
    }else if (strcmp(trozos[0], "allocate") == 0){
        if(trozos[1]==NULL){
            printf("******Lista de bloques asignados para el proceso %d\n",getpid());
            listedMem("malloc", M);
            listedMem("mmap", M);
            listedMem("shared", M);
        }else if(strcmp(trozos[1],"-malloc")==0){
            do_AllocateMalloc(trozos,M); 
        }else if(strcmp(trozos[1],"-mmap")==0){
             do_AllocateMmap(trozos,M); 
        }else if(strcmp(trozos[1],"-shared")==0){
             do_AllocateAttachShared(trozos,M); 
        }else if(strcmp(trozos[1],"-createshared")==0){
             do_AllocateCreateShared(trozos,M); 
        }else{
            printf("uso: allocate [-malloc|-shared|-createshared|-mmap] ....");
        }
    }else if (strcmp(trozos[0], "deallocate") == 0){
            if(trozos[1]==NULL){
            printf("******Lista de bloques asignados para el proceso %d\n",getpid());
            listedMem("malloc", M);
            listedMem("mmap", M);
            listedMem("shared", M);
        }else if(strcmp(trozos[1],"-malloc")==0){
            do_DeallocateMalloc(trozos,M); 
        }else if(strcmp(trozos[1],"-shared")==0){
            do_DeallocateShared(trozos,M); 
        }else if(strcmp(trozos[1],"-delkey")==0){
            do_DeallocateDelkey(trozos,M); 
        }else if(strcmp(trozos[1],"-mmap")==0){
            do_DeallocateMmap(trozos,M); 
        }else{
            do_DeallocateAddr(trozos,M); 
        }
    }else if (strcmp(trozos[0], "getuid") == 0){
        Cmd_getuid();
    }else if(strcmp(trozos[0], "setuid") == 0){
        Cmd_setuid(trozos);
    }else if(strcmp(trozos[0], "listjobs") == 0){
        UpdateJobs(P);
        listjobs(P);
    }else if(strcmp(trozos[0], "deljobs") == 0){
        deljobs(trozos,P);
    }else if(strcmp(trozos[0], "environ") == 0){
        show_environ(trozos, numtrozos, envp);
    }else if(strcmp(trozos[0], "search") == 0){
        search(D, trozos);
    }else if(strcmp(trozos[0], "showvar") == 0){
        showvar(trozos, numtrozos, envp);
    }else if(strcmp(trozos[0], "changevar") == 0){
        changevar(trozos, numtrozos, envp);
    }else if(strcmp(trozos[0], "subsvar") == 0){
        subsvar(trozos, envp);
    }else if(strcmp(trozos[0], "fork") == 0){
        Cmd_fork(trozos, P);
    }else if(strcmp(trozos[0], "exec") == 0){
        exec(trozos, D);
    }else if(strcmp(trozos[0], "execpri") == 0){
        execpri(trozos, D);
    }else if(strcmp(trozos[0], "fg") == 0){
        fg(trozos, envp, D,numtrozos);
    }else if(strcmp(trozos[0], "fgpri") == 0){
        fgpri(trozos, envp, D,numtrozos);
    }else if(strcmp(trozos[0], "back") == 0){
        Back(trozos, P, D, envp);
    }else if(strcmp(trozos[0], "backpri") == 0){
        Backpri(trozos, P, D, envp);
    }else{
        programa_externo(trozos,envp,D);
    }
}




