#ifndef MEMORIA_CONEXIONESMEMORIA_H_
#define MEMORIA_CONEXIONESMEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include "../../utils/include/logsConfigs.h"
#include <pthread.h>


void iniciar_conexiones(t_log* logger,t_config* config,int* server_fd,int* fd_cpu,int* fd_kernel);
//void atender_interfaces(void* datosServerIO);
void terminar_programa(t_log* logger,t_config* config,int* fd_cpu,int* fd_kernel);
void procesarConexionesIO(void* datosServerInterfaces);
int escucharConexionesIO(t_log* logger,int fd_escucha_interfaces);

#endif