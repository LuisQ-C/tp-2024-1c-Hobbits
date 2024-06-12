#ifndef INTERFAZ_STDIN_H_
#define INTERFAZ_STDIN_H_

#include <commons/string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<commons/collections/list.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"


void stDin(t_config* config, int fd_conexion_kernel,int fd_conexion_memoria);

t_list* lista_pruebas();

void aniadir_porcion(t_list* lista,int base,int tamanio,int dir_fisica);

#endif