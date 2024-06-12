#ifndef MEMORIA_CONEXION_CPU_
#define MEMORIA_CONEXION_CPU_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../../utils/include/protocolo.h"
#include "../include/archivos.h"
#include "user_space.h"
#include "config.h"
typedef struct
{
    int fd;
}info_fd_cpu;

void realizar_handshake_cpu(int fd_cpu);
void iniciar_hilo_cpu(int fd_cpu);
void conexionCPU(void*);
t_proceso* buscar_proceso_pid(int pid);
void imprimir_bitmap();

#endif