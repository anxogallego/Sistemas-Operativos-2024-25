/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * CREADO POR: * * * * * * * * * * *
 * * * * Anxo Gallego Mato: anxo.gallego@udc.es  * * * *
 * * Javier Armada Rodríguez: javier.armada@udc.es * * *
 * * * *  Hugo Dopazo Marta:hugo.dopazo@udc.es * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "Library.h"
#include "hist_list.h"
#include "file_list.h"
#include "mem_list.h"
#include "proc_list.h"
#include "dir_list.h"
#include "p3.h"
#include "p0.h"


struct SEN {
    char *nombre;
    int senal;
};
/*las siguientes funciones nos permiten obtener el nombre de una senal a partir
del nÃºmero y viceversa */
static struct SEN sigstrnum[]={   
	{"HUP", SIGHUP},
	{"INT", SIGINT},
	{"QUIT", SIGQUIT},
	{"ILL", SIGILL}, 
	{"TRAP", SIGTRAP},
	{"ABRT", SIGABRT},
	{"IOT", SIGIOT},
	{"BUS", SIGBUS},
	{"FPE", SIGFPE},
	{"KILL", SIGKILL},
	{"USR1", SIGUSR1},
	{"SEGV", SIGSEGV},
	{"USR2", SIGUSR2}, 
	{"PIPE", SIGPIPE},
	{"ALRM", SIGALRM},
	{"TERM", SIGTERM},
	{"CHLD", SIGCHLD},
	{"CONT", SIGCONT},
	{"STOP", SIGSTOP},
	{"TSTP", SIGTSTP}, 
	{"TTIN", SIGTTIN},
	{"TTOU", SIGTTOU},
	{"URG", SIGURG},
	{"XCPU", SIGXCPU},
	{"XFSZ", SIGXFSZ},
	{"VTALRM", SIGVTALRM},
	{"PROF", SIGPROF},
	{"WINCH", SIGWINCH}, 
	{"IO", SIGIO},
	{"SYS", SIGSYS},
/*senales que no hay en todas partes*/
#ifdef SIGPOLL
	{"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
	{"PWR", SIGPWR},
#endif
#ifdef SIGEMT
	{"EMT", SIGEMT},
#endif
#ifdef SIGINFO
	{"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
	{"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
	{"CLD", SIGCLD},
#endif
#ifdef SIGLOST
	{"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
	{"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
	{"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
	{"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
	{"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
	{"WAITING", SIGWAITING},
#endif
 	{NULL,-1},
};  

int ValorSenal(char * sen)  /*devuelve el numero de senial a partir del nombre*/ 
{ 
  int i;
  for (i=0; sigstrnum[i].nombre!=NULL; i++)
  	if (!strcmp(sen, sigstrnum[i].nombre))
		return sigstrnum[i].senal;
  return -1;
}


char *NombreSenal(int sen)  /*devuelve el nombre senal a partir de la senal*/ 
{			/* para sitios donde no hay sig2str*/
 int i;
  for (i=0; sigstrnum[i].nombre!=NULL; i++)
  	if (sen==sigstrnum[i].senal)
		return sigstrnum[i].nombre;
 return ("SIGUNKNOWN");
}

int BuscarVariable (char * var, char *e[]) {                                           
  int pos=0;
  char aux[MAXVAR];
  
  strcpy (aux,var);
  strcat (aux,"=");
  
  while (e[pos]!=NULL){
    if (!strncmp(e[pos],aux,strlen(aux)))
      return (pos);
    else 
      pos++;
  } 
  errno=ENOENT;   /*no hay tal variable*/
  return(-1);
}

int CambiarVariable(char * var, char * valor, char *e[]){                                                      
  int pos;
  char *aux;
   
  if ((pos = BuscarVariable(var, e)) == -1)
    return -1;  
 
  if ((aux = (char *)malloc(strlen(var) + strlen(valor) + 2)) == NULL)
    return -1;
	
	
  strcpy(aux, var);
  strcat(aux, "=");
  strcat(aux, valor);
  e[pos] = aux;
  free(aux);
  return pos;
}


/*fin array sigstrnum */
void getStatus(int pid, int p, listProc* P){
    int estado;
    int w;
    char * buff;
    buff = (char *) malloc(50 * sizeof(char));

    w=waitpid(pid,&estado, WNOHANG |WUNTRACED |WCONTINUED);
    if (w==0){
        sprintf(buff,"Running %s","");
        strcpy(P->data[p].estado, buff);
        return;
    }
    if (WIFEXITED(estado)){//Terminated normal
        sprintf(buff,"TERMINADO %d", WEXITSTATUS(estado));
        P->data[p].terminado = 0;
        strcpy(P->data[p].estado, buff);
    }else if (WIFSIGNALED(estado)){//Terminated by signal
        sprintf(buff,"TERMINADO POR SEÑAL %s", NombreSenal(WTERMSIG(estado)));
        P->data[p].terminado = 0;
        strcpy(P->data[p].estado, buff);
    }else if (WIFSTOPPED(estado)){//Stopped
        sprintf(buff,"Stopped %s", NombreSenal(WSTOPSIG(estado)));
        strcpy(P->data[p].estado, buff);
    }else if (WIFCONTINUED(estado)){//Running
        sprintf(buff,"Running %s","");
        strcpy(P->data[p].estado, buff);
    }
    free(buff);
}

void setCredenciales(char *tr[]){
    int nuevoUid = atoi(tr[1]);
    if (seteuid((uid_t)nuevoUid) != 0) {
        perror("Imposible cambiar credencial");
        return;
    }
}



void Cmd_getuid(){
    uid_t real = getuid(), efectiva = geteuid();
    printf("Credencial real: %d, (%s)\n", real, NombreSenal(real));
    printf("Credencial efectiva: %d, (%s)\n", efectiva, NombreSenal(efectiva));
}


void Cmd_setuid(char *tr[]) {
    if(tr[1] != NULL){
    if (tr[2] != NULL && !strcmp(tr[2],"-l")) {
        // Obtener el login de tr[2] 
        const char *login = tr[2];

        // Usar getpwnam() para obtener la estructura passwd con la información del usuario
        struct passwd *pwd = getpwnam(login);

        if (pwd == NULL) {
            // Si el usuario no se encuentra, imprimir error
            perror("No se pudo encontrar el usuario");
            return;
        } else {
            // Establecer el uid del login obtenido
            if (seteuid(pwd->pw_uid) != 0) {
                perror("Imposible cambiar credencial");
                return;
            }
        }
    } else {
        setCredenciales(tr);
        return;
    }
    Cmd_getuid();
    }
}

void listjobs(listProc *P){
    if(P->lastPos==-1){
        perror("Lista de procesos vacía.");
        return;
    }else{
        int i;
        for(i=0; i<=P->lastPos; i++){
            if (P->data[i].terminado == 1){
                getStatus(P->data[i].pid, i, P);
            }
            __uid_t nombre = getuid();
            struct passwd *p;
            if ((p=getpwuid(nombre)) == NULL){
                p= (struct passwd *) "anxo";
            }

            printf(" %d\t %s p=%d %s %s (000) ", P->data[i].pid,p->pw_name, getpriority(PRIO_PROCESS, P->data[i].pid), P->data[i].inicio,P->data[i].estado);
            printf(" %s\n ", P->data[i].command);

        }
    }
}

void deljobs(char * trozos[], listProc *P){
    int i, aux;
    UpdateJobs(P);
    if (trozos[1] == NULL){
        listjobs(P);
    }else if (trozos[1]!=NULL){
        if (strcmp(trozos[1], "-term")==0){
            aux = P->lastPos;
            for(i=0; i < aux + 1; i++){
                    if (strstr(P->data[i].estado, "TERMINADO") != NULL) {
                        deleteNodePri(i, P);
                        i = i -1;
                        aux = aux -1;
                    }
            }
        }else if(strcmp(trozos[1], "-sig")==0){
            aux = P->lastPos;
            for(i=0; i < aux + 1; i++){
                    if (strstr(P->data[i].estado, "SEÑAL") != NULL) {
                        deleteNodePri(i, P);
                        i = i - 1;
                        aux = aux -1;
                    } 
            }
        }
        else printf("Uso: deljobs [-term|-sig]\n");
    }
}

void show_environ(char * tr[], int numtrozos, char * envp[]){
    extern char ** environ;
    int i;
    if (numtrozos == 1){
        for(i=0; envp[i] != NULL; i++){
            printf("%p -> main arg3[%d] = (%p) %s\n",(void *)&envp[i], i, envp[i], envp[i]);
        }
    }    
    else if (numtrozos == 2){
        if (strcmp(tr[1], "-environ") == 0){
            for(i=0; environ[i] != NULL; i++){
                printf("%p -> environ[%d] = (%p) %s\n",(void *)&environ[i], i, environ[i], environ[i]);
            }
        }
        else if (strcmp(tr[1], "-addr") == 0){
            printf("environ: %p (almacenado en %p)\n", environ, (void *)&environ);
            printf("tercer arg del main: %p (almacenado en %p)\n", envp, (void *)&envp);
        }
        else printf("Uso: environ [-environ|-addr]");
    }
    else printf("Uso: environ [-environ|-addr]");    
}

void search(dirList *lista, char *tr[]) {
    if (tr[1] == NULL) {
        ShowPaths(lista); 
        return;
    }
    if (strcmp(tr[1], "-add") == 0 && tr[2] != NULL) {
        AddPath(lista, tr[2]);
    } else if (strcmp(tr[1], "-del") == 0 && tr[2] != NULL) {
        RemovePath(lista, tr[2]);
    } else if (strcmp(tr[1], "-clear") == 0) {
        ClearPaths(lista);
    } else if (strcmp(tr[1], "-path") == 0) {
        ImportFromEnvPATH(lista);
    } else {
        printf("search [-add <directorio> | -del <directorio> | -clear | -path]\n");
    }
}


void fg(char **tr, char **envp, dirList *listaBusqueda,int numtrozos){
    
    for(int i=0; i < numtrozos;i++){
        tr[i] = tr[i+1];
    }
    if (tr[0] == NULL) {
        printf("Error: Comando externo no especificado.\n");
        return;
    }

    char ejecutable[MAX_NOMBRE];
    char *directorio = GetFirstPath(listaBusqueda);
    int encontrado = 0;

    while (directorio != NULL) {
        snprintf(ejecutable, MAX_NOMBRE, "%s/%s", directorio, tr[0]);
        if (access(ejecutable, X_OK) == 0) {
            encontrado = 1;
            break;
        }
        directorio = GetNextPath(listaBusqueda);
    }

    if (!encontrado) {
        printf("No ejecutado: no such file or directory");
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Error al crear el proceso hijo");
        return;
    }

    if (pid == 0) {
        if (execve(ejecutable, tr, envp) == -1) {
            perror("Error al intentar ejecutar el programa");
            exit(EXIT_FAILURE); 
        }
    } else {
        int status;
        waitpid(pid, &status, 0); 
        if (WIFEXITED(status)) {
            printf("El proceso hijo terminó con código de salida %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("El proceso hijo terminó debido a la señal %s\n", NombreSenal(WTERMSIG(status)));
        } else if (WIFSTOPPED(status)) {
            printf("El proceso hijo fue detenido por la señal %s\n", NombreSenal(WSTOPSIG(status)));
        } else if (WIFCONTINUED(status)) {
            printf("El proceso hijo continúa en ejecución.\n");
        }
    }
}


void fgpri(char **tr, char **envp, dirList *listaBusqueda, int numtrozos) {
    // Desplazar argumentos
    for (int i = 0; i < numtrozos; i++) {
        tr[i] = tr[i + 1];
    }
    if (atoi(tr[0]) == 0) {
        printf("Error: La prioridad debe ser un numero entre -20 y 19.\n");
        return;
    }

    if ((tr[1]) == NULL){
        printf("Error: Comando externo no especificado.\n");
        return;
    }
    char ejecutable[MAX_NOMBRE];
    char *directorio = GetFirstPath(listaBusqueda);
    int encontrado = 0;

    while (directorio != NULL) {
        snprintf(ejecutable, MAX_NOMBRE, "%s/%s", directorio, tr[1]);
        if (access(ejecutable, X_OK) == 0) {
            encontrado = 1;
            break;
        }
        directorio = GetNextPath(listaBusqueda);
    }

    if (!encontrado) {
        printf("Error: Programa '%s' no encontrado o no ejecutable.\n", tr[1]);
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Error al crear el proceso hijo\n");
        return;
    }

    if (pid == 0) {
        // Proceso hijo
        if (setpriority(PRIO_PROCESS, 0, atoi(tr[0])) == -1) {
            perror("Error al establecer la prioridad\n");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < numtrozos-1; i++) {
        tr[i] = tr[i + 1];
    }
        if (execve(ejecutable, tr, envp) == -1) {
            perror("Error al intentar ejecutar el programa\n");
            exit(EXIT_FAILURE);
        }
    } else {
        // Proceso padre
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("El proceso hijo terminó con código de salida %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("El proceso hijo terminó debido a la señal %s\n", NombreSenal(WTERMSIG(status)));
        } else if (WIFSTOPPED(status)) {
            printf("El proceso hijo fue detenido por la señal %s\n", NombreSenal(WSTOPSIG(status)));
        } else if (WIFCONTINUED(status)) {
            printf("El proceso hijo continúa en ejecución.\n");
        }
    }
}



void programa_externo(char **tr, char **envp, dirList *listaBusqueda) {
    if (tr[0] == NULL) {
        printf("Error: Comando no especificado.\n");
        return;
    }

    int i = 0;
    char **entorno = NULL;
    char *ejecutable_comando = NULL;
    char **trozos = NULL;
    char ejecutable[MAX_NOMBRE];
    char *directorio = NULL;
    int encontrado = 0;

    // Identificar variables de entorno en tr
    while (tr[i] != NULL) {
        int es_variable = 0;
        for (int j = 0; envp[j] != NULL; j++) {
            if (strncmp(tr[i], envp[j], strlen(tr[i])) == 0 && envp[j][strlen(tr[i])] == '=') {
                es_variable = 1;
                break;
            }
        }
        if (!es_variable) break;
        i++;
    }

    // Crear un nuevo entorno con las variables identificadas
    if (i > 0) {
        entorno = malloc((i + 1) * sizeof(char *));
        if (entorno == NULL) {
            perror("Error al asignar memoria");
            return;
        }
        for (int j = 0; j < i; j++) {
            entorno[j] = tr[j];
        }
        entorno[i] = NULL;
    }

    // Obtener ejecutable y argumentos
    ejecutable_comando = tr[i];
    trozos = &tr[i];

    if (ejecutable_comando == NULL) {
        printf("No ejecutado: Bad address\n");
        if (entorno){
             free(entorno);
        }
        return;
    }

    // Buscar el ejecutable en las rutas
    directorio = GetFirstPath(listaBusqueda);
    while (directorio != NULL) {
        snprintf(ejecutable, MAX_NOMBRE, "%s/%s", directorio, ejecutable_comando);
        if (access(ejecutable, X_OK) == 0) {
            encontrado = 1;
            break;
        }
        directorio = GetNextPath(listaBusqueda);
    }

    if (!encontrado) {
        printf("Error: '%s' no encontrado.\n", ejecutable_comando);
        if (entorno){
            free(entorno);
        }
        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Error al crear proceso hijo");
        if (entorno){ 
            free(entorno);
        }
        return;
    }

    if (pid == 0) {
        if (execve(ejecutable, trozos, entorno ? entorno : envp) == -1) {
            perror("Error al ejecutar");
            if (entorno){ 
                free(entorno);
            }
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("El proceso hijo terminó con código de salida %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("El proceso hijo terminó debido a la señal %s\n", NombreSenal(WTERMSIG(status)));
        } else if (WIFSTOPPED(status)) {
            printf("El proceso hijo fue detenido por la señal %s\n", NombreSenal(WSTOPSIG(status)));
        } else if (WIFCONTINUED(status)) {
            printf("El proceso hijo continúa en ejecución.\n");
        }
    }

    if (entorno){
        free(entorno);
    }
}


void showvar(char *tr[], int numtrozos, char *envp[]){
    extern char ** environ;
    char *valor;
    int i, j, pos;
    if (numtrozos < 2){
        for(i=0; envp[i] != NULL; i++){
            printf("%p -> main arg3[%d] = (%p) %s\n",(void *)&envp[i], i, envp[i], envp[i]);
        }
    }
    
    for(i = 1; i < numtrozos; i++){
      pos = BuscarVariable(tr[i], envp);
      if (pos != -1){
          char *entrada_arg3 = envp[pos];
          for(j = 0; entrada_arg3[j] != '\0' && entrada_arg3[j] != '='; j++);
          if (entrada_arg3[j] == '='){
              valor = &entrada_arg3[j+1];
              printf("Con arg3 %s=%s (%p) @%p\n", tr[i], valor, (void *)valor, (void *)&envp[pos]);
          }
          
          else printf("Con arg3 %s no encontrada\n", tr[i]);
      }
      else printf("Con arg3 %s no encontrada\n", tr[i]);
      
      
      pos = BuscarVariable(tr[i], environ);
      if (pos != -1){
          char *entrada_env = environ[pos];
          for(j = 0; entrada_env[j] != '\0' && entrada_env[j] != '='; j++);
          if (entrada_env[j] == '='){
              valor = &entrada_env[j+1];
              printf("Con environ %s=%s (%p) @%p\n", tr[i], valor, (void *)valor, (void *)&environ[pos]);
          }
          
          else printf("Con environ %s no encontrada\n", tr[i]);
      }
      else printf("Con environ %s no encontrada\n", tr[i]);
      
      valor = getenv(tr[i]);
      if (valor != NULL){
        printf("Con getenv %s=%s (%p)\n", tr[i], valor, (void *)valor);
      }
      else printf("Con getenv %s no encontrada\n" ,tr[i]);
    }
}

void changevar(char *tr[], int numtrozos, char *envp[]){
    int pos;
    extern char **environ;
    
    if (numtrozos != 4){
      printf("Changevar [-a|-e|-p] var valor\n");
      return;
    }
    
    if (strcmp(tr[1], "-a") == 0){
        pos = CambiarVariable(tr[2], tr[3], envp);
        if (pos == -1){
            printf("No se pudieron cambiar las variables %s, %s usando el tercer arg del main\n" ,tr[2], tr[3]);
        }
        else printf("Variable %s cambiada a %s usando el arg3 de main\n" ,tr[2], tr[3]);
    }
    
    else if (strcmp(tr[1], "-e") == 0){
        pos = CambiarVariable(tr[2], tr[3], environ);
        if (pos == -1){
            printf("No se pudieron cambiar las variables %s, %s usando environ\n" ,tr[2], tr[3]);
        }
        else printf("Variable %s cambiada a %s usando environ\n" ,tr[2], tr[3]);
    }
    
    else if (strcmp(tr[1], "-p") == 0) {
    // Utilizamos setenv en lugar de putenv
    if (setenv(tr[2], tr[3], 1) != 0) {
        perror("Error: setenv\n");
        return;
    }else{
        printf("Variable %s cambiada a %s usando setenv\n", tr[2], tr[3]);
    }
    }else{
        printf("Changevar [-a|-e|-p] var valor\n");
    }       
}   


void subsvar(char *tr[], char*envp[]){
   extern char **environ;
   int i = 0;

    if(tr[1] == NULL || tr[2] == NULL || tr[3] == NULL || tr[4] == NULL){
    printf("Uso: subsvar [-a|-e] var valor");
    return;
    }      

    if(strcmp(tr[1], "-a") == 0){

        i = BuscarVariable(tr[2],envp);
        if (i == -1) {
        printf("Imposible sustituir %s por %s: %s no existe en %s\n",
               tr[2], tr[3], tr[2], strcmp(tr[1], "-a") == 0 ? "arg3 main" : "environ");
        return;
    }

        snprintf(envp[i], 2 * (strlen(tr[3]) + strlen(tr[4])), "%s=%s", tr[3], tr[4]);

        if(envp[i] == NULL){
            perror("no encontrada");
            return;
        }

    }else if(strcmp(tr[1], "-e") == 0){
        i = BuscarVariable(tr[2],environ);
        snprintf(environ[i], 2 * (strlen(tr[3]) + strlen(tr[4])) ,"%s=%s", tr[3], tr[4]);

        if(environ[i] == NULL){
        perror("no encontrada");
        return;
        }

    }else{
      perror("Argumentos invalidos");
      return;
    }
}




void Cmd_fork (char *tr[], listProc *P)
{
	pid_t pid;
	
	if ((pid=fork())==0){
		clearListaP(P);
		printf ("ejecutando proceso %d\n", getpid());
	}
	else if (pid!=-1)
		waitpid (pid,NULL,0);
}

void Back(char *tr[], listProc *P, dirList *listaBusqueda, char *envp[]) {
    if (tr[0] == NULL) {
        printf("Uso: back prog args...\n");
        return;
    }
    char ejecutable[MAX_NOMBRE];
    char *directorio = GetFirstPath(listaBusqueda);
    int encontrado = 0;
    time_t tiempo;
    pid_t pid;
    
    while (directorio != NULL) {
        snprintf(ejecutable, MAX_NOMBRE, "%s/%s", directorio, tr[1]); 
        if (access(ejecutable, X_OK) == 0) {
            encontrado = 1;
            break;
        }
        directorio = GetNextPath(listaBusqueda);
    }
    if (!encontrado) {
        printf("No ejecutado: No se encontró '%s' o no es ejecutable.\n", tr[1]);
        tiempo = time(NULL);
        proceso nuevoProceso;
        nuevoProceso.pid = -1; // Indica que no se pudo ejecutar
        snprintf(nuevoProceso.command, sizeof(nuevoProceso.command), "%s", tr[1]);
        strftime(nuevoProceso.inicio, sizeof(nuevoProceso.inicio), "%Y-%m-%d %H:%M:%S", localtime(&tiempo));
        snprintf(nuevoProceso.estado, sizeof(nuevoProceso.estado), "TERMINADO (255)");
        nuevoProceso.terminado = -1; // Solo aquí p=-1
        insertItemPri(&nuevoProceso, P);
        return;
    }
    
    pid = fork();
    if (pid < 0) {
        perror("Error al crear el proceso hijo");
        return;
    }
    if (pid == 0) {
        printf("Ejecutando en segundo plano proceso %d: %s\n", getpid(), ejecutable);
        if (execve(ejecutable, &tr[1], envp) == -1) {
            perror("Error al intentar ejecutar el programa");
            exit(255);
        }
    } else {
        tiempo = time(NULL);
        proceso nuevoProceso;
        nuevoProceso.pid = pid;
        snprintf(nuevoProceso.command, sizeof(nuevoProceso.command), "%s", tr[1]);
        strftime(nuevoProceso.inicio, sizeof(nuevoProceso.inicio), "%Y-%m-%d %H:%M:%S", localtime(&tiempo));
        snprintf(nuevoProceso.estado, sizeof(nuevoProceso.estado), "ACTIVO (000)");
        nuevoProceso.terminado = 0;
        insertItemPri(&nuevoProceso, P); // Agrega el proceso a la lista
        printf("Proceso en segundo plano creado con PID %d: %s\n", pid, tr[1]);
        
    }
}

void Backpri(char *tr[], listProc *P, dirList *listaBusqueda, char *envp[]) {
    if (tr[0] == NULL || tr[1] == NULL) {
        printf("Uso: backpri prio prog args...\n");
        return;
    }

    int priod = atoi(tr[1]);
    if (priod < -20 || priod > 19) {
        printf("Imposible establecer prioridad: Valor no válido\n");
        return;
    }

    char ejecutable[MAX_NOMBRE];
    char *directorio = GetFirstPath(listaBusqueda);
    int encontrado = 0;
    time_t tiempo;

    while (directorio != NULL) {
        snprintf(ejecutable, MAX_NOMBRE, "%s/%s", directorio, tr[2]);
        if (access(ejecutable, X_OK) == 0) {
            encontrado = 1;
            break;
        }
        directorio = GetNextPath(listaBusqueda);
    }

    if (!encontrado) {
        printf("Error: Programa '%s' no encontrado o no ejecutable.\n", tr[2]);
        tiempo = time(NULL);
        proceso nuevoProceso;
        nuevoProceso.pid = -1; // Indica que no se pudo ejecutar
        snprintf(nuevoProceso.command, sizeof(nuevoProceso.command), "%s", tr[1]);
        strftime(nuevoProceso.inicio, sizeof(nuevoProceso.inicio), "%Y-%m-%d %H:%M:%S", localtime(&tiempo));
        snprintf(nuevoProceso.estado, sizeof(nuevoProceso.estado), "TERMINADO (255)");
        nuevoProceso.terminado = -1; // Solo aquí p=-1
        insertItemPri(&nuevoProceso, P);
        return;

        return;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Error proceso hijo");
        return;
    }

    if (pid == 0) { 
        if (setpriority(PRIO_PROCESS, 0, priod) == -1) {
            perror("Error con la prioridad");
            exit(EXIT_FAILURE);
        }

        if (execve(ejecutable, &tr[2], envp) == -1) {
            perror("Error al ejecutar");
            exit(EXIT_FAILURE);
        }
    } else { 
        proceso nuevoProceso;
        nuevoProceso.pid = pid;
        snprintf(nuevoProceso.command, sizeof(nuevoProceso.command), "%s", tr[2]);
        tiempo = time(NULL);
        strftime(nuevoProceso.inicio, sizeof(nuevoProceso.inicio), "%Y-%m-%d %H:%M:%S", localtime(&tiempo));
        snprintf(nuevoProceso.estado, sizeof(nuevoProceso.estado), "ACTIVO (000)"); 
        nuevoProceso.terminado = 0; 
        insertItemPri(&nuevoProceso, P);
        printf("Proceso con %d de pid y prioridad de %d añadido en segundo plano : %s\n", pid, priod, tr[2]);
    }
}

char * Ejecutable (char *s, dirList *D)
{
    static char path[MAXNAME];
    struct stat st;
    char *p;

    if (s==NULL || (p=GetFirstPath(D))==NULL)
        return s;
    if (s[0]=='/' || !strncmp (s,"./",2) || !strncmp (s,"../",3))
        return s;        /*is an absolute pathname*/

    strncpy (path, p, MAXNAME-1);strncat (path,"/",MAXNAME-1); strncat(path,s,MAXNAME-1);
    if (lstat(path,&st)!=-1)
        return path;
    while ((p=GetNextPath(D))!=NULL){
        strncpy (path, p, MAXNAME-1);strncat (path,"/",MAXNAME-1); strncat(path,s,MAXNAME-1);
        if (lstat(path,&st)!=-1)
            return path;
    }
    return s;
}

int Execpve(char *tr[], char **NewEnv, int * pprio, dirList *D)
{
    char *p;               /*NewEnv contains the address of the new environment*/
    /*pprio the address of the new priority*/
    /*NULL indicates no change in environment and/or priority*/
    if (tr[0]==NULL || (p=Ejecutable(tr[0], D))==NULL){
        errno=EFAULT;
        return-1;
    }
    if (pprio !=NULL  && setpriority(PRIO_PROCESS,getpid(),*pprio)==-1 && errno){
        printf ("Imposible cambiar prioridad: %s\n",strerror(errno));
        return -1;
    }

    if (NewEnv==NULL)
        return execv(p,tr);
    else
        return execve (p, tr, NewEnv);
}

void execpri(char **tr, dirList *D) {
    int ppri;
    char **suma_trozos;
    if (tr[1] == NULL || tr[2] == NULL) {
        perror("execpri: prio prog args\n");
        return;
    }
    ppri = atoi(tr[1]);
    if (ppri < -20 || ppri > 19) { 
        perror("Prioridad fuera de rango\n");
        return;
    }

    suma_trozos = &tr[2];

    if (Execpve(suma_trozos, NULL, &ppri, D) == -1) {
        perror("Error al ejecutar el programa\n");
        return;
    
    }
}

void exec(char *tr[], dirList *D) {
    char *path;
    char **suma_trozos;
    if (tr[1] == NULL) {
        printf("Error: No se ha proporcionado un programa para ejecutar.\n");
        return;
    }

    path = Ejecutable(tr[1], D);
    if (path == NULL) {
        printf("Error: No se encontró el ejecutable '%s'.\n", tr[1]);
        return;
    }

    suma_trozos = &tr[1]; // Argumentos empiezan desde tr[1]

    if (Execpve(suma_trozos, NULL, NULL, D) == -1) {
        perror("Error al ejecutar el programa");
        return;
    }
}


int main(int argc, char * argv[], char * envp[]){
    int i;
    bool terminado = false;
    char line[1024];
    char *trozos[64];
    tList L;
    fileList F;
    mList M;
    listProc P;
    dirList D;

    createEmptyList(&L);
    createFileList(&F);
    createEmptyListMem(&M);
    createEmptyListProc(&P);
    InitPathList(&D);


    while (!terminado){
        //imprimirPrompt();
        printf("-> ");

        //leerEntrada();
        fgets(line, BUFFER, stdin);
        saveOnList(line, &L);

        i = TrocearCadena(line, trozos);

        if(i != 0)
            procesarEntrada(trozos, &terminado, &L,&F,&M,&P,&D,i, envp);
        
    }
    delete_H(&L);
    clearListaM(&M);
    clearListaP(&P);
    ClearPaths(&D);
}
