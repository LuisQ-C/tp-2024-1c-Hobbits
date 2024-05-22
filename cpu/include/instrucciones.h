#ifndef CPU_INSTRUCCIONES_H_
#define CPU_INSTRUCCIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include "../../utils/include/protocolo.h"
#include "../include/conversores.h"
//FALTAN CAMPOS EN EL REGISTRO QUE PIDE LA CONSIGNA


void set_8(uint8_t* reg,uint8_t valor);
void set_32(uint32_t* reg,uint32_t valor);
int set(char** instruccion);

void sum_8_32(uint8_t* registroDestino,uint32_t* registroOrigen);
void sum_8_8(uint8_t* registroDestino,uint8_t* registroOrigen);
void sum_32_8(uint32_t* registroDestino,uint8_t* registroOrigen);
void sum_32_32(uint32_t* registroDestino,uint32_t* registroOrigen);
void sum(char** instruccion);

void sub_8_8(uint8_t* registroDestino,uint8_t* registroOrigen);
void sub_32_8(uint32_t* registroDestino,uint8_t* registroOrigen);
void sub_32_32(uint32_t* registroDestino,uint32_t* registroOrigen);
void sub_8_32(uint8_t* registroDestino,uint32_t* registroOrigen);
void sub(char** instruccion);

void jnz_8(uint8_t* reg,uint32_t instruccion_proxima);
void jnz_32(uint32_t* reg,uint32_t instruccion_proxima);
void jnz(char** instruccion);

void io_gen_sleep(t_pcb* pcb_a_enviar,char** instruccionDesarmada,int fd_dispatch);

void instruccion_exit(t_pcb* pcb_a_enviar,int fd_dispatch);

enum{
    PC_MODIFICADO,
    PC_SIN_MODIFICAR
};




#endif