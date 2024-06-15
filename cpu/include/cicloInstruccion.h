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
#include "mmu.h"

typedef char* t_instruccion;

//FUNCION PRINCIPAL DE LA CPU
void realizarCicloInstruccion(int fd_conexion_memoria,t_pcb* pcb_recibido,int cliente_fd_conexion_dispatch);

//void resetear_var_globales();
void resetear_interrupcion();
//void resetear_pid_actual();
//void resetear_motivo_interrupcion();
void establecer_contexto(t_pcb* pcb_recibido);

t_instruccion fetch(uint32_t pc, int fd_conexion_memoria, int pid);
void pedir_instruccion(uint32_t pc,int pid,int fd_conexion_memoria);
t_instruccion recibirInstruccion(int fd_conexion_memoria);

void decode_and_execute(t_instruccion instruccion,t_pcb* pcb_a_enviar,int fd_dispatch,int fd_memoria);
void logear_instruccion_ejecutada(int pid,char* instruccion);

void actualizar_pcb(t_pcb* pcb_a_actualizar);

int fue_desalojado();
void desalojar_proceso(t_pcb* pcb_a_desalojar,int motivo_desalojo,int fd_dispatch);

int check_interrupt(t_pcb* pcb_a_chequear,int fd_dispatch);

//SOLICITAR ESCRITURA A MEMORIA
void solicitar_lectura(int pid,void** ptro_dato,int tam_dato_leer, int fd_destino, int offset, t_list* marcos, int tipo_dato);

void solicitar_escritura(int pid,void* ptro_dato,int tam_dato_escribir, int fd_destino, int offset, t_list* marcos, int tipo_dato);

void memcpy_fragmento_void(void* ptro_dato,int base,void* fragmento,int tamanio_fragmento);

void imprimir_registros(t_pcb* pcb); //PARA TESTS

#endif