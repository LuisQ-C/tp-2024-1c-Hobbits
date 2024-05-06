#ifndef CPU_CICLOINSTRUCCION_H_
#define CPU_CICLOINSTRUCCION_H_

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/string.h>
#include "../include/instrucciones.h"
#include "../include/conversores.h"
#include "../../utils/include/protocolo.h"
#include <stdint.h>

typedef char* t_instruccion;

//FUNCION PRINCIPAL DE LA CPU
void realizarCicloInstruccion(int fd_conexion_memoria,t_pcb* pcb_recibido,int cliente_fd_conexion_dispatch);

t_instruccion fetch(uint32_t pc, int fd_conexion_memoria, int pid);
void pedir_instruccion(uint32_t pc,int pid,int fd_conexion_memoria);
t_instruccion recibirInstruccion(int fd_conexion_memoria);
void resetear_var_globales();
void establecer_contexto(t_pcb* pcb_recibido);
void actualizar_pcb(t_pcb* pcb_a_actualizar);
void decode_and_execute(t_instruccion instruccion,t_pcb* pcb_a_enviar,int fd_dispatch);
void logear_instruccion_ejecutada(int pid,char* instruccion);
int fue_desalojado();
int check_interrupt(t_pcb* pcb_a_chequear,int fd_dispatch);




#endif