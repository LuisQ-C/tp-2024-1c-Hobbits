#ifndef IO_CONEXIONESIO_H_
#define IO_CONEXIONESIO_H_

#include<stdio.h>
#include<stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfig.h"

void conectar_interfaz(t_log* logger, t_config* config);

#endif