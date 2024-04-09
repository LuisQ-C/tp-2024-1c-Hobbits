#include "../include/main.h"
 //el extern que va por delante es cuando queremos utilizar este logger en muchos archivos o lugares, pra no redeclarar lo mismo
typedef struct
{
    int fd_escucha_memoria;
    t_log* logger;
} t_datos_server_interfaces;



int main(int argc, char* argv[]) {
    t_log* logger;
    t_config* config;
    logger = iniciar_logger("memoria.log", "MODULO MEMORIA", 1, LOG_LEVEL_DEBUG);
    config = iniciar_config("memoria.config",logger);
    iniciar_conexiones(config,logger);
    //int server_fd = iniciar_servidor(logger,"0.0.0.0","8096");
    
    
    
    
    log_destroy(logger);
    config_destroy(config);
    //close(server_fd); //liberamos el log, debemos liberar la conexion cuando se cierra server?
    return 0;
}
void iniciar_conexiones(t_config* config,t_log* logger)
{
    //FALTA IP, esta hardcodadeado
    char* puerto;
    pthread_t hiloConexionIO;
    t_datos_server_interfaces* datosServerIO = malloc(sizeof(t_datos_server_interfaces));
    //INICIA SERVIDOR
    puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
    int server_fd = iniciar_servidor(logger,"0.0.0.0",puerto);
    log_info(logger,"Servidor listo para recibir cliente!");
    //ESPERAR CPU
    int fd_cpu = esperar_cliente(server_fd,logger,"CPU");
    //ESPERAR KERNEL
    int fd_kernel = esperar_cliente(server_fd,logger,"KERNEL");
    //ESPERAR INFINITAMENTE IOS
    datosServerIO->fd_escucha_memoria = server_fd;
    datosServerIO->logger= logger;
    pthread_create(&hiloConexionIO,NULL,(void*) atender_interfaces,(void*) datosServerIO);
    pthread_join(hiloConexionIO,NULL);
}
void atender_interfaces(void* datosServerIO)
{
    t_datos_server_interfaces* auxiliarDatosServer = (t_datos_server_interfaces*) datosServerIO;
    int fd_escucha_interfaces = auxiliarDatosServer->fd_escucha_memoria;
    t_log* logger = auxiliarDatosServer->logger;
    free(auxiliarDatosServer);
    while(1)
    {
        int fd_conexion_IO = esperar_cliente(fd_escucha_interfaces,logger,"INTERFAZ I/O");
    }
}

/*
    pthread_t hiloKernel;
    datos* args = malloc(sizeof(datos));
    args->fd = esperar_cliente(server_fd,logger);
    pthread_create(&hiloKernel,
                    NULL,
                    atender_cliente,
                    (void*) args);
    pthread_join(hiloKernel,NULL);
void* atender_cliente(void* args)
{
    datos* args2 = (datos*) args;
    int fd_conexion = args2->fd;
    while(1)
    {
        int32_t handshake = 6;
        int32_t resultOk = 0;
        int32_t resultError = -1;
        printf("El numero de hanshake antes de recibir: %d",handshake);
        recv(fd_conexion, &handshake, sizeof(int32_t), MSG_WAITALL);
        printf("El numero de hanshake recibido es: %d",handshake);
        if (handshake == 1) {
            send(fd_conexion, &resultOk, sizeof(int32_t), 0);
        } else {
            send(fd_conexion, &resultError, sizeof(int32_t), 0);
        }
        break;
    }
    close(fd_conexion);
    return NULL;
}*/




