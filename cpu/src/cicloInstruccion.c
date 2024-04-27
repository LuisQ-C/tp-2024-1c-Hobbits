#include "../include/cicloInstruccion.h"

extern t_registro_cpu registro;
extern t_log* logger;

#define HAY_INTERRUPCION 0

void realizarCicloInstruccion(int fd_conexion_memoria, t_pcb* pcb_recibido)
{
    int i=0;
    establecer_contexto(pcb_recibido);
    while(i<3){
    // FETCH (SOLICITA Y RECIBE LA INSTRUCCION)
    t_instruccion instruccion = solicitarInstruccion(registro.PC,fd_conexion_memoria);
    
    // DECODIFICA LA INSTRUCCION Y LA EJECUTA
    decode_and_execute(instruccion);

    //ACTUALIZAR PCB
    actualizar_pcb(pcb_recibido);
    
    //CHEQUEA SI EN EL HILO DE INTERRUPCION LE LLEGO UNA INTERRUPCION
    //check_interrupt();
    i++;
    }
}

void establecer_contexto(t_pcb* pcb_recibido)
{
    registro.PC = pcb_recibido->pc;
    registro.AX = pcb_recibido->registros_CPU.AX;
    registro.BX = pcb_recibido->registros_CPU.BX;
    registro.CX = pcb_recibido->registros_CPU.CX;
    registro.DX = pcb_recibido->registros_CPU.DX;
    registro.EAX = pcb_recibido->registros_CPU.EAX;
    registro.EBX = pcb_recibido->registros_CPU.EBX;
    registro.ECX = pcb_recibido->registros_CPU.ECX;
    registro.EDX = pcb_recibido->registros_CPU.EDX;
}

void actualizar_pcb(t_pcb* pcb_a_actualizar)
{
    pcb_a_actualizar->pc = registro.PC;
    pcb_a_actualizar->registros_CPU.AX = registro.AX;
    pcb_a_actualizar->registros_CPU.BX = registro.BX;
    pcb_a_actualizar->registros_CPU.CX = registro.CX;
    pcb_a_actualizar->registros_CPU.DX = registro.DX;
    pcb_a_actualizar->registros_CPU.EAX = registro.EAX;
    pcb_a_actualizar->registros_CPU.EBX = registro.EBX;
    pcb_a_actualizar->registros_CPU.ECX = registro.ECX;
    pcb_a_actualizar->registros_CPU.EDX = registro.EDX;
}

/*Solicita la instruccion a memoria de acuerdo al pc indicado*/
t_instruccion solicitarInstruccion(uint32_t pc, int fd_conexion_memoria)
{
    char* pc_a_enviar = string_itoa(pc);
    enviar_mensaje(pc_a_enviar,fd_conexion_memoria,INSTRUCCION);
    t_instruccion instruccion = recibirInstruccion(fd_conexion_memoria);
    free(pc_a_enviar);
    return instruccion;
}

/*Recibe la instruccion de la memoria, necesita free luego de utilizarse*/
t_instruccion recibirInstruccion(int fd_conexion_memoria)
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
    return instruccion;
}

/* Decodifica y ejecuta la instruccion pasada por parametro*/
void decode_and_execute(t_instruccion instruccion)
{
    char** instruccionDesarmada = string_split(instruccion," ");
    int op_code = string_to_opcode(instruccionDesarmada[0]);

    switch(op_code)
    {
        case SET:
        {
            set(instruccionDesarmada);
            registro.PC = registro.PC + 1;
            break;
        }
        case SUM:
        {
            sum(instruccionDesarmada);
            registro.PC = registro.PC + 1;
            break;
        }
        case SUB:
        {
            sub(instruccionDesarmada);
            registro.PC = registro.PC + 1;
            break;
        }
        case JNZ:
        {
            jnz(instruccionDesarmada);
            break;
        }
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
    
}

void check_interrupt()
{
    if(HAY_INTERRUPCION)
    {
        //le devuelve el pcb con el contexto actualizado con motivoDesalojo
    }
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

void set(char** instruccion)
{
    if(instruccion[1][0] != 'E')
            {
                uint8_t* registroRecibido = string_to_register8(instruccion[1]);
                set_8(registroRecibido,atoi(instruccion[2]));
            }
            else{
                uint32_t* registroRecibido = string_to_register32(instruccion[1]);
                set_32(registroRecibido,atoi(instruccion[2]));
            }
}

void sum(char** instruccion)
{
    if(instruccion[1][0] != 'E')
            {
                if(instruccion[2][0] != 'E')
                {
                    uint8_t* registroDestino = string_to_register8(instruccion[1]);
                    uint8_t* registroOrigen = string_to_register8(instruccion[2]);
                    sum_8_8(registroDestino,registroOrigen);
                }
                else{
                    uint8_t* registroDestino = string_to_register8(instruccion[1]);
                    uint32_t* registroOrigen = string_to_register32(instruccion[2]);
                    sum_8_32(registroDestino,registroOrigen);
                }
            }
            else{
                if(instruccion[2][0] != 'E')
                {
                    uint32_t* registroDestino = string_to_register32(instruccion[1]);
                    uint8_t* registroOrigen = string_to_register8(instruccion[2]);
                    sum_32_8(registroDestino,registroOrigen);
                }
                else{
                    uint32_t* registroDestino = string_to_register32(instruccion[1]);
                    uint32_t* registroOrigen = string_to_register32(instruccion[2]);
                    sum_32_32(registroDestino,registroOrigen);
                }
            }
}

void sub(char** instruccion)
{
    if(instruccion[1][0] != 'E')
            {
                if(instruccion[2][0] != 'E')
                {
                    uint8_t* registroDestino = string_to_register8(instruccion[1]);
                    uint8_t* registroOrigen = string_to_register8(instruccion[2]);
                    sub_8_8(registroDestino,registroOrigen);
                }
                else{
                    uint8_t* registroDestino = string_to_register8(instruccion[1]);
                    uint32_t* registroOrigen = string_to_register32(instruccion[2]);
                    sub_8_32(registroDestino,registroOrigen);
                }
            }
            else{
                if(instruccion[2][0] != 'E')
                {
                    uint32_t* registroDestino = string_to_register32(instruccion[1]);
                    uint8_t* registroOrigen = string_to_register8(instruccion[2]);
                    sub_32_8(registroDestino,registroOrigen);
                }
                else{
                    uint32_t* registroDestino = string_to_register32(instruccion[1]);
                    uint32_t* registroOrigen = string_to_register32(instruccion[2]);
                    sub_32_32(registroDestino,registroOrigen);
                }
            }
}

void jnz(char** instruccion)
{
    if(instruccion[1][0] != 'E')
            {
                uint8_t* registro_a_chequear = string_to_register8(instruccion[1]);
                uint32_t instruccion_a_saltar = atoi(instruccion[2]);
                jnz_8(registro_a_chequear,instruccion_a_saltar);
            }
            else{
                uint32_t* registro_a_chequear = string_to_register32(instruccion[1]);
                uint32_t instruccion_a_saltar = atoi(instruccion[2]);
                jnz_32(registro_a_chequear,instruccion_a_saltar);
            }
}