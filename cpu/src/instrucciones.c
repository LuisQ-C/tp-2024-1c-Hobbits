#include "../include/instrucciones.h"

extern t_registro_cpu registro;

// INSTRUCCIONES 
// SET REGISTRO VALOR
void set_8(u_int8_t* reg,u_int8_t valor)
{
    *reg = valor;
}

void set_32(u_int32_t* reg,u_int32_t valor)
{
    *reg = valor;
}

// SUM REGISTRO_DESTINO REGISTRO_ORIGEN
void sum_8(u_int8_t* registroDestino,int registroOrigen)
{
    *registroDestino += registroOrigen;
}

void sum_32(u_int32_t* registroDestino,int registroOrigen)
{
    *registroDestino += registroOrigen;
}

// SUB REGISTRO_DESTINO REGISTRO_ORIGEN (PREGUNTAR PROBLEMAS SOBRE RESULTADOS NEGATIVOS)
void sub_8(u_int8_t* registroDestino,int registroOrigen) 
{
    *registroDestino -= registroOrigen;
}

void sub_32(u_int32_t* registroDestino,int registroOrigen)
{
    *registroDestino -= registroOrigen;
}

// JNZ REGISTRO INSTRUCCION (aprobado?)
void jnz(int reg,u_int32_t instruccion_proxima){
    if(reg == 0)
        registro.pc = instruccion_proxima;
}

// IO_GEN_SLEPP INTERFAZ UNIDADES_TRABAJO 
void io_gen_sleep(int interfaz,int tiempoEspera){
    //mandarle a kernel para que kernel le mande a la interfaz que se "duerma" n tiemposEspera
}