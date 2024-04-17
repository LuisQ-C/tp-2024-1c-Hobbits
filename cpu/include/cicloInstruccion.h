#ifndef CPU_CICLOINSTRUCCION_H_
#define CPU_CICLOINSTRUCCION_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    u_int32_t pc;
    u_int8_t ax;
    u_int8_t bx;
    u_int8_t cx;
    u_int8_t dx;
    u_int32_t eax;
} t_registro_cpu;


char* identificarInstruccion(char* instruccion);
int contadorHastaPrimerEspacio(char* instruccion);

#endif