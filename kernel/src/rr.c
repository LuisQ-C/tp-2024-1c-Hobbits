#include "../include/rr.h"

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
extern int fd_interrupt;

extern int quantum;

void planificacion_rr(){
    while (1)
    {
        sem_wait(&proceso_en_cola_ready);
        sem_wait(&pasar_a_ejecucion);
        sem_wait(&planificacion_ready_iniciada);
        //pthread_t hilo_int_quantum;
        //pthread_create(&hilo_int_quantum, NULL, (void*)interrupcion_quantum, NULL);
        //pthread_detach(hilo_int_quantum);
        // POR AHORA NO LA USES! pasar_a_cola_exec();
        t_pcb* pcb_auxiliar = squeue_pop(lista_procesos_ready);
        pcb_auxiliar->estado = EXEC;
        squeue_push(lista_procesos_exec, pcb_auxiliar);
        log_info(logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXEC", pcb_auxiliar->pid);
        
        //t_pcb* pcb_auxiliar1 = squeue_peek(lista_procesos_exec);
//        log_info(logger, "%d", fd_dispatch);
        enviar_pcb(pcb_auxiliar, fd_dispatch);
        //interrupcion_quantum();
        //usleep(quantum * 1000);

        //int pid_a_enviar = 0;
        //pid_a_enviar = pcb_auxiliar->pid;
        interrupcion_quantum(pcb_auxiliar);
        //send(fd_interrupt, &pid_a_enviar, sizeof(int),0);

        //log_debug(logger, "deberias haber ido a fin de q");
        //recibir_contexto_actualizado(fd_dispatch);
        recibir_contexto_actualizado(fd_dispatch);
        //log_warning(logger, "pude actualizarme");
        //recibir_operacion(fd_dispatch);
        //t_list* pcb_con_motivo = recibir_paquete(fd_dispatch);
        sem_post(&pasar_a_ejecucion);
        sem_post(&planificacion_ready_iniciada);

    }
    
}

void interrupcion_quantum(t_pcb* pcb_auxiliar){
    usleep(quantum * 1000);
    int pid_a_enviar = 0;
    pid_a_enviar = pcb_auxiliar->pid;
    send(fd_interrupt, &pid_a_enviar, sizeof(int), 0);
}