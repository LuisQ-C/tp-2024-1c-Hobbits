#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"
#include "../include/conexionesCPU.h"
#include <pthread.h>
#include "../include/cicloInstruccion.h"
#include "../include/instrucciones.h"
#include "../include/dispatch.h"
#include "../include/interrupt.h"
#include "../include/conversores.h"
#include "mmu.h"
#include "tlb.h"



//void* atender_cliente(void*);
//t_buffer *persona_serializar(t_persona *persona);
//void mandar_persona();
//void manejarConexionKernel(t_log* logger,int* cliente_fd_conexion_dispatch,int* cliente_fd_conexion_interrupt);


#endif