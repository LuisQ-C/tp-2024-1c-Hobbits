#ifndef  MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"
//#include<commons/log.h> //utils ya tiene un include de log
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include"../include/conexiones.h"
#include "../include/iniciar_planificador.h"
#include "../include/planificadorLP.h"

#include <pthread.h>
void iniciar_consola(void* fd_info);
void conexion_dispatch(void* dispatch);
void destruir_pcb_con_motivo(void* self);

typedef struct
{
    int fd_memoria;
    int fd_cpu_dispatch;
    int fd_cpu_interrupt;
    int fd_escucha_interfaces;
}info_fd;

#endif