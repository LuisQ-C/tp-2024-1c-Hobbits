#include "../include/conexionesMemoria.h"

typedef struct
{
    int fd_escucha_memoria;
    t_log* logger;
} t_datos_server_interfaces;

void iniciar_conexiones(t_log* logger,t_config* config,int* server_fd,int* fd_cpu,int* fd_kernel)
{
    //FALTA IP, esta hardcodadeado
    char* puerto;
    pthread_t hiloConexionIO;
    t_datos_server_interfaces* datosServerIO = malloc(sizeof(t_datos_server_interfaces));
    //INICIA SERVIDOR
    puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
    *server_fd = iniciar_servidor(logger,"0.0.0.0",puerto);
    log_info(logger,"Servidor listo para recibir cliente!");
    //ESPERAR CPU
    *fd_cpu = esperar_cliente(*server_fd,logger,"CPU");
    //ESPERAR KERNEL
    *fd_kernel = esperar_cliente(*server_fd,logger,"KERNEL");
    //ESPERAR INFINITAMENTE IOS
    datosServerIO->fd_escucha_memoria = *server_fd;
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

void terminar_programa(t_log* logger,t_config* config,int* fd_cpu,int* fd_kernel){
    destruir_log_config(logger,config);
    close(*fd_cpu);
    close(*fd_kernel);
}