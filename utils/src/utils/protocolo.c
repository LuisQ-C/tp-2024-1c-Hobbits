#include "../include/protocolo.h"

void mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario)
{
    int bytes;
    int32_t result = 0;
    cod_op handshake = HANDSHAKE;
    int valorHandshake = handshake;
    while(1)
    {   
        bytes =send(fd_destinatario,&valorHandshake,sizeof(int32_t),0);
        if(bytes == -1)
        {
            log_error(logger,"Error al enviar el handshake (funcion send)");
            perror("\nError en send de mandarHandshake");
            exit(1);
        }
        bytes = recv(fd_destinatario,&result,sizeof(int32_t),MSG_WAITALL);
        if(bytes == -1)
        {
            log_error(logger,"Error al enviar el handshake (funcion send)");
            perror("\nError en recv de mandarHandshake");
            exit(1);
        }
        if(result==0)
        {
            log_info(logger,"HANDSHAKE ACEPTADO DE: %s!",nombreDestinatario);
        }else{
            log_error(logger,"HANDSHAKE DENEGADO DE: %s!",nombreDestinatario);
        }
        break;
    }
    
}
void recibir_handshake(t_log* logger,int fd_origen, char* nombreOrigen)
{
    int bytes;
    int32_t resultOk = 0;
    //int32_t resultError = -1;
    log_info(logger,"HANDSHAKE ACEPTADO A: %s!", nombreOrigen);
    bytes = send(fd_origen, &resultOk, sizeof(int32_t), 0);
    if(bytes == -1)
        {
            log_error(logger,"Error al enviar el resultado del handshake (funcion send)");
            perror("\nError en send de recibir_handshake");
            exit(1);
        }
}

//SERIALIZACION DEL TP0
int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
    {
        //printf("COP: %d",cod_op);
		return cod_op;}
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}


/*Esta funcion requiere un FREE despues de utilizarla*/
char* recibir_mensaje(int socket_cliente,t_log* logger)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	//log_info(logger, "Me llego el mensaje: %s", buffer);
    return buffer;
	//free(buffer);
}

void enviar_mensaje(char* mensaje, int socket_cliente, cod_op operacion)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = operacion;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}
void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}
void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}










































/*
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

void buffer_add(t_buffer *buffer, void* data, uint32_t size){

}


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

char* buffer_read_string(t_buffer *buffer, uint32_t length){
    void* stream = buffer->stream;
    memcpy(&length, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    char* stringRecibido = malloc(sizeof(length)); //falta free
    memcpy(stringRecibido, stream, length);
    stream += sizeof(length);
    return stringRecibido;
}*/
