#include "../include/manejar_fin.h"

extern int fd_mem;
extern t_squeue* lista_procesos_exit;
extern sem_t grado_de_multiprogramacion;
extern t_log* logger;

void manejar_fin_con_motivo(int motivo_interrupcion, t_pcb* pcb_a_finalizar){
    pcb_a_finalizar->estado = EXIT;
    int pid = pcb_a_finalizar->pid;
    liberar_recursos(pid);
    enviar_destruccion_proceso(pid, fd_mem);
    switch (motivo_interrupcion){
    case SUCCESS:
        squeue_push(lista_procesos_exit, pcb_a_finalizar);
        log_info(logger, "Finaliza el proceso %d - Motivo: SUCCESS", pcb_a_finalizar->pid);
        log_info(logger, "PID: %d - Estado Anterior: EXECUTE - Estado Actual: EXIT", pcb_a_finalizar->pid);
        break;
    case INVALID_RESOURCE:
        squeue_push(lista_procesos_exit, pcb_a_finalizar);
        log_info(logger, "Finaliza el proceso %d - Motivo: INVALID RESOURCE", pcb_a_finalizar->pid);
        log_info(logger, "PID: %d - Estado Anterior: EXECUTE - Estado Actual: EXIT", pcb_a_finalizar->pid);
        break;
    case INVALID_INTERFACE:
        squeue_push(lista_procesos_exit, pcb_a_finalizar);
        log_info(logger, "Finaliza el proceso %d - Motivo: INVALID INTERFACE", pcb_a_finalizar->pid);
        log_info(logger, "PID: %d - Estado Anterior: EXECUTE - Estado Actual: EXIT", pcb_a_finalizar->pid);
        break;
    case OUT_OF_MEMORY_FIN:
        squeue_push(lista_procesos_exit, pcb_a_finalizar);
        log_info(logger, "Finaliza el proceso %d - Motivo : OUT_OF_MEMORY", pcb_a_finalizar->pid);
        log_info(logger, "PID: %d - Estado Anterior: EXECUTE - Estado Actual: EXIT", pcb_a_finalizar->pid);
        break;
    case INTERRUPTED_BY_USER_READY:
        squeue_push(lista_procesos_exit, pcb_a_finalizar);
        log_info(logger, "Finaliza el proceso %d - Motivo: INTERRUPTED BY USER", pcb_a_finalizar->pid);
        log_info(logger, "PID: %d - Estado Anterior: READY - Estado Actual: EXIT", pcb_a_finalizar->pid);
        /*pthread_t hilo_fin_ready;
        pthread_create(&hilo_fin_ready, NULL, (void*) fin_fin_ready, NULL);
        pthread_detach(hilo_fin_ready);//Agregar un hilo para que no se bloquee*/
        sem_post(&grado_de_multiprogramacion);
        break;
    case INTERRUPTED_BY_USER_NEW:
        squeue_push(lista_procesos_exit, pcb_a_finalizar);
        log_info(logger, "Finaliza el proceso %d - Motivo: INTERRUPTED BY USER", pcb_a_finalizar->pid);
        log_info(logger, "PID: %d - Estado Anterior: NEW - Estado Actual: EXIT", pcb_a_finalizar->pid);
        /*pthread_t hilo_fin;
        pthread_create(&hilo_fin, NULL, (void*)fin_fin, NULL);
        pthread_detach(hilo_fin);*/
        break;
    case INTERRUPTED_BY_USER_EXEC:
        squeue_push(lista_procesos_exit, pcb_a_finalizar);
        log_info(logger, "Finaliza el proceso %d - Motivo: INTERRUPTED BY USER", pcb_a_finalizar->pid);
        log_info(logger, "PID: %d - Estado Anterior: EXEC - Estado Actual: EXIT", pcb_a_finalizar->pid);
        sem_post(&grado_de_multiprogramacion);
        break;
    case INTERRUPTED_BY_USER_BLOCKED:
        squeue_push(lista_procesos_exit, pcb_a_finalizar);
        log_info(logger, "Finaliza el proceso %d - Motivo: INTERRUPTED BY USER", pcb_a_finalizar->pid);
        log_info(logger, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: EXIT", pcb_a_finalizar->pid);
        sem_post(&grado_de_multiprogramacion);
        break;
    default:
        break;
    }

}

void enviar_destruccion_proceso(int pid, int fd_memoria){
    t_paquete* paquete = crear_paquete(FINALIZAR_PROCESO);
    agregar_a_paquete(paquete, &pid, sizeof(int));
    enviar_paquete(paquete, fd_memoria);
    eliminar_paquete(paquete);
}