#include "../include/consola.h"

extern t_log* logger;
extern t_config* config;

extern t_squeue *lista_procesos_new;
extern t_squeue *lista_procesos_ready;
extern t_squeue *lista_procesos_exec;
extern t_squeue *lista_procesos_exit;
extern t_slist *lista_procesos_blocked;

extern sem_t grado_de_multiprogramacion;
extern sem_t proceso_en_cola_new;
extern sem_t proceso_en_cola_ready;
extern sem_t pasar_a_ejecucion;

extern sem_t planificacion_new_iniciada;
extern sem_t planificacion_ready_iniciada;
extern sem_t planificacion_exec_iniciada;
extern sem_t planificacion_blocked_iniciada;
extern sem_t hay_una_peticion_de_proceso;

extern bool planificacion_iniciada;

t_squeue* squeue_path;

char* opciones[] = {
    "EJECUTAR_SCRIPT",
    "INICIAR_PROCESO pseudos/",
    "FINALIZAR_PROCESO",
    "DETENER_PLANIFICACION",
    "INICIAR_PLANIFICACION",
    "MULTIPROGRAMACION",
    "PROCESO_ESTADO",
    NULL
};
//Por parte de la documentación compartida en UTNSO
char** custom_completion(const char* text, int start, int end){
    char** matches = NULL;

    if (start == 0) {
        matches = rl_completion_matches(text, &custom_completion_generator);
    } else {
        rl_attempted_completion_over = 1;
    }

    return matches;
}

char* custom_completion_generator(const char* text, int state){
    static int list_index, len;
    const char* name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = opciones[list_index])) {
        list_index++;

        if (strncasecmp(name, text, len) == 0) {
            return strdup(name);
        }
    }
    return NULL;
}

pthread_mutex_t hilo_pid_mutex;
int pid_contador = 0;

extern int multiprog;
extern char* algoritmo;

bool interrupcion_usuario = false;

int fd_dispatch;
int fd_interrupt;
int fd_mem;
int fd_IO;
//asdasd
void iniciar_consola(void* fd_info){
    squeue_path = squeue_create();
    pthread_t hilo_crear_proceso;
    pthread_create(&hilo_crear_proceso, NULL, (void*) crear_proceso, NULL);
    pthread_detach(hilo_crear_proceso);
    //t_log* logger;
    //logger = iniciar_logger("kernel.log","Kernel",1,LOG_LEVEL_INFO);
    info_fd* auxiliar = fd_info;
    //les asigno un valor a las variables globales
    fd_dispatch = auxiliar->fd_cpu_dispatch;
    fd_interrupt = auxiliar->fd_cpu_interrupt;
    fd_mem = auxiliar->fd_memoria;
    fd_IO = auxiliar->fd_escucha_interfaces;

    rl_attempted_completion_function = custom_completion;

    char* leido;
    printf("EJECUTAR_SCRIPT [PATH] \n");
    printf("INICIAR_PROCESO [PATH] \n");
    printf("FINALIZAR_PROCESO [PID] \n");
    printf("DETENER_PLANIFICACION \n");
    printf("INICIAR_PLANIFICACION \n");
    printf("MULTIPROGRAMACION [VALOR]\n");
    printf("PROCESO_ESTADO \n");

    leido = readline("> ");
    bool validarComando;
    while(!string_is_empty(leido)){
        validarComando = validar_instrucciones_leidas(leido);
        if(!validarComando){
            log_error(logger, "Comando no reconocido");
            free(leido);
            leido = readline("> ");
            continue;
        }
        add_history(leido);
        instrucciones_consola(leido);
        free(leido);
        leido = readline("> ");
    }
    free(leido);
}

bool validar_instrucciones_leidas(char* leido){
    char** instruccion_leida = string_split(leido, " ");
    bool valido;
    //log_info(logger, "%s y %s y %s", instruccion_leida[0], instruccion_leida[1], instruccion_leida[2]);

    if(strcmp(instruccion_leida[0], "EJECUTAR_SCRIPT") == 0 && instruccion_leida[1] != NULL && strcmp(instruccion_leida[1], "") != 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "INICIAR_PROCESO") == 0 && instruccion_leida[1] != NULL && strcmp(instruccion_leida[1], "") != 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "FINALIZAR_PROCESO") == 0 && instruccion_leida[1] != NULL && strcmp(instruccion_leida[1], "") != 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "DETENER_PLANIFICACION") == 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "INICIAR_PLANIFICACION") == 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "MULTIPROGRAMACION") == 0 && instruccion_leida[1] != NULL && strcmp(instruccion_leida[1], "") != 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "PROCESO_ESTADO") == 0)
        valido = true;
    else 
        valido = false;

    string_array_destroy(instruccion_leida);

    return valido;
}

void instrucciones_consola(char* leido){
    char** instruccion_leida = string_split(leido, " ");

    if(strcmp(instruccion_leida[0], "EJECUTAR_SCRIPT") == 0)
        ejecutar_script(instruccion_leida[1]);
    else if (strcmp(instruccion_leida[0], "INICIAR_PROCESO") == 0){
        //iniciar_proceso(instruccion_leida[1]);
        char* aux = string_duplicate(instruccion_leida[1]);
        squeue_push(squeue_path, aux);
        sem_post(&hay_una_peticion_de_proceso);
    }
    else if (strcmp(instruccion_leida[0], "FINALIZAR_PROCESO") == 0)
        finalizar_proceso(atoi(instruccion_leida[1]));
    else if (strcmp(instruccion_leida[0], "DETENER_PLANIFICACION") == 0)
        detener_planificacion();
    else if (strcmp(instruccion_leida[0], "INICIAR_PLANIFICACION") == 0)
        iniciar_planificacion();
    else if (strcmp(instruccion_leida[0], "MULTIPROGRAMACION") == 0)
        multiprogramacion(atoi(instruccion_leida[1]));
    else if (strcmp(instruccion_leida[0], "PROCESO_ESTADO") == 0)
        proceso_estado();

    string_array_destroy(instruccion_leida);
}


void ejecutar_script(char* path){
    FILE* arch_instrucciones;
    char* linea_almacenada = NULL;
    size_t tam_almacenamiento = 0;
    int linea_leida;
    
    arch_instrucciones = fopen(path, "rt");
    if(arch_instrucciones == NULL)
    {
        log_error(logger, "No se pudo abrir el archivo de instrucciones");
        return;
    }
    while((linea_leida = getline(&linea_almacenada, &tam_almacenamiento, arch_instrucciones)) != -1){
        strtok(linea_almacenada, "\n");
        if(!string_is_empty(linea_almacenada)){
            if(!validar_instrucciones_leidas(linea_almacenada)){
                log_error(logger, "Instruccion no reconocida");
                continue;
            }
            instrucciones_consola(linea_almacenada);
            
        }
    }
    free(linea_almacenada);
    fclose(arch_instrucciones);
}

void iniciar_proceso(char* path){
    //printf("iniciar_proceso \n");
    //log_debug(logger,"PATH A MANDAR: %s",path);
    t_pcb *nuevo_pcb = crear_pcb();
    enviar_nuevo_proceso(&nuevo_pcb->pid, path, fd_mem);
    
    //Le envio las instrucciones a memoria y espero respuesta
    int respuesta;
    recv(fd_mem, &respuesta,sizeof(int), MSG_WAITALL);
    if(respuesta == ARCHIVO_INVALIDO){
        pthread_mutex_lock(&hilo_pid_mutex);
        pid_contador--;
        pthread_mutex_unlock(&hilo_pid_mutex);
        log_warning(logger, "PARA UN POCO, REVISA LA RUTA DEL ARCHIVO");
        free(nuevo_pcb);
        return;
    }

    squeue_push(lista_procesos_new, nuevo_pcb);
    log_info(logger, "Se crea el proceso %d en NEW", nuevo_pcb->pid);
    
    sem_post(&proceso_en_cola_new);

}

void crear_proceso(){

    while(1){
        sem_wait(&hay_una_peticion_de_proceso);
        char* path_aux = squeue_pop(squeue_path);
        iniciar_proceso(path_aux);
        free(path_aux);
    }

}


void finalizar_proceso(int pid){
    detener_planificacion();
    //printf("finalizar_proceso \n");
    
    t_pcb* pcb_auxiliar;

    bool _elemento_encontrado(t_pcb* pcb){
        bool coincide = pcb->pid == pid;
        return coincide;
    }
    if(squeue_any_satisfy(lista_procesos_new, (void*) _elemento_encontrado)){
        pcb_auxiliar = squeue_remove_by_condition(lista_procesos_new, (void*) _elemento_encontrado);
        manejar_fin_con_motivo(INTERRUPTED_BY_USER_NEW, pcb_auxiliar);
    }
    else if(squeue_any_satisfy(lista_procesos_ready, (void*) _elemento_encontrado)){
        pcb_auxiliar = squeue_remove_by_condition(lista_procesos_ready, (void*) _elemento_encontrado);
        manejar_fin_con_motivo(INTERRUPTED_BY_USER_READY, pcb_auxiliar);
    }
    else if(squeue_any_satisfy(lista_procesos_exec, (void*) _elemento_encontrado)){
        pcb_auxiliar = squeue_peek(lista_procesos_exec);
        int pid_auxiliar = pcb_auxiliar->pid;
        if(strcmp(algoritmo, "RR") == 0 || strcmp(algoritmo, "VRR") == 0)
            interrupcion_usuario = true;
        
        enviar_interrupcion(USER_INTERRUPT, pid_auxiliar, fd_interrupt);    

    }
    else if(squeue_any_satisfy(lista_procesos_exit, (void*) _elemento_encontrado)){
        log_error(logger, "QUE HACES, SI YA ESTA EN EXIT");
    }

    iniciar_planificacion();

}



//sem_post(&proceso_en_cola_new);
        //sem_post(&proceso_en_cola_ready);
        //sem_post(&pasar_a_ejecucion);
        
void detener_planificacion(){
    //printf("detener_planificador \n");
    if(planificacion_iniciada)
    {
        planificacion_iniciada = false;
        /*
        pthread_t detener_new, detener_ready, detener_exec, detener_blocked;
        pthread_create(&detener_new,NULL,(void*) detener_cola_new,NULL);
        pthread_create(&detener_ready,NULL,(void*) detener_cola_ready,NULL);
        pthread_create(&detener_exec,NULL,(void*) detener_cola_exec,NULL);
        pthread_create(&detener_blocked,NULL,(void*) detener_cola_blocked,NULL);
        pthread_detach(detener_new);
        pthread_detach(detener_ready);
        pthread_detach(detener_exec);
        pthread_detach(detener_blocked);
        */
        
        log_info(logger, "Se detuvo la planificacion");
    }else
    {
        log_info(logger,"la plani esta pausada ya");
    }
    
}
void detener_cola_new(void* arg)
{
    sem_wait(&planificacion_new_iniciada);
}
void detener_cola_ready(void* arg)
{
    sem_wait(&planificacion_ready_iniciada);
}
void detener_cola_exec(void* arg)
{
    sem_wait(&planificacion_exec_iniciada);
}
void detener_cola_blocked(void* arg)
{
    sem_wait(&planificacion_blocked_iniciada);
}

void iniciar_planificacion(){
    if(!planificacion_iniciada){
    //printf("iniciar_planificacion \n");
    planificacion_iniciada = true;
    
    sem_post(&planificacion_new_iniciada);
    sem_post(&planificacion_ready_iniciada);
    sem_post(&planificacion_exec_iniciada);
    sem_post(&planificacion_blocked_iniciada);
    log_info(logger, "Planificación %s iniciada", config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
    }
}

void multiprogramacion(int valor){
    //printf("multiprogramacion \n");
    cambiar_grado_de_multiprogramacion(valor);
}

void proceso_estado(){
    //printf("proceso_estado \n");
    if(!squeue_is_empty(lista_procesos_new))
    {
        char* pids_listar = listar_pids(lista_procesos_new);
        log_info(logger, "Procesos cola new: %s", pids_listar);
        free(pids_listar);
    }
    else
        log_info(logger, "La cola new esta vacia");


    if(!squeue_is_empty(lista_procesos_ready))
    {
        char* pids_listar = listar_pids(lista_procesos_ready);
        log_info(logger, "Procesos cola ready: %s", pids_listar);
        free(pids_listar);
    }
    else 
        log_info(logger, "La cola ready esta vacia");

    if(!squeue_is_empty(lista_procesos_exec))
    {
        char* pids_listar = listar_pids(lista_procesos_exec);
        log_info(logger, "Procesos cola exec: %s", pids_listar);
        free(pids_listar);
    }
    else
        log_info(logger, "La cola exec esta vacia");

    if(!squeue_is_empty(lista_procesos_exit))
    {
        char* pids_listar = listar_pids(lista_procesos_exit);
        log_info(logger, "Procesos cola exit: %s", pids_listar);
        free(pids_listar);
    }
    else
        log_info(logger, "La cola exit esta vacia");
    if(!lista_interfaces_is_empty()){

        char* pids = string_new();
        void listar_pids_blocked(t_list_io* interfaz){
            if(!cola_io_is_empty(interfaz)){
            char* auxiliar_pids = pids_blocked(interfaz);
            string_append_with_format(&pids, "%s, ", auxiliar_pids);    
            }
        }

        pthread_mutex_lock(lista_procesos_blocked->mutex);
        list_iterate(lista_procesos_blocked->lista, (void*) listar_pids_blocked);
        pthread_mutex_unlock(lista_procesos_blocked->mutex);

        if(!string_is_empty(pids)){
            log_info(logger, "Procesos cola blocked: %s", pids);
            free(pids);
        }
        else
            log_info(logger, "La cola blocked esta vacia");
    }
    else
        log_info(logger, "No hay interfaz conectada");
    //FALTA IMPRIMIR BLOCKED, TODAS SUS COLAS
}

char* pids_blocked(t_list_io* interfaz){
    char* pids = string_new();

    void obtener_pids_blocked(t_elemento_iogenerica* elemento_iogenerico){
        char* pid = string_itoa(elemento_iogenerico->pcb->pid);
        if(!(strcmp(pid, " ") == 0)){
            string_append_with_format(&pids, "%s", pid);
            log_warning(logger, "%s", pid);
        }
        free(pid);
    }

    cola_io_iterate(interfaz, (void*) obtener_pids_blocked);
    return pids;
}

////////////

void cambiar_grado_de_multiprogramacion(int nuevo_grado_mp){
    if(nuevo_grado_mp > multiprog){
        int diferencia_entre_grados = nuevo_grado_mp - multiprog;
        for (int i = 0; i < diferencia_entre_grados; i++)
        {
            sem_post(&grado_de_multiprogramacion);
        }
    }
    else if (nuevo_grado_mp < multiprog){
        int diferencia_entre_grados = multiprog - nuevo_grado_mp;
        for (int i = 0; i < diferencia_entre_grados; i++)
        {
            //creo el hilo para que no se congele la consola
            pthread_t hilo_cambio_mp;
            pthread_create(&hilo_cambio_mp, NULL, (void *) bajar_grado, NULL);
            pthread_detach(hilo_cambio_mp);
        }
    }
    log_warning(logger, "Se cambio el grado de multiprogramacion: Anterior %d - Nuevo: %d", multiprog, nuevo_grado_mp);
    multiprog = nuevo_grado_mp;
}

void bajar_grado(){
    sem_wait(&grado_de_multiprogramacion);
}

//////Funciones procesos
t_pcb* crear_pcb(){
    t_pcb* pcb_creado = malloc(sizeof(t_pcb));

    pcb_creado->pid = asignar_pid();
    pcb_creado->pc = 0;
    pcb_creado->quantum = config_get_int_value(config, "QUANTUM");
    pcb_creado->estado = NEW;
    pcb_creado->registros_CPU = iniciar_registros_vacios();
    return pcb_creado;
}

t_registros_generales iniciar_registros_vacios(){
    t_registros_generales registro_auxiliar;

    registro_auxiliar.AX = 0;
    registro_auxiliar.BX = 0;
    registro_auxiliar.CX = 0;
    registro_auxiliar.DX = 0;
    registro_auxiliar.EAX = 0;
    registro_auxiliar.EBX = 0;
    registro_auxiliar.ECX = 0;
    registro_auxiliar.EDX = 0;

    return registro_auxiliar;
}

int asignar_pid(){
    int pid_a_asignar;
    pthread_mutex_lock(&hilo_pid_mutex);
    pid_a_asignar = pid_contador++;
    pthread_mutex_unlock(&hilo_pid_mutex);
    return pid_a_asignar;
}