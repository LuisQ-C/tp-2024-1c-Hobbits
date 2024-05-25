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
void iniciar_PCP(void);

void recibir_contexto_actualizado(int fd_dispatch);

void actualizar_pcb_ejecutado(t_pcb* pcb_a_actualizar,t_list* pcb_con_motivo);

void manejar_motivo_interrupcion(t_pcb* pcb_a_actualizar,t_list* pcb_con_motivo);

//void interrupcion_quantum(t_pcb* pcb_auxiliar);

#endif