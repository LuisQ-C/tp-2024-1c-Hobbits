#include "../include/cicloInstruccion.h"

extern t_registro_cpu registro;


/*
 CICLO_DE_INSTRUCCION(){

    instruccion fetch();
    void decodificar(instruccion);
    tipo ejecutar(instruccion,a,b);
    ir al hilo para chequear interrupcion
}*/

void realizarCicloInstruccion(t_log* logger, int fd_conexion_memoria)
{
    //while(1){
    //FALTA SOLICITAR INSTRUCCION
    enviar_mensaje("3",fd_conexion_memoria,INSTRUCCION);
    //RECIBE LA INSTRUCCION SOLICITADA
    t_instruccion instruccion = recibirInstruccion(logger,fd_conexion_memoria);
    //DECODIFICA LA INSTRUCCION Y EJECUTA "SET AX 3"
    char** instruccionDesarmada = string_split(instruccion," ");
    int op_code = string_to_opcode(instruccionDesarmada[0]);

    switch(op_code)
    {
        case SET:
        {
            if(instruccionDesarmada[1][0] != 'E')
            {
                uint8_t* registroRecibido = string_to_register8(instruccionDesarmada[1]);
                set_8(registroRecibido,atoi(instruccionDesarmada[2]));
            }
            else{
                uint32_t* registroRecibido = string_to_register32(instruccionDesarmada[1]);
                set_32(registroRecibido,atoi(instruccionDesarmada[2]));
            }
            break;
        }
        case SUM:
        {
            if(instruccionDesarmada[1][0] != 'E')
            {
                uint8_t* registroDestino = string_to_register8(instruccionDesarmada[1]);
                uint8_t* registroOrigen = string_to_register8(instruccionDesarmada[2]);
                sum_8(registroDestino,registroOrigen);
            }
            else{
                uint32_t* registroDestino = string_to_register32(instruccionDesarmada[1]);
                uint32_t* registroOrigen = string_to_register32(instruccionDesarmada[2]);
                sum_32(registroDestino,registroOrigen);
            }
            break;
        }
        case SUB:
        {
            if(instruccionDesarmada[1][0] != 'E')
            {
                uint8_t* registroDestino = string_to_register8(instruccionDesarmada[1]);
                uint8_t* registroOrigen = string_to_register8(instruccionDesarmada[2]);
                sub_8(registroDestino,registroOrigen);
            }
            else{
                uint32_t* registroDestino = string_to_register32(instruccionDesarmada[1]);
                uint32_t* registroOrigen = string_to_register32(instruccionDesarmada[2]);
                sub_32(registroDestino,registroOrigen);
            }
            break;
        }
        case JNZ:
            break;
        case IO_GEN_SLEEP:
            break;
        case -1:
            log_warning(logger,"Instruccion invalida");
        default:
            log_error(logger,"Error al decodificar instruccion");
    }

    //LIBERAMOS EL CHAR** INSTRUCCIONDESARMADA Y LUEGO EL CHAR* INSTRUCCION
    string_array_destroy(instruccionDesarmada);
    free(instruccion);
    //CHEQUEA SI EN EL HILO DE INTERRUPCION LE LLEGO UNA INTERRUPCION
    //SINO LLEGO NADA, CONTINUA LA EJECUCION NORMALMENTE (IP+1 SI NO HAY JUMP)

    //}
}

/*Recibe la instruccion de la memoria, necesita free luego de utilizarse*/
t_instruccion recibirInstruccion(t_log* logger, int fd_conexion_memoria)
{
    int cod_op;
    t_instruccion instruccion = string_new();
    cod_op = recibir_operacion(fd_conexion_memoria);
    if(cod_op == -1)
    {
        log_error(logger,"Error al recibir la instruccion");
    }
    log_info(logger,"Valor de CODOP: %d",cod_op);
    char* instruccionRecibida = recibir_mensaje(fd_conexion_memoria,logger);
    string_append(&instruccion,instruccionRecibida);
    free(instruccionRecibida);
    //instruccion = recibir_mensaje(fd_conexion_memoria,logger);
    return instruccion;
}

/*Funcion que convierte char* a opcode del enum, en caso de error retorna -1*/
int string_to_opcode(char* instruccion)
{
    if(string_equals_ignore_case("SET",instruccion))
    {
        return SET;
    }
    else if(string_equals_ignore_case("SUM",instruccion))
    {
        return SUM;
    }
    else if(string_equals_ignore_case("SUB",instruccion))
    {
        return SUB;
    }
    else if(string_equals_ignore_case("JNZ",instruccion))
    {
        return JNZ;
    }
    else if(string_equals_ignore_case("IO_GEN_SLEEP",instruccion))
    {
        return IO_GEN_SLEEP;
    }
    else
    {
        return -1;
    }
}

/*Funcion que convierte char* a uint8_t*, en caso de error retorna NULL*/
uint8_t* string_to_register8(char* registroConvertir)
{
    if(string_equals_ignore_case("AX",registroConvertir))
    {
        return &registro.AX;
    }
    else if(string_equals_ignore_case("BX",registroConvertir))
    {
        return &registro.BX;
    }
    else if(string_equals_ignore_case("CX",registroConvertir))
    {
        return &registro.CX;
    }
    else if(string_equals_ignore_case("DX",registroConvertir))
    {
        return &registro.DX;
    }
    else{
        return NULL;
    }
}

/*Funcion que convierte char* a uint32_t*, en caso de error retorna NULL*/
uint32_t* string_to_register32(char* registroConvertir)
{
    if(string_equals_ignore_case("EAX",registroConvertir))
    {
        return &registro.EAX;
    }
    else if(string_equals_ignore_case("EBX",registroConvertir))
    {
        return &registro.EBX;
    }
    else if(string_equals_ignore_case("ECX",registroConvertir))
    {
        return &registro.ECX;
    }
    else if(string_equals_ignore_case("EDX",registroConvertir))
    {
        return &registro.EDX;
    }
    else{
        return NULL;
    }
}





