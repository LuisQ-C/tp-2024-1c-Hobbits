#ifndef INTERFAZ_GENERICA_H_
#define INTERFAZ_GENERICA_H_

#include<stdio.h>
#include<stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"
#include "interfazGen.h"
#include "interfazStdin.h"
#include "interfazStdout.h"
#include "interfazDialFS.h"


void iniciarInterfaz(char* nombreDeInterfaz,  t_config* config, int fd_conexion_kernel,int fd_conexion_memoria );
//void interfazGenerica(t_config* config, int fd_conexion_kernel);

#endif