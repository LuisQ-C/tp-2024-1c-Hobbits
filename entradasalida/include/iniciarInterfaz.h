#ifndef INTERFAZ_GENERICA_H_
#define INTERFAZ_GENERICA_H_

#include<stdio.h>
#include<stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"
#include "../include/interfazGenerica.h"

void iniciarInterfaz(char* nombreDeInterfaz,  t_config* config, int fd_conexion_kernel );

#endif