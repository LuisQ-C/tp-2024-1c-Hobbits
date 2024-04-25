#ifndef CPU_CONEXIONESCPU_H_
#define CPU_CONEXIONESCPU_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"
#include <pthread.h>
#include<commons/string.h>
#include<commons/collections/list.h>

int iniciar_conexiones(int* fd_conexion_memoria,int* server_fd_escucha_dispatch, int* server_fd_escucha_interrupt, int* cliente_fd_conexion_dispatch, int* cliente_fd_conexion_interrupt);
void manejarConexionKernel(int* cliente_fd_conexion_dispatch,int* cliente_fd_conexion_interrupt);
void manejarConexionDispatch(int cliente_fd_conexion_dispatch);
void inicializar_hilo_interrupt(int cliente_fd_conexion_interrupt);
void manejarConexionInterrupt(void* fd_interrupt);
void manejarConexionDispatch(int cliente_fd_conexion_dispatch);
void terminar_programa(int* fd_conexion_memoria,int* cliente_fd_conexion_dispatch,int*cliente_fd_conexion_interrupt);


#endif 