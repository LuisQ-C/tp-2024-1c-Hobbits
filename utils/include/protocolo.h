#ifndef UTILS_PROTOCOLO_H
#define UTILS_PROTOCOLO_H

#include <stdio.h>
#include <stdlib.h>
#include "../include/sockets.h"
#include "commons/string.h"

typedef enum{
    NEW = 100,
    READY,
    EXEC,
    BLOCKED,
    COLA_EXIT
}asd;

typedef struct{
    int base;
    int tamanio;
    int direccion_fisica;
}t_porcion_dato;

typedef enum{
    HANDSHAKE = 1,
    PERSONA,
    EJECUTAR_SCRIPT_OPERACIONES,
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    INICIAR_PLANIFICACION,
    DETENER_PLANIFICACION,
    LISTAR_PROCESOS_POR_ESTADO,
    PROCESO,
    INSTRUCCION,
    PCB,
    SET,
    SUM,
    SUB,
    JNZ,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    IO_FS,
    INTERRUPCION_QUANTUM,
    USER_INTERRUPT,
    WAIT,
    SIGNAL,
    EXIT,
    MOV_OUT,
    MOV_IN,
    ESCRITURA,
    LECTURA,
    RESIZE,
    ACCESO_TABLA_PAGINA,
    COPY_STRING,
    TLB_FIFO,
    TLB_LRU
}cod_op;

enum{
    DESCONEXION = 150,
    ERROR,
    ARCHIVO_EXISTE,
    ARCHIVO_INVALIDO,
    INTERFAZ_LISTA,
    MARCO_INVALIDO,
    DIRECCION_ERRONEA,
    OUT_OF_MEMORY,
    PROCESO_AMPLIADO,
    PROCESO_REDUCIDO,
    MODIFICACION_NULA,
    DATOS_CONFIG,
    HANDSHAKE_DENEGADO,
    HANDSHAKE_ACEPTADO,
    MARCOS_CONSULTADOS,
    PROCESO_INEXISTENTE,
    PROCESO_ELIMINADO
};
/*
typedef struct {
    uint32_t size; // Tamaño del payload
    uint32_t offset; // Desplazamiento dentro del payload
    void* stream; // Payload
} t_buffer;*/
typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
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
}t_registros_generales;

typedef struct
{
   int pid;
   uint32_t pc;
   int quantum;
   uint32_t estado;
   t_registros_generales registros_CPU;
} t_pcb;

typedef struct {
    int codigo_operacion;
    t_buffer* buffer;
} t_paquete;

//void mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario,int32_t valorHandshake);
//void recibirHandshake(t_log* logger,int fd_origen, char* nombreOrigen,int32_t valorHandshake);
int mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario, char* nombreOrigen);
//PARA ENVIARLE A CPU EL TAM_MEMORIA Y TAM_PAGINA NECESARIOS PARA LA MMU
void enviar_datos_config_memoria_a_cpu(int tam_memoria, int tam_pagina,int retardo_memoria, int fd_cpu);
void recibir_datos_config_memoria(int* tam_memoria, int* tam_pagina,int* retardo_memoria, int fd_memoria, t_log* logger);
//
//
void enviar_handshake_ok(t_log* logger,int fd_origen, char* nombreOrigen);
void enviar_handshake_error(t_log* logger,int fd_origen, char* nombreOrigen);


//SERIALIZACION TP0
int recibir_operacion(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);
char* recibir_mensaje(int socket_cliente,t_log* logger); //YA NO NECESITA EL LOGGER, POR AHORA
void enviar_mensaje(char* mensaje, int socket_cliente, cod_op operacion);
//void enviar_mensaje(char* mensaje, int socket_cliente);
void* serializar_paquete(t_paquete* paquete, int bytes);
t_list* recibir_paquete(int socket_cliente);
t_paquete* crear_paquete(cod_op codigo_op);
void crear_buffer(t_paquete* paquete);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);

//FUNCIONES PCB

t_pcb* recibir_pcb(int fd_dispatch);
void enviar_pcb(t_pcb* pcb_a_enviar,int fd_dispatch);
t_paquete* armar_paquete_pcb(t_pcb* pcb_a_enviar);
void liberar_elemento(void* self);
void enviar_nuevo_proceso(int* pid, char* nombre_archivo, int fd_memoria);
void recibir_nuevo_proceso(int fd_kernel);

// FUNCIONES PARA MANEJO DE INTERFACES

void enviar_solicitud_io_generico(int pid, int tiempo, int fd_interfaz);
void recibir_solicitud_io_generico(int* pid,int* tiempo, int fd_kernel);

// INTERRUPCIONES

void enviar_interrupcion(int interrupcion, int pid, int fd_interrupt);
int recibir_interrupcion(int* pid, int fd_interrupt);

//FUNCIONES ESCRITURA, LECTURA EN MEMORIA, SOLICITUD MACRO
void enviar_paquete_escritura(int pid,void* dato,int tamanio_dato,int base,int tamanio_fragmento,int direccion_fisica, int fd_destino);

void enviar_solicitud_macro(int pid, int pagina_solicitar,int fd_destino);

//t_list* solicitar_macros(int pagina_inicial, int paginas_totales,int pid, int fd_destinatario);

void enviar_paquete_lectura(int pid,int tamanio_fragmento,int direccion_fisica, int fd_destino);

void* recibir_dato_leido (int fd_destino, int tamanio_fragmento);

void* recibir_peticion_escritura(int* pid,int* base,int* tamanio,int* direccion_fisica,int fd_cpu);

void* recibir_peticion_lectura(int* pid, int* tamanio,int* direccion_fisica,int fd_cpu);

//RECIBIR CREACION PROCESO Y DESTRUCCION
void recibir_creacion_proceso(int* pid, char** path_kernel,int fd_kernel);
void recibir_destruccion_proceso(int* pid, int fd_kernel);

#endif