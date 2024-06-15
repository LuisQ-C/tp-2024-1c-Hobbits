#ifndef PLANIFICADORCP_H_
#define PLANIFICADORCP_H_

#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include "../../utils/include/protocolo.h"
#include "../include/iniciar_planificador.h"
#include "consola.h"
#include "manejo_interfaces.h"
#include <stdbool.h>

void ejecutar_procesos_exec(void);
//void pasar_a_cola_exec(void);
void planificacion_fifo(void);
void planificacion_rr(void);
void planificacion_vrr(void);
void iniciar_PCP(void);

void recibir_contexto_actualizado(int fd_dispatch);

void actualizar_pcb_ejecutado(t_pcb* pcb_a_actualizar,t_list* pcb_con_motivo);

bool manejar_motivo_interrupcion(t_pcb* pcb_a_actualizar,t_list* pcb_con_motivo);

typedef enum{
    SUCCESS = 777,
    INVALID_RESOURCE,
    INVALID_INTERFACE,
    OUT_OF_MEMORY_FIN,
    INTERRUPTED_BY_USER_READY,
    INTERRUPTED_BY_USER_NEW,
    INTERRUPTED_BY_USER_EXEC,
    INTERRUPTED_BY_USER_BLOCKED_REC
}motivos_fin;

void manejar_fin_con_motivo(int motivo_interrupcion, t_pcb* pcb_a_finalizar);

void fin_fin();

typedef struct
{
    int quantum;
    int pid;
}data;

void fin_fin_ready();
void hilo_quantum(void* arg);
void enviar_destruccion_proceso(int pid, int fd_memoria);
//void enviar_interrupcion(int interrupcion, int pid);

//void interrupcion_quantum(t_pcb* pcb_auxiliar);

#endif