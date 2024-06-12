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
#include "../include/dispatch.h"
#include "../include/interrupt.h"

typedef struct{
    int tam_memoria;
    int tam_pagina;
    int cant_marcos;
    int retardo_memoria;
} config_memoria;

int iniciar_conexiones(int* fd_conexion_memoria,int* server_fd_escucha_dispatch, int* server_fd_escucha_interrupt, int* cliente_fd_conexion_dispatch, int* cliente_fd_conexion_interrupt);
void manejarConexionKernel(int* cliente_fd_conexion_dispatch,int* cliente_fd_conexion_interrupt);
//void manejarConexionDispatch(int cliente_fd_conexion_dispatch);
void realizar_handshakes_cpu(int fd_conexion_memoria,int cliente_fd_conexion_dispatch, int cliente_fd_conexion_interrupt);
void habilitar_dispatch_interrupt(int fd_dispatch, int fd_interrupt, int fd_memoria);
//void inicializar_hilo_interrupt(int cliente_fd_conexion_interrupt);
//void manejarConexionInterrupt(void* fd_interrupt);
//void manejarConexionDispatch(int cliente_fd_conexion_dispatch);
void calcular_marcos();
void terminar_programa(int* fd_conexion_memoria,int* cliente_fd_conexion_dispatch,int*cliente_fd_conexion_interrupt);


#endif 