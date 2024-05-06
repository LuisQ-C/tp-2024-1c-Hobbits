#include "../include/cicloInstruccion.h"

extern t_registro_cpu registro;
extern t_log* logger;
extern int HAY_INTERRUPCION;
extern int PID_ACTUAL;
extern int MOTIVO_DESALOJO;
extern pthread_mutex_t mutex_interrupcion;
extern pthread_mutex_t mutex_pid;
extern sem_t semaforo_pcb_recibido;

void realizarCicloInstruccion(int fd_conexion_memoria, t_pcb* pcb_recibido,int cliente_fd_conexion_dispatch)
{
    resetear_var_globales();
    establecer_contexto(pcb_recibido);                                                //CAMBIAR PID_ACTUAL POR EL DEL PCB RECIBIDO

    while(1){
    
    t_instruccion instruccion = fetch(registro.PC,fd_conexion_memoria,pcb_recibido->pid); //FETCH (SOLICITA Y RECIBE LA INSTRUCCION)
    
    decode_and_execute(instruccion,pcb_recibido,cliente_fd_conexion_dispatch);         //DECODIFICA LA INSTRUCCION Y LA EJECUTA 

    actualizar_pcb(pcb_recibido);                                                      //ACTUALIZAR PCB
    
    if(fue_desalojado())                                                               //CHEQUEA SI HUBO DESALOJO POR IO, EXIT, ETC
    {
        log_debug(logger,"fue desalojado");
        break;
    }
    
    if(check_interrupt(pcb_recibido,cliente_fd_conexion_dispatch))                     //CHEQUEA SI EN EL HILO DE INTERRUPCION LE LLEGO UNA INTERRUPCION
    {
        log_debug(logger,"fue interrumpido");
        break;
    }
    
    }
}

void resetear_var_globales()
{
    MOTIVO_DESALOJO = -1;
    pthread_mutex_lock(&mutex_pid);
    PID_ACTUAL = -1;
    pthread_mutex_unlock(&mutex_pid);
    pthread_mutex_lock(&mutex_interrupcion);
    HAY_INTERRUPCION = 0;
    pthread_mutex_unlock(&mutex_interrupcion);
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
    pthread_mutex_lock(&mutex_pid);
    PID_ACTUAL = pcb_recibido->pid;
    pthread_mutex_unlock(&mutex_pid);
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
t_instruccion fetch(uint32_t pc, int fd_conexion_memoria,int pid)
{
    pedir_instruccion(pc,pid,fd_conexion_memoria);
    t_instruccion instruccion = recibirInstruccion(fd_conexion_memoria);
    log_info(logger,"Fetch Instruccion: \"PID: %d - FETCH - Program Counter: %u\".",pid,pc);
    return instruccion;
}

/*Crea un paquete con pc y pid para mandarselo a memoria*/
void pedir_instruccion(uint32_t pc, int pid,int fd_conexion_memoria)
{
    t_paquete* paquete = crear_paquete(INSTRUCCION);
    agregar_a_paquete(paquete,&pid,sizeof(int));
    agregar_a_paquete(paquete,&pc,sizeof(uint32_t));
    enviar_paquete(paquete,fd_conexion_memoria);
    eliminar_paquete(paquete);
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
    //log_info(logger,"Valor de CODOP: %d",cod_op);
    char* instruccionRecibida = recibir_mensaje(fd_conexion_memoria,logger);
    string_append(&instruccion,instruccionRecibida);
    free(instruccionRecibida);
    return instruccion;
}

/* Decodifica y ejecuta la instruccion pasada por parametro*/
void decode_and_execute(t_instruccion instruccion,t_pcb* pcb_a_enviar,int fd_dispatch)
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
        {
            MOTIVO_DESALOJO = IO_GEN_SLEEP;
            io_gen_sleep(pcb_a_enviar,instruccionDesarmada,fd_dispatch);
            registro.PC++;
            break;
        }
        case EXIT:
        {
            MOTIVO_DESALOJO = EXIT;
            instruccion_exit(pcb_a_enviar,fd_dispatch);
            break;
        }
        case -1:
            log_warning(logger,"Instruccion invalida");
        default:
            log_error(logger,"Error al decodificar instruccion");
    }
    //hola
    logear_instruccion_ejecutada(pcb_a_enviar->pid,instruccion);
    string_array_destroy(instruccionDesarmada);
    free(instruccion);
    
}

void logear_instruccion_ejecutada(int pid,char* instruccion)
{
    char** instruccionLogear = string_n_split(instruccion,1," ");
    if(instruccionLogear[1]!=NULL)
    {
        log_info(logger,"Instruccion Ejecutada: \"PID: %d - Ejecutando: %s - %s\".",pid,instruccionLogear[0],instruccionLogear[1]);
    }
    else{
        log_info(logger,"Instruccion Ejecutada: \"PID: %d - Ejecutando: %s\".",pid,instruccionLogear[0]);
    }
    string_array_destroy(instruccionLogear);
}

int fue_desalojado()
{
    int desalojado = 0;
    if(MOTIVO_DESALOJO != -1)
    {
        pthread_mutex_lock(&mutex_interrupcion);
        HAY_INTERRUPCION = 0;
        pthread_mutex_unlock(&mutex_interrupcion);
        desalojado = 1;
    }
    return desalojado;
}

int check_interrupt(t_pcb* pcb_a_chequear,int fd_dispatch)
{
    int ocurrio_interrupcion = 0;
    pthread_mutex_lock(&mutex_interrupcion);

    if(HAY_INTERRUPCION)
    {
        enviar_pcb(pcb_a_chequear,fd_dispatch); //ACA SE DEBERIA DEVOLVER CONTEXTO CON MOTIVO "INTERRUPCION"
        HAY_INTERRUPCION = 0;
        ocurrio_interrupcion = 1;
    }

    pthread_mutex_unlock(&mutex_interrupcion);

    return ocurrio_interrupcion;
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