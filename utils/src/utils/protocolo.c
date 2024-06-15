#include "../include/protocolo.h"

int mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario, char* nombreOrigen)
{
    int bytes;
    int32_t result = 3;
    
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
    if(result==HANDSHAKE_ACEPTADO)
    {
        log_info(logger,"HANDSHAKE ACEPTADO DE: %s!",nombreDestinatario);
        return HANDSHAKE_ACEPTADO;
    }else{
        log_error(logger,"HANDSHAKE DENEGADO DE: %s!",nombreDestinatario);
        return HANDSHAKE_DENEGADO;
    }

}

void enviar_handshake_ok(t_log* logger,int fd_origen, char* nombreOrigen)
{
    int bytes;
    int32_t resultOk = HANDSHAKE_ACEPTADO;
    log_info(logger,"HANDSHAKE ACEPTADO A: %s!", nombreOrigen);
    bytes = send(fd_origen, &resultOk, sizeof(int32_t), 0);
    if(bytes == -1)
        {
            log_error(logger,"Error al enviar el resultado del handshake (funcion send)");
            perror("\nError en send de recibir_handshake");
            exit(1);
        }
}
void enviar_handshake_error(t_log* logger,int fd_origen, char* nombreOrigen)
{
    int bytes;
    int32_t resultError = HANDSHAKE_DENEGADO;
    log_info(logger,"HANDSHAKE DENEGADO A: %s!", nombreOrigen);
    bytes = send(fd_origen, &resultError, sizeof(int32_t), 0);
    if(bytes == -1)
    {
        log_error(logger,"Error al enviar el resultado del handshake (funcion send)");
        perror("\nError en send de recibir_handshake");
        exit(1);
    }
}

void enviar_datos_config_memoria_a_cpu(int tam_memoria, int tam_pagina,int retardo_memoria, int fd_cpu)
{   
    t_paquete* nuevo_paquete = crear_paquete(DATOS_CONFIG);

    agregar_a_paquete(nuevo_paquete,&tam_memoria,sizeof(int));
    agregar_a_paquete(nuevo_paquete,&tam_pagina,sizeof(int));
    agregar_a_paquete(nuevo_paquete,&retardo_memoria,sizeof(int));

    enviar_paquete(nuevo_paquete,fd_cpu);
    eliminar_paquete(nuevo_paquete);
}

void recibir_datos_config_memoria(int* tam_memoria, int* tam_pagina,int* retardo_memoria, int fd_memoria,t_log* logger)
{
    if(recibir_operacion(fd_memoria)==DATOS_CONFIG)
    {
        t_list* paquete_recibido = recibir_paquete(fd_memoria);

        int* tam_memoria_recibido = list_get(paquete_recibido,0);
        int* tam_pagina_recibido = list_get(paquete_recibido,1);
        int* retardo_memoria_recibido = list_get(paquete_recibido,2);

        *tam_memoria = *tam_memoria_recibido;
        *tam_pagina = *tam_pagina_recibido;
        *retardo_memoria = *retardo_memoria_recibido;

        list_destroy_and_destroy_elements(paquete_recibido,(void*)liberar_elemento);
    }
    else
    {
        log_error(logger,"ERROR AL RECIBIR DATOS CONFIG DE MEMORIA");
        abort();
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

int enviar_paquete_io(t_paquete* paquete, int socket_cliente)
{
    int bytes = paquete->buffer->size + 2*sizeof(int);
    void* a_enviar = serializar_paquete(paquete, bytes);

    int err = send(socket_cliente, a_enviar, bytes, MSG_NOSIGNAL);

    free(a_enviar);

    return err;
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

int recibir_aviso(int fd_dispatch)
{
    int rta;
    recv(fd_dispatch,&rta,sizeof(int),MSG_WAITALL);

    return rta; //PUEDE DEVOLVER NUEVO_PID O MISMO_PID
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

int enviar_solicitud_io_generico(int pid, int tiempo, int fd_interfaz)
{
    t_paquete* paquete = crear_paquete(IO_GEN_SLEEP);
    
    agregar_a_paquete(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, &tiempo, sizeof(int));
    
    int err = enviar_paquete_io(paquete, fd_interfaz);//DEVUELVE -1 SI HUBO ERROR
    
    eliminar_paquete(paquete);
    return err;
}

void recibir_solicitud_io_generico(int* pid,int* tiempo, int fd_kernel)
{
    t_list* lista = recibir_paquete(fd_kernel);
    int* pid_recibido = list_get(lista, 0);
    int* tiempo_recibido = list_get(lista, 1);
    *pid = *pid_recibido;
    *tiempo = *tiempo_recibido;
    list_destroy_and_destroy_elements(lista,(void*) liberar_elemento);
}

int enviar_solicitud_stdin_stdout(int pid, t_list* direcciones_fisicas , int fd_interfaz , int cant_direcciones, int tipo_interfaz)
{
    t_paquete* paquete = crear_paquete(tipo_interfaz);
    t_porcion_dato auxiliar;
    agregar_a_paquete(paquete,&pid,sizeof(int));

    for(int i=0;i<cant_direcciones;i++)
    {
        t_porcion_dato* direccion = list_get(direcciones_fisicas,i);
        auxiliar.base=direccion->base;
        auxiliar.direccion_fisica=direccion->direccion_fisica;
        auxiliar.tamanio=direccion->tamanio;
        agregar_a_paquete(paquete,&auxiliar,sizeof(t_porcion_dato));
    }
    int err = enviar_paquete_io(paquete, fd_interfaz);//DEVUELVE -1 SI HUBO ERROR

    eliminar_paquete(paquete);

    return err;
}

void enviar_nuevo_proceso(int* pid, char* nombre_archivo, int fd_memoria){
    t_paquete* paquete = crear_paquete(INICIAR_PROCESO);

    agregar_a_paquete(paquete, pid, sizeof(int));
    //printf("\nENVIAR NUEVO PROCESO PID: %d\n",*pid);
    //printf("\nENVIAR NUEVO PROCESO NOMBRE ARCHIVO: %s\n",nombre_archivo);
    agregar_a_paquete(paquete, nombre_archivo, strlen(nombre_archivo)+1);
    //int tamanio = strlen(nombre_archivo);
    //printf("\nLONGITUD NOMBRE ARCHIVO: %d\n",tamanio);
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

//ENVIO Y RECEPCION DE INTERRUPCIONES

void enviar_interrupcion(int interrupcion, int pid, int fd_interrupt){
    t_paquete* paquete = crear_paquete(interrupcion);
    agregar_a_paquete(paquete, &pid, sizeof(int));
    enviar_paquete(paquete, fd_interrupt);
    eliminar_paquete(paquete);
}

int recibir_interrupcion(int* pid, int fd_interrupt)
{
    int interrupcion_recibida = recibir_operacion(fd_interrupt);//CONFIAMOS EN QUE SOLO LLEGAN INTERRUPCIONES, NOS LO DIJO UN AYUDANTE
    if(interrupcion_recibida !=  DESCONEXION && interrupcion_recibida != ERROR)
    {
        t_list* paquete = recibir_paquete(fd_interrupt);
        int* pid_recibido = list_get(paquete,0);

        *pid = *pid_recibido;

        list_destroy_and_destroy_elements(paquete,(void*)liberar_elemento);
    }
    return interrupcion_recibida;
}

//PAQUETE ESCRITURA DE CPU-MEMORIA
void enviar_paquete_escritura(int pid,void* dato,int tamanio_dato,int base,int tamanio_fragmento,int direccion_fisica, int fd_destino)
{
    t_paquete* paquete = crear_paquete(ESCRITURA);

    agregar_a_paquete(paquete,&pid,sizeof(int));
    agregar_a_paquete(paquete,dato,tamanio_dato);
    agregar_a_paquete(paquete,&base,sizeof(int));
    agregar_a_paquete(paquete,&tamanio_fragmento,sizeof(int));
    agregar_a_paquete(paquete,&direccion_fisica,sizeof(int));

    enviar_paquete(paquete,fd_destino);

    eliminar_paquete(paquete);
}
//LECTURA 
void enviar_paquete_lectura(int pid,int tamanio_fragmento,int direccion_fisica, int fd_destino)
{
    t_paquete* paquete = crear_paquete(LECTURA);

    agregar_a_paquete(paquete,&pid,sizeof(int));
    agregar_a_paquete(paquete,&tamanio_fragmento,sizeof(int));
    agregar_a_paquete(paquete,&direccion_fisica,sizeof(int));

    enviar_paquete(paquete,fd_destino);

    eliminar_paquete(paquete);
}
void* recibir_dato_leido (int fd_destino, int tamanio_fragmento)
{
    void* dato_leido = malloc(tamanio_fragmento); //requiere free fuera funcion
    recv(fd_destino,dato_leido,tamanio_fragmento,MSG_WAITALL);
    return dato_leido;
}

//SOLICITUD MACRO
void enviar_solicitud_macro(int pid, int pagina_solicitar,int fd_destino)
{
            
    t_paquete* paquete = crear_paquete(ACCESO_TABLA_PAGINA);
            
    agregar_a_paquete(paquete,&pid,sizeof(int));
    agregar_a_paquete(paquete,&pagina_solicitar,sizeof(int));

    enviar_paquete(paquete,fd_destino);
            
    eliminar_paquete(paquete);
}
//SOLICITUD TODOS LOS MARCOS, DEVUELVE LISTA ORDENADA CON LOS MARCOS
/*
t_list* solicitar_macros(int pagina_inicial, int paginas_totales,int pid, int fd_destinatario)
{
    t_list* lista_marcos = list_create();
    int* marco;
    for(int i=0;i<paginas_totales;i++)
    {
        //BUSCAR MARCO EN TLB
        *marco = 
        marco = malloc(sizeof(int));
        enviar_solicitud_macro(pid,pagina_inicial,fd_destinatario);
        pagina_inicial++;
        recv(fd_destinatario,marco,sizeof(int),MSG_WAITALL);
        list_add(lista_marcos,marco);
    }
    return lista_marcos;
}*/

//DESERIALIZAR DATOS CREACION NUEVO PROCESO
void recibir_creacion_proceso(int* pid, char** path_kernel,int fd_kernel)
{
    t_list* paquete = recibir_paquete(fd_kernel);
    int* pid_recibido = list_get(paquete,0);
    char* path_kernel_recibido = list_get(paquete,1);
    *pid = *pid_recibido;
    *path_kernel = string_duplicate(path_kernel_recibido);
    list_destroy_and_destroy_elements(paquete,(void*) liberar_elemento);
}
// RECIBIR DESTRUCCION PROCESO
void recibir_destruccion_proceso(int* pid, int fd_kernel)
{
    t_list* paquete = recibir_paquete(fd_kernel);
    int* pid_recibido = list_get(paquete,0);
    *pid = *pid_recibido;
    list_destroy_and_destroy_elements(paquete,(void*)liberar_elemento);
}

void* recibir_peticion_escritura(int* pid,int* base,int* tamanio,int* direccion_fisica,int fd_cpu)
{
    t_list* porcion_escritura = recibir_paquete(fd_cpu);
    //VIENE EN ORDEN: DATO, BASE, TAMANIO, DIRECCION FISICA
    int* pid_recibido = list_get(porcion_escritura,0);
    void* dato_escribir = list_get(porcion_escritura,1);
    int* base_recibido = list_get(porcion_escritura,2);
    int* tamanio_recibido = list_get(porcion_escritura,3);
    int* direccion_fisica_recibido = list_get(porcion_escritura,4);

    *pid = *pid_recibido;
    *base = *base_recibido;
    *tamanio = *tamanio_recibido;
    *direccion_fisica = *direccion_fisica_recibido;

    list_destroy(porcion_escritura);
    free(pid_recibido);
    free(base_recibido);
    free(tamanio_recibido);
    free(direccion_fisica_recibido);

    return dato_escribir;
}

void* recibir_peticion_lectura(int* pid, int* tamanio,int* direccion_fisica,int fd_cpu)
{
    t_list* dato_leer = recibir_paquete(fd_cpu);
            
    int* pid_recibido = list_get(dato_leer,0);
    int* tamanio_fragmento_recibido = list_get(dato_leer,1);
    int* direccion_fisica_recibido = list_get(dato_leer,2);

    void* dato_leido = malloc(*tamanio_fragmento_recibido);

    *pid = *pid_recibido;
    *tamanio = *tamanio_fragmento_recibido;
    *direccion_fisica = *direccion_fisica_recibido;

    list_destroy_and_destroy_elements(dato_leer,(void*) liberar_elemento);

    return dato_leido;
}

void liberar_elemento(void* self)
{
    free(self);
}


   


































