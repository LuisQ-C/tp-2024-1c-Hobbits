#ifndef UTILS_PROTOCOLO_H
#define UTILS_PROTOCOLO_H

#include <stdio.h>
#include <stdlib.h>
#include "../include/sockets.h"


typedef enum{
    HANDSHAKE,
    HOLA
}cod_op;

//void mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario,int32_t valorHandshake);
//void recibirHandshake(t_log* logger,int fd_origen, char* nombreOrigen,int32_t valorHandshake);
void mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario);
void recibir_handshake(t_log* logger,int fd_origen, char* nombreOrigen);


#endif