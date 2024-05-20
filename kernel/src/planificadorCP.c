#include "../include/planificadorCP.h"

extern t_config* config;
extern t_log* logger;

extern t_squeue *lista_procesos_new;
extern t_squeue *lista_procesos_ready;
extern t_squeue *lista_procesos_exec;
extern t_squeue *lista_procesos_exit;
extern t_slist *lista_procesos_blocked;

extern sem_t grado_de_multiprogramacion;
extern sem_t proceso_en_cola_new;
extern sem_t proceso_en_cola_ready;
extern sem_t ejecutar_proceso;
extern sem_t pasar_a_ejecucion;

extern sem_t planificacion_new_iniciada;
extern sem_t planificacion_ready_iniciada;
extern sem_t planificacion_exec_iniciada;

extern bool planificacion_iniciada;

extern int fd_dispatch;


void iniciar_PCP(){
    char* algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");

    pthread_t hilo_CP;

    if(strcmp(algoritmo, "FIFO")==0)
        pthread_create(&hilo_CP, NULL, (void*) planificacion_fifo, NULL);
    if(strcmp(algoritmo, "RR")==0)
        pthread_create(&hilo_CP, NULL, (void*) planificacion_rr, NULL);
    
    pthread_detach(hilo_CP);

    pthread_t hilo_ejecucion;
    pthread_create(&hilo_ejecucion, NULL, (void *) ejecutar_procesos_exec, NULL);
    pthread_detach(hilo_ejecucion);

    //CREAR OTRO HILO PARA MANDAR PROCESOS A BLOCKED

    log_info(logger, "Planificacion %s iniciada", algoritmo);

}

void planificacion_fifo(){
    while (1)
    {
        sem_wait(&proceso_en_cola_ready);
        sem_wait(&pasar_a_ejecucion);
        sem_wait(&planificacion_ready_iniciada);
        pasar_a_cola_exec();
    }
    
}

void planificacion_rr(){

}


void pasar_a_cola_exec(){

    t_pcb* pcb_auxiliar = squeue_pop(lista_procesos_ready);
    pcb_auxiliar->estado = EXEC;
    squeue_push(lista_procesos_exec, pcb_auxiliar);
    log_info(logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXEC", pcb_auxiliar->pid);
    sem_post(&ejecutar_proceso);
    sem_post(&planificacion_ready_iniciada);
}


//HILO ENCARGADO DE UNICAMENTE ENVIAR A EXEC, RECIBIR CONTEXTO ACTUALIZADO Y ENVIAR A BLOCKED, EXIT DEPENDIENDO DEL CASO
void ejecutar_procesos_exec(){
    while (1)
    {
        sem_wait(&ejecutar_proceso);
        t_pcb* pcb_auxiliar = squeue_peek(lista_procesos_exec);
//        log_info(logger, "%d", fd_dispatch);
        enviar_pcb(pcb_auxiliar, fd_dispatch);

        
        recibir_contexto_actualizado(fd_dispatch);
        

        log_info(logger, "me ejecute %d", pcb_auxiliar->pid);
        //supongamos que terminó
        sem_post(&pasar_a_ejecucion);
        //sem_post(&grado_de_multiprogramacion); LO MANDAMOS AL CASE EXIT, PORQUE ES EN EL UNICO CASO DONDE AUMENTA EL GRADO MULTRIPROGR.
    }
}

//HILO ENCARGADO DE MANEJAR LOS PROCESOS DE BLOCKED, CON QUE CRITERIO? NI IDEA
//SEMAFORO QUE TIENE CADA IO TE PUEDE AVISAR CUANDO LE PODES MANDAR UNA SOLICITUD DE VUELTA

void recibir_contexto_actualizado(int fd_dispatch)
{
    recibir_operacion(fd_dispatch);
    t_list* pcb_con_motivo = recibir_paquete(fd_dispatch);
    sem_wait(&planificacion_exec_iniciada);
    t_pcb* pcb_a_actualizar = squeue_pop(lista_procesos_exec);
    actualizar_pcb_ejecutado(pcb_a_actualizar,pcb_con_motivo);
    manejar_motivo_interrupcion(pcb_a_actualizar,pcb_con_motivo);
    sem_post(&planificacion_exec_iniciada);
    list_destroy_and_destroy_elements(pcb_con_motivo,(void*) liberar_elemento);

}
void actualizar_pcb_ejecutado(t_pcb* pcb_a_actualizar,t_list* pcb_con_motivo)
{
    uint32_t* pc = list_get(pcb_con_motivo,0);
    int* pid = list_get(pcb_con_motivo,1);
    int* quantum = list_get(pcb_con_motivo,2);
    uint32_t* estado = list_get(pcb_con_motivo,3);
    t_registros_generales* registros_generales = list_get(pcb_con_motivo,4);
    pcb_a_actualizar->pid = *pid;
    pcb_a_actualizar->pc = *pc;
    pcb_a_actualizar->quantum = *quantum;
    pcb_a_actualizar->estado = *estado;
    pcb_a_actualizar->registros_CPU = *registros_generales;
}
void manejar_motivo_interrupcion(t_pcb* pcb_a_actualizar,t_list* pcb_con_motivo)
{
    int* motivo_interrupcion = list_get(pcb_con_motivo,5);
    switch(*motivo_interrupcion)
    {
        case EXIT:
            pcb_a_actualizar->estado = EXIT;
            squeue_push(lista_procesos_exit,pcb_a_actualizar);

            log_info(logger, "PID: %d - Estado Anterior: EXECUTE - Estado Actual: EXIT", pcb_a_actualizar->pid);
            
            sem_post(&grado_de_multiprogramacion);
            break;
        case IO_GEN_SLEEP:
            char* nombre_interfaz = list_get(pcb_con_motivo,6);
            int* tiempo_dormicion = list_get(pcb_con_motivo,7);
            bool validez = slist_comprobate_io(nombre_interfaz,IO_GEN_SLEEP);
            if(validez)
            {
                t_list_io* interfaz_buscada = slist_buscar_interfaz(nombre_interfaz);
                pcb_a_actualizar->estado = BLOCKED;
                t_elemento_iogenerica* nueva_solicitud_gen = malloc(sizeof(t_elemento_iogenerica));
                nueva_solicitud_gen->pcb= pcb_a_actualizar;
                nueva_solicitud_gen->tiempo = *tiempo_dormicion;
                push_elemento_cola_io(interfaz_buscada,nueva_solicitud_gen);

                log_info(logger, "PID: %d - Estado Anterior: EXECUTE - Estado Actual: BLOCKED", pcb_a_actualizar->pid);

                sem_post(interfaz_buscada->hay_proceso_cola);
            }
            else
            {
                pcb_a_actualizar->estado = EXIT;
                squeue_push(lista_procesos_exit,pcb_a_actualizar);

                log_info(logger, "PID: %d - Estado Anterior: EXECUTE - Estado Actual: EXIT", pcb_a_actualizar->pid);

                sem_post(&grado_de_multiprogramacion);
            }
            break;
        case INTERRUPCION:
            break;
        default:
            log_warning(logger,"MOTIVO DE DESALOJO DESCONOCIDO");
            break;
    }
}


/*
hilo que manda a ejecutar a las interfaces

le manda la operacion, recibe el ok de la interfaz, saca el proceso de blocked y lo manda a ready

//con criterio va ir vaciando

//el hilo que cada interfaz

//semaforo de cada interfaz
//cuando yo le mando algo a la interfaz lo seteo en 0

//cuando me da el ok hago signal, entonces el hilo va a hacer pop del proceso y lo manda a ready
//y procede a sacar el 2do proceso de la cola

*/
