#ifndef CPU_INTERRUPT_H
#define CPU_INTERRUPT_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../../utils/include/protocolo.h"

typedef struct{
    int fd;
}info_fd_conexion;


void inicializar_hilo_interrupt(int cliente_fd_conexion_interrupt);
void realizar_handshake_interrupt(int fd_interrupt);
void manejarConexionInterrupt(void* fd_interrupt);

#endif