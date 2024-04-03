#include "../include/sockets.h"

void decir_hola(char* quien) {
    printf("Hola desde %s!!\n", quien);
}

int iniciar_servidor(t_log* logger,char* ip,char* puerto)
{
    //NO HACE FALTA ENTENDER EN PROFUNDIDAD LO SIGUIENTE:
    int err;
    int fd_escucha;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    //LEER PUERTO DESDE CONFIG, LA IP ESTA BIEN PORQUE SE EJECUTA EN LA IP DE LA MAQUINA DONDE VA CORRER
    err = getaddrinfo(ip,puerto,&hints,&servinfo);
    if(err != 0){
        log_error(logger,"Error al obtener informacion de la ip y puerto del servidor");
        exit(1);
    }
    //Creamos el socket de escucha del servidor
    fd_escucha = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
    if(fd_escucha == -1)
    {
        log_error(logger,"Error al crear el socket!");
        exit(1); 
    }
    //Asociamos socket a un puerto
    err = bind(fd_escucha,servinfo->ai_addr,servinfo->ai_addrlen);
    if(err != 0){
        log_error(logger,"Error al bindear el socket al ip y puerto");
        exit(1);
    }
    //escuchamos conexiones entrantes, ponemos el socket creado como escucha, solo se encarga de eso
    err = listen(fd_escucha,SOMAXCONN);
    if(err == -1){
        log_error(logger,"Error al marcar el socket como escucha");
        exit(1);
    }
    

    freeaddrinfo(servinfo);
    log_trace(logger,"Listo para escuchar a mi cliente!");

    return fd_escucha;
}
int esperar_cliente(int fd_escucha,t_log* logger)
{

	// Aceptamos un nuevo cliente, se crea un nuevo socket, canal de comunicacion
	int fd_conexion = accept(fd_escucha, NULL, NULL);
    if(fd_conexion == -1)
    {
        log_error(logger,"No se pudo aceptar la conexion entrante");
        exit(1);
    }
	log_info(logger,"Se conecto un cliente!");

	return fd_conexion;
}
int crear_conexion(char *ip, char* puerto,t_log* logger,char* nombre_servidor)
{
    int err;
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo(ip, puerto, &hints, &server_info);
    if(err != 0){
        log_error(logger,"Error al obtener informacion de la ip y puerto del cliente"); // REVISAR DESCRIPCION DEL LOGGER
        exit(1);
    }
	// Ahora vamos a crear el socket.
	int fd_conexion = socket(server_info->ai_family,server_info->ai_socktype,server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	err = connect(fd_conexion, server_info->ai_addr, server_info->ai_addrlen);
    if(err == -1){
        log_error(logger,"Error al conectar cliente-servidor");
        exit(1);
    }

    log_info(logger,"Conectado a %s, IP:%s, PUERTO:%s",nombre_servidor,ip,puerto);

	freeaddrinfo(server_info);

	return fd_conexion;
}
void liberar_conexion(int fd_conexion)
{
	close(fd_conexion);
}
