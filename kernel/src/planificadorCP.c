#include "../include/planificadorCP.h"

extern t_config* config;
extern t_log* logger;

extern t_squeue *lista_procesos_new;
extern t_squeue *lista_procesos_ready;
extern t_squeue *lista_procesos_exec;
extern t_squeue *lista_procesos_exit;

extern sem_t grado_de_multiprogramacion;
extern sem_t proceso_en_cola_new;
extern sem_t proceso_en_cola_ready;
extern sem_t ejecutar_proceso;
extern sem_t pasar_a_ejecucion;

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

    log_info(logger, "Planificacion %s iniciada", algoritmo);

}

void planificacion_fifo(){
    while (1)
    {
        sem_wait(&proceso_en_cola_ready);
        sem_wait(&pasar_a_ejecucion);
        if(!planificacion_iniciada){
            break;
        }
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
}

void ejecutar_procesos_exec(){
    while (1)
    {
        sem_wait(&ejecutar_proceso);
        t_pcb* pcb_auxiliar = squeue_peek(lista_procesos_exec);
//        log_info(logger, "%d", fd_dispatch);
        enviar_pcb(pcb_auxiliar, fd_dispatch);
        int ok;
        recv(fd_dispatch, &ok,sizeof(int), MSG_WAITALL);
        t_pcb* pcb_aux = squeue_pop(lista_procesos_exec);
        squeue_push(lista_procesos_exit,pcb_aux);

        log_info(logger, "me ejecute %d", pcb_auxiliar->pid);
        //supongamos que terminó
        sem_post(&pasar_a_ejecucion);
        sem_post(&grado_de_multiprogramacion);
    }
    
}