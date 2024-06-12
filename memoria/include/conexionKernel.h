#ifndef MEMORIA_CONEXION_KERNEL_
#define MEMORIA_CONEXION_KERNEL_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../../utils/include/protocolo.h"
#include "../include/archivos.h"
#include "config.h"
typedef struct
{
    int fd;
}info_fd_kernel;

void realizar_handshake_kernel(int fd_kernel);
void iniciar_hilo_kernel(int fd_kernel);
void conexionKernel(void* info_fd);
#endif