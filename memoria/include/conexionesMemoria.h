#ifndef MEMORIA_CONEXIONESMEMORIA_H_
#define MEMORIA_CONEXIONESMEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include "../../utils/include/logsConfigs.h"
#include"../include/archivos.h"
#include"../include/conexionCPU.h"
#include"../include/conexionKernel.h"
#include <pthread.h>


int iniciar_conexiones(int* server_fd,int* fd_cpu,int* fd_kernel);
void realizar_handshakes_memoria(int fd_cpu, int fd_kernel);
void inicializar_hilos(int fd_cpu, int fd_kernel);
void terminar_programa(int* fd_cpu,int* fd_kernel);
void procesarConexionesIO(void* datosServerInterfaces);
int escucharConexionesIO(int fd_escucha_interfaces);
void conexionCPU(void* fds);
void conexionKernel(void* fds2);
//void conexionCPU(void* fds);
//void conexionKernel(void* fds2);

#endif