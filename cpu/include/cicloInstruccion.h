#ifndef CPU_CICLOINSTRUCCION_H_
#define CPU_CICLOINSTRUCCION_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include "../include/instrucciones.h"
#include "../../utils/include/protocolo.h"

typedef char* t_instruccion;
//FUNCION PRINCIPAL DE LA CPU
void realizarCicloInstruccion(t_log* logger, int fd_conexion_memoria);

t_instruccion recibirInstruccion(t_log* logger, int fd_conexion_memoria);
char* identificarInstruccion(char* instruccion);
int string_to_opcode(char* instruccion);
uint8_t* string_to_register8(char* registroConvertir);
uint32_t* string_to_register32(char* registroConvertir);
int contadorHastaPrimerEspacio(char* instruccion);

#endif