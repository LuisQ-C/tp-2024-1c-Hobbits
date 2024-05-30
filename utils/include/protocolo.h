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
    IO_FS,
    INTERRUPCION_QUANTUM,
    USER_INTERRUPT,
    EXIT
}cod_op;

enum{
    DESCONEXION = 150,
    ERROR,
    ARCHIVO_EXISTE,
    ARCHIVO_INVALIDO,
    INTERFAZ_LISTA,
    MARCO_INVALIDO
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
void mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario, char* nombreOrigen);
void enviar_handshake_ok(t_log* logger,int fd_origen, char* nombreOrigen);


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


//BUFFER, SERIALIZACION
/*
t_buffer *buffer_create(uint32_t size);
void buffer_destroy(t_buffer *buffer);
void buffer_add_uint32(t_buffer *buffer, uint32_t data);
void buffer_add_uint8(t_buffer *buffer, uint8_t data);
void buffer_add_string(t_buffer *buffer, uint32_t length, char* string);
uint32_t buffer_read_uint32(t_buffer *buffer);
uint8_t buffer_read_uint8(t_buffer *buffer);
char *buffer_read_string(t_buffer *buffer, uint32_t length);
*/
#endif