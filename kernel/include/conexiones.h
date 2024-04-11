#ifndef KERNEL_CONEXIONES_H_
#define KERNEL_CONEXIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include <pthread.h>

//INICIAR CONEXIONES DEBE TENER UN MANEJO DE ERRORES
void iniciar_conexiones(t_config* config,t_log* logger,int* fd_memoria,int* fd_cpu_dispatch, int* fd_cpu_interrupt,int* fd_escucha_interfaces);
void escucharConexionesIO(void* datosServerInterfaces);

/*void escuchar_interfaces();
void crear_hilos(int*,int*);
void* gestionarConexionConInterfaces(void*);
void* gestionarConexionMemoria(void*);
void* gestionarConexionCPU(void* );*/

#endif 