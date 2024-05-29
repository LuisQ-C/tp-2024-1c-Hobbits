#include "../include/planificadorLP.h"
#include "../include/main.h"

extern t_log* logger;
extern t_config* config;

extern t_squeue *lista_procesos_new;
extern t_squeue *lista_procesos_ready;

extern sem_t grado_de_multiprogramacion;
extern sem_t proceso_en_cola_new;
extern sem_t proceso_en_cola_ready;

extern sem_t planificacion_new_iniciada;
extern sem_t planificacion_ready_iniciada;
extern sem_t planificacion_exec_iniciada;

extern bool planificacion_iniciada;

void atender_estados_new(){

    
    while(1){

        sem_wait(&proceso_en_cola_new);
        /*if(!planificacion_iniciada){
            break;
        }*/
        sem_wait(&planificacion_new_iniciada);
        sem_wait(&grado_de_multiprogramacion);
        t_pcb* pcb_auxiliar = squeue_pop(lista_procesos_new);
        cambiar_a_ready(pcb_auxiliar);
        sem_post(&proceso_en_cola_ready);
        sem_post(&planificacion_new_iniciada);
        //Una vez que pasa los estados de new a ready
        log_info(logger, "PID: %d - Estado Anterior: NEW - Estado Actual: READY", pcb_auxiliar->pid);

    }
}

void iniciar_PLP(){
    pthread_t hilo_plp; //hilo del planificador de largo plazo, para atender estados en new
    pthread_create(&hilo_plp, NULL, (void *) atender_estados_new, NULL);
    pthread_detach(hilo_plp);
}

void cambiar_a_ready(t_pcb* pcb){
    //uint32_t estado_anterior = pcb->estado;
    pcb->estado = READY;
    squeue_push(lista_procesos_ready, pcb);

    mostrar_cola_ready();
}


char* listar_pids(t_squeue* squeue){
    char* pids = string_new();    

    void obtener_string_pids(t_pcb* pcb){
        char* pid = string_itoa(pcb->pid);
        string_append_with_format(&pids, "%s, ", pid);
        free(pid);
    }

    //list_iterate(squeue->cola->elements, (void*) obtener_string_pids);

    squeue_iterate(squeue, (void*) obtener_string_pids);
    return pids;
}

void mostrar_cola_ready(){

    char* lista_pids = listar_pids(lista_procesos_ready);

    log_info(logger, "Cola ready: %s", lista_pids);

    free(lista_pids);

}