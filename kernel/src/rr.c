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
        
        t_pcb* pcb_auxiliar = squeue_pop(lista_procesos_ready);
        pcb_auxiliar->estado = EXEC;
        squeue_push(lista_procesos_exec, pcb_auxiliar);
        log_info(logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXEC", pcb_auxiliar->pid);
        
        
        enviar_pcb(pcb_auxiliar, fd_dispatch);
        

        
        //interrupcion_quantum(pcb_auxiliar);
        
        pthread_t hilo_q;
        data* new_data = malloc(sizeof(data));
        new_data->quantum = quantum;
        new_data->pid = pcb_auxiliar->pid;
        pthread_create(&hilo_q,NULL,(void*)hilo_quantum,(void*) new_data);
        
        recibir_contexto_actualizado(fd_dispatch);
        pthread_cancel(hilo_q); //PODRIA IR DENTRO DE RECIBIR CONTEXTO ACTUALIZADO LUEGO DE RECIBIR OPERACION PARA QUE LO CANCELE LO ANTES POSIBLE
        
        sem_post(&pasar_a_ejecucion);
        sem_post(&planificacion_ready_iniciada);

    }
    
}
void hilo_quantum(void* arg)
{
    data* quantum_recibido = arg;
    int quantum_r = quantum_recibido->quantum;
    int pid_r = quantum_recibido->pid;
    free(quantum_recibido);
    usleep(quantum_r*1000);
    send(fd_interrupt,&pid_r,sizeof(int),0);
}

//HILO QUE ESPERE Y MANDE LA INTERRUPCION, EL OTRO SE QUEDARA BLOQUEANTE
void interrupcion_quantum(t_pcb* pcb_auxiliar){
    usleep(quantum * 1000);
    int pid_a_enviar = 0;
    pid_a_enviar = pcb_auxiliar->pid;
    send(fd_interrupt, &pid_a_enviar, sizeof(int), 0);
}