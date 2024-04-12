#ifndef IO_CONEXIONESIO_H_
#define IO_CONEXIONESIO_H_

#include<stdio.h>
#include<stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"

void conectar_interfaz(t_log* logger, t_config* config,int* fd_conexion_kernel,int* fd_conexion_memoria);
void terminar_programa(t_log* logger, t_config* config,int* fd_conexion_kernel,int* fd_conexion_memoria);

#endif