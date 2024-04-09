#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"
#include <pthread.h>
void* atender_cliente(void*);
void iniciar_conexiones(t_config*,t_log*);
#endif