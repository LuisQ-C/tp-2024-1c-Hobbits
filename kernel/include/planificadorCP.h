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

void ejecutar_procesos_exec(void);
void pasar_a_cola_exec(void);
void planificacion_rr(void);
void planificacion_fifo(void);
void iniciar_PCP(void);

#endif