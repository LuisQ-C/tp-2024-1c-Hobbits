#include "../include/instrucciones.h"

extern t_registro_cpu registro;
extern t_log* logger;
extern t_config* config;

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
void sum_8_8(uint8_t* registroDestino,uint8_t* registroOrigen)
{
    //uint8_t numero_a_sumar = *registroOrigen;
    *registroDestino += *registroOrigen;
}

void sum_8_32(uint8_t* registroDestino,uint32_t* registroOrigen)
{
    *registroDestino += *registroOrigen;
}

void sum_32_32(uint32_t* registroDestino,uint32_t* registroOrigen)
{
    *registroDestino += *registroOrigen;
}
void sum_32_8(uint32_t* registroDestino,uint8_t* registroOrigen)
{
    *registroDestino += *registroOrigen;
}

// SUB REGISTRO_DESTINO REGISTRO_ORIGEN (PREGUNTAR PROBLEMAS SOBRE RESULTADOS NEGATIVOS)
void sub_8_8(uint8_t* registroDestino,uint8_t* registroOrigen) 
{
    *registroDestino -= *registroOrigen;
}

void sub_8_32(uint8_t* registroDestino,uint32_t* registroOrigen) 
{
    *registroDestino -= *registroOrigen;
}

void sub_32_32(uint32_t* registroDestino,uint32_t* registroOrigen)
{
    *registroDestino -= *registroOrigen;
}

void sub_32_8(uint32_t* registroDestino,uint8_t* registroOrigen)
{
    *registroDestino -= *registroOrigen;
}

// JNZ REGISTRO INSTRUCCION (aprobado?)

void jnz_8(uint8_t* reg,uint32_t instruccion_proxima){
    if(*reg != 0)
        registro.PC = instruccion_proxima;
    else
        registro.PC++;
}

void jnz_32(uint32_t* reg,uint32_t instruccion_proxima){
    if(*reg != 0)
        registro.PC = instruccion_proxima;
    else
        registro.PC++;
}

/*Enviar al kernel la solicitud de que duerma a un entrada salida*/
void io_gen_sleep(t_pcb* pcb_a_enviar,char** instruccionDesarmada,int fd_dispatch){

    int motivo_desalojo = IO_GEN_SLEEP;

    t_paquete* paquete = armar_paquete_pcb(pcb_a_enviar);

    agregar_a_paquete(paquete,&motivo_desalojo,sizeof(int));
    agregar_a_paquete(paquete,instruccionDesarmada[1],sizeof(strlen(instruccionDesarmada[1])+1));

    int tiempo_sleep = atoi(instruccionDesarmada[2]);

    agregar_a_paquete(paquete,&tiempo_sleep,sizeof(int));

    enviar_paquete(paquete,fd_dispatch);
    eliminar_paquete(paquete);
}

void instruccion_exit(t_pcb* pcb_a_enviar,int fd_dispatch)
{
    int motivo_desalojado = EXIT;

    t_paquete* paquete = armar_paquete_pcb(pcb_a_enviar);

    agregar_a_paquete(paquete,&motivo_desalojado,sizeof(int));

    enviar_paquete(paquete,fd_dispatch);
    eliminar_paquete(paquete);
}



