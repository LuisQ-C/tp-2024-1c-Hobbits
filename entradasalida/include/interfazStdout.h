#ifndef INTERFAZ_STDOUT_H_
#define INTERFAZ_STDOUT_H_

#include <commons/string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"


void stdOut(t_config* config, int fd_conexion_kernel,int fd_conexion_memoria);


t_list* lista_pruebas_out();

void aniadir_porcion_out(t_list* lista,int base,int tamanio,int dir_fisica);

#endif