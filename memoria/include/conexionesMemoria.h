#ifndef MEMORIA_CONEXIONESMEMORIA_H_
#define MEMORIA_CONEXIONESMEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include <pthread.h>


void iniciar_conexiones(t_config* config,t_log* logger);
void atender_interfaces(void* datosServerIO);

#endif