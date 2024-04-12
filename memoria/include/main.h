#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>

#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"
#include <pthread.h>
#include"../include/conexionesMemoria.h"

//extern t_log* logger;

//extern t_log* logger;
//void* atender_cliente(void*);
void manejarConexionCPU();
#endif