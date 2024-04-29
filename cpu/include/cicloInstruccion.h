#ifndef CPU_CICLOINSTRUCCION_H_
#define CPU_CICLOINSTRUCCION_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include "../include/instrucciones.h"
#include "../../utils/include/protocolo.h"
#include <stdint.h>

typedef char* t_instruccion;

//FUNCION PRINCIPAL DE LA CPU
void realizarCicloInstruccion(int fd_conexion_memoria,t_pcb* pcb_recibido,int cliente_fd_conexion_dispatch);

t_instruccion solicitarInstruccion(uint32_t pc, int fd_conexion_memoria);
t_instruccion recibirInstruccion(int fd_conexion_memoria);
void establecer_contexto(t_pcb* pcb_recibido);
void actualizar_pcb(t_pcb* pcb_a_actualizar);
void decode_and_execute(t_instruccion instruccion);
int check_interrupt(t_pcb* pcb_a_chequear,int fd_dispatch);
int string_to_opcode(char* instruccion);
uint8_t* string_to_register8(char* registroConvertir);
uint32_t* string_to_register32(char* registroConvertir);

void set(char** instruccion);
void sum(char** instruccion);
void sub(char** instruccion);
void jnz(char** instruccion);

#endif