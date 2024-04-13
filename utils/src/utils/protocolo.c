#include "../include/protocolo.h"
/*
void mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario,int32_t valorHandshake)
{
    int32_t result = 0;
    while(1)
    {   
        send(fd_destinatario,&valorHandshake,sizeof(int32_t),0);
        recv(fd_destinatario,&result,sizeof(int32_t),MSG_WAITALL);
        if(result==0)
        {
            log_info(logger,"HANDSHAKE ACEPTADO DE: %s!",nombreDestinatario);
        }else{
            log_error(logger,"HANDSHAKE DENEGADO DE: %s!",nombreDestinatario);
        }
        break;
    }
    close(fd_destinatario);
}*/
void mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario)
{
    int32_t result = 0;
    cod_op handshake = HANDSHAKE;
    int valorHandshake = handshake;
    while(1)
    {   
        send(fd_destinatario,&valorHandshake,sizeof(int32_t),0);
        recv(fd_destinatario,&result,sizeof(int32_t),MSG_WAITALL);
        if(result==0)
        {
            log_info(logger,"HANDSHAKE ACEPTADO DE: %s!",nombreDestinatario);
        }else{
            log_error(logger,"HANDSHAKE DENEGADO DE: %s!",nombreDestinatario);
        }
        break;
    }
    close(fd_destinatario);
}
/*
void recibirHandshake(t_log* logger,int fd_origen, char* nombreOrigen,int32_t valorHandshake)
{
    int32_t resultOk = 0;
    int32_t resultError = -1;
    while(1)
    {
        recv(fd_origen, &valorHandshake, sizeof(int32_t), MSG_WAITALL);
        if (valorHandshake == 1) {
            log_info(logger,"HANDSHAKE ACEPTADO A: %s!", nombreOrigen);
            send(fd_origen, &resultOk, sizeof(int32_t), 0);
        } else {
            log_error(logger,"HANDSHAKE DENEGADO A: %s!", nombreOrigen);
            send(fd_origen, &resultError, sizeof(int32_t), 0);
        }
        break;
    }
    close(fd_origen);
}*/

void recibir_handshake(t_log* logger,int fd_origen, char* nombreOrigen)
{
    int32_t resultOk = 0;
    int32_t resultError = -1;
    //if (valorHandshake == 1) {
    log_info(logger,"HANDSHAKE ACEPTADO A: %s!", nombreOrigen);
    send(fd_origen, &resultOk, sizeof(int32_t), 0);
    /*} else {
        log_error(logger,"HANDSHAKE DENEGADO A: %s!", nombreOrigen);
        send(fd_origen, &resultError, sizeof(int32_t), 0);
    }*/
    close(fd_origen);
}

//SERIALIZACION
//EN size si se trabaja con un char* pasarle un caracter extra \0
t_buffer *buffer_create(uint32_t size){
    t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->size = size;    
    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);
    return buffer;
}
void buffer_destroy(t_buffer *buffer){
    free(buffer->stream);
    free(buffer);
}
/*
void buffer_add(t_buffer *buffer, void* data, uint32_t size){

}
*/
// Agrega un uint8_t al buffer
void buffer_add_uint32(t_buffer *buffer, uint32_t data){
    void* stream = buffer->stream;
    int offset = buffer->offset;
    memcpy(stream + offset, &data, sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);
}
// Agrega un uint8_t al buffer
void buffer_add_uint8(t_buffer *buffer, uint8_t data){
    void* stream = buffer->stream;
    int offset = buffer->offset;
    memcpy(stream + offset, &data, sizeof(uint8_t));
    buffer->offset += sizeof(uint8_t);
}
// Agrega string al buffer con un uint32_t adelante indicando su longitud
void buffer_add_string(t_buffer *buffer, uint32_t length, char* string)
{
    void* stream = buffer->stream;
    int offset = buffer->offset;
    memcpy(stream + offset, &length, sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);
    memcpy(stream + offset, string, length);
    buffer->offset += length;
}
// Lee un uint32_t del buffer y avanza el offset
uint32_t buffer_read_uint32(t_buffer *buffer){
    void* stream = buffer->stream;
    uint32_t dataRecibida;
    memcpy(&dataRecibida, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    return dataRecibida;
}
// Lee un uint8_t del buffer y avanza el offset
uint8_t buffer_read_uint8(t_buffer *buffer){
    void* stream = buffer->stream;
    uint8_t dataRecibida;
    memcpy(&dataRecibida, stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    return dataRecibida;
}
// Lee un string y su longitud del buffer y avanza el offset
/*
char* buffer_read_string(t_buffer *buffer, uint32_t length){
    void* stream = buffer->stream;
    memcpy(&length, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    char* stringRecibido = malloc(sizeof(length)); //falta free
    memcpy(stringRecibido, stream, length);
    stream += sizeof(length);
    return stringRecibido;
}*/
