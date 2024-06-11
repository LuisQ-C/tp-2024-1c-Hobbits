#include "../include/vrr.h"

extern t_config* config;
extern t_log* logger;

extern t_squeue *lista_procesos_new;
extern t_squeue *lista_procesos_ready;

extern t_squeue *lista_procesos_ready_plus;

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
extern char* algoritmo;


void planificacion_vrr(){
    while (1)
    {
        sem_wait(&proceso_en_cola_ready);
        sem_wait(&planificacion_ready_iniciada);
        t_pcb* pcb_auxiliar;

        if(!squeue_is_empty(lista_procesos_ready_plus)){
            pcb_auxiliar = squeue_pop(lista_procesos_ready_plus);    
        }
        else{
            pcb_auxiliar = squeue_pop(lista_procesos_ready);
        }

        pthread_t hilo_q;
        data* new_data = malloc(sizeof(data));
        new_data->quantum = pcb_auxiliar->quantum;
        new_data->pid = pcb_auxiliar->pid;
        pthread_create(&hilo_q, NULL, (void *)hilo_quantum, (void*) new_data);
        pthread_detach(hilo_q);
        recibir_contexto_actualizado(fd_dispatch);
        pthread_cancel(hilo_q);
        sem_post(&planificacion_ready_iniciada);


    }
    
}