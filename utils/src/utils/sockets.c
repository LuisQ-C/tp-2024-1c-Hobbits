#include "../include/sockets.h"

void decir_hola(char* quien) {
    printf("Hola desde %s!!\n", quien);
}

int iniciar_servidor(t_log* logger,char* ip,char* puerto)
{
    //NO HACE FALTA ENTENDER EN PROFUNDIDAD LO SIGUIENTE:
    int socket_servidor;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    //LEER PUERTO DESDE CONFIG, LA IP ESTA BIEN PORQUE SE EJECUTA EN LA IP DE LA MAQUINA DONDE VA CORRER
    getaddrinfo(ip,puerto,&hints,&servinfo);

    //Creamos el socket de escucha del servidor
    socket_servidor = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
    //ERROA AL CREAR SOCKET
    if(socket_servidor == -1)
    {
        log_error(logger,"Error al crear el socket!");
        close(socket_servidor); //NO SE SI ESTA BIEN, ELIMINA EL FD
    }
    //Asociamos socket a un puerto
    bind(socket_servidor,servinfo->ai_addr,servinfo->ai_addrlen);//NO DEVUELVE ERROR
    //escuchamos conexiones entrantes, ponemos el socket creado como escucha, solo se encarga de eso
    if(listen(socket_servidor,SOMAXCONN) == -1){
        log_error(logger,"Error al marcar socket como escucha!");
        close(socket_servidor); //ELIMINA EL FD,termina conexion
    }
    

    freeaddrinfo(servinfo);
    log_trace(logger,"Listo para escuchar a mi cliente!");

    return socket_servidor;
}
int esperar_cliente(int socket_servidor,t_log* logger)
{

	// Aceptamos un nuevo cliente, se crea un nuevo socket, canal de comunicacion
	int socket_cliente = accept(socket_servidor, NULL, NULL);
    if(socket_cliente == -1)
    {
        log_error(logger,"No se pudo aceptar la solicitud del cliente!");
        close(socket_cliente);
    }
	log_info(logger,"Se conecto un cliente!");

	return socket_cliente;
}
int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,server_info->ai_socktype,server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_cliente;
}
void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
