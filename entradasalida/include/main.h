#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include "../include/conexionesIO.h"
void iniciarInterfaz(char nombre,  t_config* config, int fd_conexion_kernel );
void interfazGenerica( t_config* config, int fd_conexion_kernel,t_log* logger);

#endif
