#ifndef KERNEL_CONEXIONES_H_
#define KERNEL_CONEXIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
#include "../../utils/include/logsConfigs.h"
#include <pthread.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include "manejo_interfaces.h"
#include "planificadorLP.h"




//INICIAR CONEXIONES DEBE TENER UN MANEJO DE ERRORES
int iniciar_conexiones(t_config* config,t_log* logger,int* fd_memoria,int* fd_cpu_dispatch, int* fd_cpu_interrupt,int* fd_escucha_interfaces);
void realizar_handshakes_kernel(int fd_memoria,int fd_cpu_dispatch, int fd_cpu_interrupt);
int escucharConexionesIO(t_log* logger,int fd_escucha_interfaces);
void procesarConexionesIO(void* datosServerInterfaces);
int string_to_type(char* tipo);
void atender_interfaz_generica(t_list_io* interfaz);
void terminar_programa(t_log* logger,t_config* config,int* fd_memoria,int* fd_cpu_dispatch,int* fd_cpu_interrupt);
void atender_interfaz_stdin_stdout(t_list_io* interfaz, int tipo_interfaz);
//void escucharConexionesIO(void* datosServerInterfaces);

void mandar_pcb_cola_correspondiente(t_pcb* pcb, int cola_destino);


#endif 