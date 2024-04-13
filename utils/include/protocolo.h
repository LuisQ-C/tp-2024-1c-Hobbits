#ifndef UTILS_PROTOCOLO_H
#define UTILS_PROTOCOLO_H

#include <stdio.h>
#include <stdlib.h>
#include "../include/sockets.h"


typedef enum{
    HANDSHAKE,
    PERSONA,
    EJECUTAR_SCRIPT_OPERACIONES,
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    INICIAR_PLANIFICACION,
    DETENER_PLANIFICACION,
    LISTAR_PROCESOS_POR_ESTADO
}cod_op;
typedef struct {
    uint32_t size; // Tamaño del payload
    uint32_t offset; // Desplazamiento dentro del payload
    void* stream; // Payload
} t_buffer;

typedef struct {
    uint32_t dni;
    uint8_t edad;
    uint32_t pasaporte;
    uint32_t nombre_length;
    char* nombre;
} t_persona;
typedef struct {
    uint8_t codigo_operacion;
    t_buffer* buffer;
} t_paquete;

//void mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario,int32_t valorHandshake);
//void recibirHandshake(t_log* logger,int fd_origen, char* nombreOrigen,int32_t valorHandshake);
void mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario);
void recibir_handshake(t_log* logger,int fd_origen, char* nombreOrigen);

//BUFFER, SERIALIZACION
t_buffer *buffer_create(uint32_t size);
void buffer_destroy(t_buffer *buffer);
void buffer_add_uint32(t_buffer *buffer, uint32_t data);
void buffer_add_uint8(t_buffer *buffer, uint8_t data);
void buffer_add_string(t_buffer *buffer, uint32_t length, char* string);
uint32_t buffer_read_uint32(t_buffer *buffer);
uint8_t buffer_read_uint8(t_buffer *buffer);
char *buffer_read_string(t_buffer *buffer, uint32_t length);

#endif