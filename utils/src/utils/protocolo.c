#include "../include/protocolo.h"

void mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario, char* nombreOrigen)
{
    int bytes;
    int32_t result = 3;
    /*cod_op handshake = HANDSHAKE;
    int valorHandshake = handshake;*/
    while(1)
    {   
        enviar_mensaje(nombreOrigen,fd_destinatario,HANDSHAKE);
        /*if(bytes == -1)
        {
            log_error(logger,"Error al enviar el handshake (funcion send)");
            perror("\nError en send de mandarHandshake");
            exit(1);
        }*/
        bytes = recv(fd_destinatario,&result,sizeof(int32_t),MSG_WAITALL);
        if(bytes == -1)
        {
            log_error(logger,"Error al enviar el handshake (funcion recv)");
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
void enviar_handshake_ok(t_log* logger,int fd_origen, char* nombreOrigen)
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
    int bytes;
	if((bytes = recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL)) > 0)
    {
		return cod_op;
    }
	else if(bytes == 0)
	{
		close(socket_cliente);
		return DESCONEXION;
	}
    else
	{
		close(socket_cliente);
		return ERROR;
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

//FUNCIONES NUEVAS TP0, ARMAR PAQUETE
t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

t_paquete* crear_paquete(cod_op codigo_op)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_op;
	crear_buffer(paquete);
	return paquete;
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}


void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

/*Eliminar el paquete creado luego de enviarlo*/
void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

/*Eliminar paquete del lado del cliente*/
void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

// FUNCIONES DE PCB 

/* Liberar estado */


//Prueba enviar pcb - Luis
t_pcb* recibir_pcb(int fd_dispatch)
{
    t_list* lista;
    
    t_pcb* pcb_recibido = malloc(sizeof(t_pcb));
    //pcb_recibido->registros_CPU ;= malloc(sizeof(t_registros_generales));



    lista = recibir_paquete(fd_dispatch);

    int* pid = list_get(lista, 0);
    uint32_t* pc = list_get(lista, 1);
    int* quantum = list_get(lista, 2);
    uint32_t* estado = list_get(lista, 3);
    t_registros_generales* registros_recibidos = list_get(lista, 4);

    pcb_recibido->pid = *pid;
    pcb_recibido->pc = *pc;
    pcb_recibido->quantum = *quantum;
    pcb_recibido->estado = *estado;
    pcb_recibido->registros_CPU = *registros_recibidos;

    free(pid);
    free(pc);
    free(quantum);
    free(estado);
    free(registros_recibidos);

    list_destroy(lista);
    
    return pcb_recibido;
}



void enviar_pcb(t_pcb* pcb_a_enviar, int fd_dispatch){
    t_paquete* paquete = crear_paquete(PCB);
    
    agregar_a_paquete(paquete, &pcb_a_enviar->pid, sizeof(int));
    agregar_a_paquete(paquete, &pcb_a_enviar->pc, sizeof(uint32_t));
    agregar_a_paquete(paquete, &pcb_a_enviar->quantum, sizeof(int));
    agregar_a_paquete(paquete, &pcb_a_enviar->estado, sizeof(uint32_t));
    agregar_a_paquete(paquete, &pcb_a_enviar->registros_CPU, sizeof(t_registros_generales));
    
    enviar_paquete(paquete, fd_dispatch);
    
    eliminar_paquete(paquete);
}

void enviar_nuevo_proceso(int* pid, char* nombre_archivo, int fd_memoria){
    t_paquete* paquete = crear_paquete(INICIAR_PROCESO);

    agregar_a_paquete(paquete, pid, sizeof(int));
    printf("\nENVIAR NUEVO PROCESO PID: %d\n",*pid);
    printf("\nENVIAR NUEVO PROCESO NOMBRE ARCHIVO: %s\n",nombre_archivo);
    agregar_a_paquete(paquete, nombre_archivo, strlen(nombre_archivo)+1);
    int tamanio = strlen(nombre_archivo);
    printf("\nLONGITUD NOMBRE ARCHIVO: %d\n",tamanio);
    enviar_paquete(paquete, fd_memoria);

    eliminar_paquete(paquete);
}

void recibir_nuevo_proceso(int fd_kernel){
    t_list* lista;
    lista = recibir_paquete(fd_kernel);
    int* pid = list_get(lista, 0);
    char* nombre_archivo = list_get(lista, 1);

    free(pid);
    free(nombre_archivo);
    
    list_destroy(lista);

}

t_paquete* armar_paquete_pcb(t_pcb* pcb_a_enviar)
{
    t_paquete* paquete = crear_paquete(PCB);
    agregar_a_paquete(paquete,&pcb_a_enviar->pid,sizeof(int));
    agregar_a_paquete(paquete,&pcb_a_enviar->pc,sizeof(uint32_t));
    agregar_a_paquete(paquete,&pcb_a_enviar->estado,sizeof(uint32_t));
    agregar_a_paquete(paquete,&pcb_a_enviar->quantum,sizeof(int));
    agregar_a_paquete(paquete,&pcb_a_enviar->registros_CPU,sizeof(t_registros_generales));
    return paquete;
}

void liberar_elemento(void* self)
{
    free(self);
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
