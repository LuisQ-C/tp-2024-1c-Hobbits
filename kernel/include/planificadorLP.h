#ifndef PLANIFICADORLP_H_
#define PLANIFICADORLP_H_

#include<commons/string.h>
#include<commons/config.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include "../../utils/include/protocolo.h"
#include "../include/iniciar_planificador.h"

void iniciar_PLP(void);
void cambiar_a_ready(t_pcb* pcb);
void mostrar_cola_ready(void);
void atender_estados_new(void);
char* listar_pids(t_squeue* squeue);

#endif