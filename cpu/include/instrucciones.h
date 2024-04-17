#ifndef CPU_INSTRUCCIONES_H_
#define CPU_INSTRUCCIONES_H_

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

void set_8(u_int8_t* reg,u_int8_t valor);
void set_32(u_int32_t* reg,u_int32_t valor);
void sum_8(u_int8_t* registroDestino,int registroOrigen);
void sum_32(u_int32_t* registroDestino,int registroOrigen);
void sub_8(u_int8_t* registroDestino,int registroOrigen);
void sub_32(u_int32_t* registroDestino,int registroOrigen);
void jnz(int reg,u_int32_t instruccion_proxima);
void io_gen_sleep(int interfaz,int tiempoEspera); //FALTA TODO DE ESTA

#endif