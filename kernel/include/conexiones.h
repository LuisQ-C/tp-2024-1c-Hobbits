#ifndef KERNEL_CONEXIONES_H_
#define KERNEL_CONEXIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include <pthread.h>

int generar_conexiones(t_log*,int*,int*,t_config*);
void* gestionarConexionMemoria(void*);
void* gestionarConexionCPU(void* );

#endif 