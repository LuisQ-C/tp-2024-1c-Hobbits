#ifndef CPU_CONEXIONESCPU_H_
#define CPU_CONEXIONESCPU_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include <pthread.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/log.h>

void iniciar_conexiones(t_config* config,t_log* logger);
void* atender_cliente(void* args);

#endif 