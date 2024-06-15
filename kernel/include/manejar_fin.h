#ifndef MANEJAR_FIN_H
#define MANEJAR_FIN_H

#include<commons/string.h>
#include<commons/config.h>
#include "../../utils/include/protocolo.h"
#include "../../utils/include/listas.h"
//#include "iniciar_planificador.h"
#include "manejo_recursos.h"
#include <stdbool.h>



void manejar_fin_con_motivo(int motivo_interrupcion, t_pcb* pcb_a_finalizar);
void enviar_destruccion_proceso(int pid, int fd_memoria);

#endif