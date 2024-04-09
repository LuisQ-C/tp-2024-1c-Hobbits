#ifndef KERNEL_CONEXIONES_H_
#define KERNEL_CONEXIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include <pthread.h>

void iniciar_conexiones(t_config*,t_log*);
void escucharConexionesIO(void* datosServerInterfaces);
/*void escuchar_interfaces();
void crear_hilos(int*,int*);
void* gestionarConexionConInterfaces(void*);
void* gestionarConexionMemoria(void*);
void* gestionarConexionCPU(void* );*/

#endif 