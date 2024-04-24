#include "../include/instrucciones.h"

extern t_registro_cpu registro;

// INSTRUCCIONES 
// SET REGISTRO VALOR
void set_8(uint8_t* reg,uint8_t valor)
{
    *reg = valor;
}

void set_32(uint32_t* reg,uint32_t valor)
{
    *reg = valor;
}

// SUM REGISTRO_DESTINO REGISTRO_ORIGEN
void sum_8(uint8_t* registroDestino,uint8_t* registroOrigen)
{
    *registroDestino += *registroOrigen;
}

void sum_32(uint32_t* registroDestino,uint32_t* registroOrigen)
{
    *registroDestino += *registroOrigen;
}

// SUB REGISTRO_DESTINO REGISTRO_ORIGEN (PREGUNTAR PROBLEMAS SOBRE RESULTADOS NEGATIVOS)
void sub_8(uint8_t* registroDestino,uint8_t* registroOrigen) 
{
    *registroDestino -= *registroOrigen;
}

void sub_32(uint32_t* registroDestino,uint32_t* registroOrigen)
{
    *registroDestino -= *registroOrigen;
}

// JNZ REGISTRO INSTRUCCION (aprobado?)

void jnz_8(uint8_t* reg,uint32_t instruccion_proxima){
    if(*reg != 0)
        registro.PC = instruccion_proxima;
}

void jnz_32(uint32_t* reg,uint32_t instruccion_proxima){
    if(*reg != 0)
        registro.PC = instruccion_proxima;
}

// IO_GEN_SLEPP INTERFAZ UNIDADES_TRABAJO 
void io_gen_sleep(int interfaz,int tiempoEspera){
    //mandarle a kernel para que kernel le mande a la interfaz que se "duerma" n tiemposEspera
}