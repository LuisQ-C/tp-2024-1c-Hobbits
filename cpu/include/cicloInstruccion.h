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
void realizarCicloInstruccion(int fd_conexion_memoria);

t_instruccion solicitarInstruccion(uint32_t pc, int fd_conexion_memoria);
t_instruccion recibirInstruccion(int fd_conexion_memoria);
void decode_and_execute(t_instruccion instruccion);
char* identificarInstruccion(char* instruccion);
int string_to_opcode(char* instruccion);
uint8_t* string_to_register8(char* registroConvertir);
uint32_t* string_to_register32(char* registroConvertir);
int contadorHastaPrimerEspacio(char* instruccion);

#endif