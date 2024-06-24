#include "../include/instrucciones.h"

extern t_registro_cpu registro;
extern t_log* logger;
extern t_config* config;
extern config_memoria config_mem;

// INSTRUCCIONES 
// SET
/* Setea el valor en el registro*/
void set_8(uint8_t* reg,uint8_t valor)
{
    *reg = valor;
}

/* Setea el valor en el registro*/
void set_32(uint32_t* reg,uint32_t valor)
{
    *reg = valor;
}

/* Ejecuta instruccion SET */
int set(char* registro_setear, int numero_setear)
{
    if(strcmp(registro_setear,"PC")==0)
    {
        uint32_t* registroRecibido = string_to_register32(registro_setear);
        set_32(registroRecibido,numero_setear);
        return PC_MODIFICADO;
    }
    else if(strcmp(registro_setear,"DI")==0 || strcmp(registro_setear,"SI")==0)
    {
        uint32_t* registroRecibido = string_to_register32(registro_setear);
        set_32(registroRecibido,numero_setear);
        return PC_SIN_MODIFICAR;
    }
    else if(registro_setear[0] != 'E')
    {
        uint8_t* registroRecibido = string_to_register8(registro_setear);
        set_8(registroRecibido,numero_setear);
        return PC_SIN_MODIFICAR;
    }
    else {
        uint32_t* registroRecibido = string_to_register32(registro_setear);
        set_32(registroRecibido,numero_setear);
        return PC_SIN_MODIFICAR;
    }
}

// SUM
/* Suma los valores de los registros pasados, guardando el resultado en el primero */
void sum_8_8(uint8_t* registroDestino,uint8_t* registroOrigen)
{
    //uint8_t numero_a_sumar = *registroOrigen;
    *registroDestino += *registroOrigen;
}

/* Suma los valores de los registros pasados, guardando el resultado en el primero */
void sum_8_32(uint8_t* registroDestino,uint32_t* registroOrigen)
{
    *registroDestino += *registroOrigen;
}

/* Suma los valores de los registros pasados, guardando el resultado en el primero */
void sum_32_32(uint32_t* registroDestino,uint32_t* registroOrigen)
{
    *registroDestino += *registroOrigen;
}

/* Suma los valores de los registros pasados, guardando el resultado en el primero */
void sum_32_8(uint32_t* registroDestino,uint8_t* registroOrigen)
{
    *registroDestino += *registroOrigen;
}

/* Ejecuta instruccion SUM */
void sum(char** instruccion)
{

    if(instruccion[1][0] != 'E' && strcmp(instruccion[1],"DI") != 0 &&  strcmp(instruccion[1],"SI") != 0)
    {
        if(instruccion[2][0] != 'E' && strcmp(instruccion[2],"DI") != 0 &&  strcmp(instruccion[2],"SI") != 0)
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
        if(instruccion[2][0] != 'E' && strcmp(instruccion[2],"DI") != 0 &&  strcmp(instruccion[2],"SI") != 0)
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

// SUB
/* Resta los valores de los registros pasados, guardando el resultado en el primero */
void sub_8_8(uint8_t* registroDestino,uint8_t* registroOrigen) 
{
    *registroDestino -= *registroOrigen;
}

/* Resta los valores de los registros pasados, guardando el resultado en el primero */
void sub_8_32(uint8_t* registroDestino,uint32_t* registroOrigen) 
{
    *registroDestino -= *registroOrigen;
}

/* Resta los valores de los registros pasados, guardando el resultado en el primero */
void sub_32_32(uint32_t* registroDestino,uint32_t* registroOrigen)
{
    *registroDestino -= *registroOrigen;
}

/* Resta los valores de los registros pasados, guardando el resultado en el primero */
void sub_32_8(uint32_t* registroDestino,uint8_t* registroOrigen)
{
    *registroDestino -= *registroOrigen;
}

/* Ejecuta instruccion SUB */
void sub(char** instruccion)
{
    if(instruccion[1][0] != 'E'  && strcmp(instruccion[1],"DI") != 0 &&  strcmp(instruccion[1],"SI") != 0)
    {
        if(instruccion[2][0] != 'E' && strcmp(instruccion[2],"DI") != 0 &&  strcmp(instruccion[2],"SI") != 0)
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
        if(instruccion[2][0] != 'E' && strcmp(instruccion[2],"DI") != 0 &&  strcmp(instruccion[2],"SI") != 0)
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

// JNZ
/* Compara si el registro pasado es igual a 0, de ser asi el PC salta a la instruccion pasada */
void jnz_8(uint8_t* reg,uint32_t instruccion_proxima){
    if(*reg != 0)
        registro.PC = instruccion_proxima;
    else
        registro.PC++;
}

/* Compara si el registro pasado es igual a 0, de ser asi el PC salta a la instruccion pasada */
void jnz_32(uint32_t* reg,uint32_t instruccion_proxima){
    if(*reg != 0)
        registro.PC = instruccion_proxima;
    else
        registro.PC++;
}

/* Ejecuta instruccion JNZ */
void jnz(char** instruccion)
{
    if(instruccion[1][0] != 'E' && strcmp(instruccion[1],"DI") != 0 &&  strcmp(instruccion[1],"SI") != 0)
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

// IO_GEN_SLEEP
/* Enviar al kernel la solicitud de que duerma a un entrada salida */
void io_gen_sleep(t_pcb* pcb_a_enviar,char** instruccionDesarmada,int fd_dispatch){

    int motivo_desalojo = IO_GEN_SLEEP;

    t_paquete* paquete = armar_paquete_pcb(pcb_a_enviar);

    agregar_a_paquete(paquete,&motivo_desalojo,sizeof(int));
    agregar_a_paquete(paquete,instruccionDesarmada[1],strlen(instruccionDesarmada[1])+1);

    int tiempo_sleep = atoi(instruccionDesarmada[2]);

    agregar_a_paquete(paquete,&tiempo_sleep,sizeof(int));

    enviar_paquete(paquete,fd_dispatch);
    eliminar_paquete(paquete);
}

//STDIN Y STDOUT
void io_stdin_stdout(t_pcb* pcb_a_enviar,char* nombre_interfaz,int direccion_logica,int tamanio_dato,int fd_dispatch, int fd_memoria, int motivo)
{
    int motivo_desalojo = motivo;
    int tam_pagina = config_mem.tam_pagina;
    int tamanio_enviar;
    //
    int pagina_inicial = traducir_direccion_pagina(direccion_logica);
    int offset = traducir_direccion_desplazamiento(direccion_logica,pagina_inicial);
    int pag_necesarias = paginas_necesarias(offset,tamanio_dato);
    //BASE, DIR FISICA Y TAMANIO
    int base = 0;
    int dir_fisica;
    int restante = tamanio_dato;
    int espacio_restante = tam_pagina - offset;
    //PORCION EMPAQUETAR
    t_porcion_dato porcion_empaquetar;
    //EMPAQUETO PCB PRIMERO
    t_paquete* paquete = armar_paquete_pcb(pcb_a_enviar);
    //LUEGO EL MOTIVO
    agregar_a_paquete(paquete,&motivo_desalojo,sizeof(int));
    //LUEGO EL NOMBRE DE LA INTERFAZ
    agregar_a_paquete(paquete,nombre_interfaz,strlen(nombre_interfaz)+1);
    //SOLICITO LOS MARCOS
    t_list* marcos = solicitar_macros(pagina_inicial,pag_necesarias,pcb_a_enviar->pid,fd_memoria);
    //EMPIEZO A CALCULAR Y EMPAQUETAR LOS T_PORCION_DATO
    int* ptro_nro_frame = list_get(marcos,0);
    dir_fisica = calcular_direccion_fisica(*ptro_nro_frame,offset);
    
    tamanio_enviar = (espacio_restante > tamanio_dato) ? tamanio_dato : espacio_restante;

    //asignar_porcion_dato(&porcion_empaquetar,base,dir_fisica,tamanio_enviar);
    porcion_empaquetar.base = base;
    porcion_empaquetar.direccion_fisica = dir_fisica;
    porcion_empaquetar.tamanio = tamanio_enviar;

    //printf("BASE: %d TAMANIO: %d DIR.FIS: %d",porcion_empaquetar.base,porcion_empaquetar.);

    agregar_a_paquete(paquete,&porcion_empaquetar,sizeof(t_porcion_dato));

    avanzar_base_restante(&base,&restante,espacio_restante);

    for(int i =1;i<pag_necesarias;i++)
    {
        ptro_nro_frame = list_get(marcos,i);
        dir_fisica = calcular_direccion_fisica(*ptro_nro_frame,0);

        tamanio_enviar = (restante<tam_pagina) ? restante : tam_pagina;

        //asignar_porcion_dato(&porcion_empaquetar,base,dir_fisica,tamanio_enviar);

        porcion_empaquetar.base = base;
        porcion_empaquetar.direccion_fisica = dir_fisica;
        porcion_empaquetar.tamanio = tamanio_enviar;

        agregar_a_paquete(paquete,&porcion_empaquetar,sizeof(t_porcion_dato));

        avanzar_base_restante(&base,&restante,tam_pagina);

    }

    enviar_paquete(paquete,fd_dispatch);

    list_destroy_and_destroy_elements(marcos,(void*) liberar_elemento);
    
    eliminar_paquete(paquete);
}

//IO FILESYSTEM
void io_fs_write_read(t_pcb* pcb_a_enviar,char* nombre_interfaz,char* nombre_archivo, int direccion_logica,int tamanio_dato,int puntero_archivo,int fd_dispatch, int fd_memoria,int motivo)
{
    int motivo_desalojo = motivo;
    
    //EMPAQUETO PCB PRIMERO
    t_paquete* paquete = armar_paquete_pcb(pcb_a_enviar);
    //LUEGO EL MOTIVO
    agregar_a_paquete(paquete,&motivo_desalojo,sizeof(int));
    //LUEGO EL NOMBRE DE LA INTERFAZ
    agregar_a_paquete(paquete,nombre_interfaz,strlen(nombre_interfaz)+1);
    agregar_a_paquete(paquete,nombre_archivo,strlen(nombre_archivo)+1);
    agregar_a_paquete(paquete,&puntero_archivo,sizeof(int));
    //SOLICITO LOS MARCOS
    aniadir_porciones_dato(paquete,pcb_a_enviar->pid,direccion_logica,tamanio_dato,fd_memoria);

    enviar_paquete(paquete,fd_dispatch);
    
    eliminar_paquete(paquete);
}

void aniadir_porciones_dato (t_paquete* paquete,int pid ,int direccion_logica, int tamanio_dato, int fd_memoria)
{
    int tam_pagina = config_mem.tam_pagina;
    int tamanio_enviar;
    //
    int pagina_inicial = traducir_direccion_pagina(direccion_logica);
    int offset = traducir_direccion_desplazamiento(direccion_logica,pagina_inicial);
    int pag_necesarias = paginas_necesarias(offset,tamanio_dato);
    //BASE, DIR FISICA Y TAMANIO
    int base = 0;
    int dir_fisica;
    int restante = tamanio_dato;
    int espacio_restante = tam_pagina - offset;
    //PORCION EMPAQUETAR
    t_porcion_dato porcion_empaquetar;
    //EMPIEZO A CALCULAR Y EMPAQUETAR LOS T_PORCION_DATO
    t_list* marcos = solicitar_macros(pagina_inicial,pag_necesarias,pid,fd_memoria);

    int* ptro_nro_frame = list_get(marcos,0);
    dir_fisica = calcular_direccion_fisica(*ptro_nro_frame,offset);
    
    tamanio_enviar = (espacio_restante > tamanio_dato) ? tamanio_dato : espacio_restante;

    asignar_porcion_dato(&porcion_empaquetar,base,dir_fisica,tamanio_enviar);

    agregar_a_paquete(paquete,&porcion_empaquetar,sizeof(t_porcion_dato));

    avanzar_base_restante(&base,&restante,espacio_restante);

    for(int i =1;i<pag_necesarias;i++)
    {
        ptro_nro_frame = list_get(marcos,i);
        dir_fisica = calcular_direccion_fisica(*ptro_nro_frame,0);

        tamanio_enviar = (restante<tam_pagina) ? restante : tam_pagina;

        asignar_porcion_dato(&porcion_empaquetar,base,dir_fisica,tamanio_enviar);

        agregar_a_paquete(paquete,&porcion_empaquetar,sizeof(t_porcion_dato));

        avanzar_base_restante(&base,&restante,tam_pagina);

    }

    list_destroy_and_destroy_elements(marcos,(void*) liberar_elemento);
}

void io_fs_create(t_pcb* pcb_a_enviar, char* nombre_interfaz, char* nombre_archivo, int fd_dispatch)
{
    int motivo_desalojo = IO_FS_CREATE;

    t_paquete* paquete = armar_paquete_pcb(pcb_a_enviar);

    agregar_a_paquete(paquete,&motivo_desalojo,sizeof(int));
    agregar_a_paquete(paquete,nombre_interfaz,strlen(nombre_interfaz)+1);
    agregar_a_paquete(paquete,nombre_archivo,strlen(nombre_archivo)+1);

    enviar_paquete(paquete,fd_dispatch);
    eliminar_paquete(paquete);
}
void io_fs_delete(t_pcb* pcb_a_enviar, char* nombre_interfaz, char* nombre_archivo, int fd_dispatch)
{
    int motivo_desalojo = IO_FS_DELETE;

    t_paquete* paquete = armar_paquete_pcb(pcb_a_enviar);

    agregar_a_paquete(paquete,&motivo_desalojo,sizeof(int));
    agregar_a_paquete(paquete,nombre_interfaz,strlen(nombre_interfaz)+1);
    agregar_a_paquete(paquete,nombre_archivo,strlen(nombre_archivo)+1);

    enviar_paquete(paquete,fd_dispatch);
    eliminar_paquete(paquete);
}
void io_fs_truncate(t_pcb* pcb_a_enviar, char* nombre_interfaz, char* nombre_archivo, int tamanio_archivo,int fd_dispatch)
{
    int motivo_desalojo = IO_FS_TRUNCATE;
    int tamanio = tamanio_archivo;

    t_paquete* paquete = armar_paquete_pcb(pcb_a_enviar);

    agregar_a_paquete(paquete,&motivo_desalojo,sizeof(int));
    agregar_a_paquete(paquete,nombre_interfaz,strlen(nombre_interfaz)+1);
    agregar_a_paquete(paquete,nombre_archivo,strlen(nombre_archivo)+1);
    agregar_a_paquete(paquete,&tamanio,sizeof(int));

    enviar_paquete(paquete,fd_dispatch);
    eliminar_paquete(paquete);
}

void asignar_porcion_dato(t_porcion_dato* porcion, int base, int dir_fisica, int tamanio)
{
    /**(porcion->base) = base;
    *(porcion->tamanio) = tamanio;
    *(porcion->direccion_fisica) = dir_fisica;*/
    /*porcion->base = base;
    porcion->direccion_fisica = dir_fisica;
    porcion->tamanio = tamanio;*/
}

int resize(int pid,int new_size,int fd_memoria)
{
    t_paquete* paquete = crear_paquete(RESIZE);
    agregar_a_paquete(paquete,&pid,sizeof(int));
    agregar_a_paquete(paquete,&new_size,sizeof(int));
    enviar_paquete(paquete,fd_memoria);
    eliminar_paquete(paquete);

    int respuesta;
    recv(fd_memoria,&respuesta,sizeof(int),MSG_WAITALL);
    return respuesta;
}

void instruccion_signal(t_pcb* pcb_a_enviar,char* nombre_recurso, int fd_dispatch)
{
    int motivo_desalojo = SIGNAL;

    t_paquete* paquete = armar_paquete_pcb(pcb_a_enviar);

    agregar_a_paquete(paquete,&motivo_desalojo,sizeof(int));
    agregar_a_paquete(paquete,nombre_recurso,strlen(nombre_recurso)+1);

    enviar_paquete(paquete,fd_dispatch);
    eliminar_paquete(paquete);
}

void instruccion_wait(t_pcb* pcb_a_enviar, char* nombre_recurso, int fd_dispatch)
{
    int motivo_desalojo = WAIT;

    t_paquete* paquete = armar_paquete_pcb(pcb_a_enviar);

    agregar_a_paquete(paquete,&motivo_desalojo,sizeof(int));
    agregar_a_paquete(paquete,nombre_recurso,strlen(nombre_recurso)+1);

    enviar_paquete(paquete,fd_dispatch);
    eliminar_paquete(paquete);
}


void mov_out(int pagina,int desplazamiento, uint32_t* dato_a_escribir,int fd_memoria)
{
    t_paquete* paquete = crear_paquete(ESCRITURA);
    agregar_a_paquete(paquete,&pagina,sizeof(pagina));
    agregar_a_paquete(paquete,&desplazamiento,sizeof(desplazamiento));
    agregar_a_paquete(paquete,dato_a_escribir,sizeof(dato_a_escribir));
    enviar_paquete(paquete,fd_memoria);
    printf("\n mov_out enviado \n");
    eliminar_paquete(paquete);

}

