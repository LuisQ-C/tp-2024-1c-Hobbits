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
#include "mmu.h"


//FALTAN CAMPOS EN EL REGISTRO QUE PIDE LA CONSIGNA


void set_8(uint8_t* reg,uint8_t valor);
void set_32(uint32_t* reg,uint32_t valor);
int set(char* registro_setear, int numero_setear);

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

int resize(int pid,int new_size,int fd_memoria);

void mov_out(int pagina,int desplazamiento, uint32_t* dato_a_escribir,int fd_memoria);

void instruccion_signal(t_pcb* pcb_a_enviar,char* nombre_recurso, int fd_dispatch);

void instruccion_wait(t_pcb* pcb_a_enviar, char* nombre_recurso, int fd_dispatch);

void io_stdin_read(t_pcb* pcb_a_enviar,char* nombre_interfaz,int direccion_logica,int tamanio_dato,int fd_dispatch);

void asignar_porcion_dato(t_porcion_dato* porcion, int base, int dir_fisica, int tamanio);


enum{
    PC_MODIFICADO,
    PC_SIN_MODIFICAR
};




#endif