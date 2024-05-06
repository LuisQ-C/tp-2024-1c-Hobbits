#ifndef CPU_CONVERSORES_H
#define CPU_CONVERSORES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/string.h>
#include "../../utils/include/protocolo.h"
//#include "../include/instrucciones.h"
//#include "../include/cicloInstruccion.h"

typedef struct 
{
    uint32_t PC;
    uint8_t AX;
    uint8_t BX;
    uint8_t CX;
    uint8_t DX;
    uint32_t EAX;
    uint32_t EBX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t SI;
    uint32_t DI;
} t_registro_cpu;

extern t_registro_cpu registro;

int string_to_opcode(char* instruccion);
uint8_t* string_to_register8(char* registroConvertir);
uint32_t* string_to_register32(char* registroConvertir);

#endif