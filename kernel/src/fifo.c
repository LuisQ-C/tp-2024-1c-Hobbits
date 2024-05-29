#include "../include/fifo.h"

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

extern int fd_dispatch;


void planificacion_fifo(){
    while (1)
    {
        sem_wait(&proceso_en_cola_ready);
        sem_wait(&pasar_a_ejecucion);
        sem_wait(&planificacion_ready_iniciada);
        pasar_a_cola_exec();
    }
    
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
    
        //log_info(logger, "me ejecute %d", pcb_auxiliar->pid);
        //supongamos que terminó
        sem_post(&pasar_a_ejecucion);
        //sem_post(&grado_de_multiprogramacion); LO MANDAMOS AL CASE EXIT, PORQUE ES EN EL UNICO CASO DONDE AUMENTA EL GRADO MULTRIPROGR.
    }
}