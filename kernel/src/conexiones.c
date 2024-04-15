#include "../include/conexiones.h"
typedef struct
{
    int fd_conexion_IO;
    t_log* logger;
} t_datos_server_interfaces;

int iniciar_conexiones(t_config* config,t_log* logger,int* fd_memoria,int* fd_cpu_dispatch, int* fd_cpu_interrupt,int* fd_escucha_interfaces)
{
    char* ip;
    char* puerto;
    //CONECTARSE A MEMORIA
    ip = config_get_string_value(config,"IP_MEMORIA");
    puerto = config_get_string_value(config,"PUERTO_MEMORIA");
    *fd_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    //CONECTARSE A CPU A TRAVES DE DISPATCH E INTERRUPT
    ip = config_get_string_value(config,"IP_CPU");
    puerto = config_get_string_value(config,"PUERTO_CPU_DISPATCH");
    *fd_cpu_dispatch = crear_conexion(ip,puerto,logger,"CPU-DISPATCH");
    puerto = config_get_string_value(config,"PUERTO_CPU_INTERRUPT");
    *fd_cpu_interrupt = crear_conexion(ip,puerto,logger,"CPU-INTERRUPT");
    //LEVANTAR SERVER PARA I/O CON UN HILO
    ip = config_get_string_value(config,"IP_KERNEL");
    puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
    *fd_escucha_interfaces = iniciar_servidor(logger,ip,puerto);
    return *fd_memoria != 0 && *fd_cpu_dispatch != 0 && *fd_cpu_interrupt != 0 && *fd_escucha_interfaces != 0;
}

int escucharConexionesIO(t_log* logger,int fd_escucha_interfaces){
    int err;
    int fd_conexion_IO = esperar_cliente(fd_escucha_interfaces,logger,"INTERFAZ I/O");
    pthread_t conexionesIO;
    t_datos_server_interfaces* datosServerInterfaces = malloc(sizeof(t_datos_server_interfaces));
    datosServerInterfaces->fd_conexion_IO= fd_conexion_IO;
    datosServerInterfaces->logger = logger;
    err = pthread_create(&conexionesIO,NULL,(void*) procesarConexionesIO,(void*) datosServerInterfaces);
    if(err != 0)
    {
        log_error(logger,"Error al crear el hilo de conexion IO");
        perror("\nError creando hilo IO");
        exit(1);
    }
    pthread_detach(conexionesIO);
    
    
    return 1;
}

void procesarConexionesIO(void* datosServerInterfaces){
    t_datos_server_interfaces* auxiliarDatosServer = (t_datos_server_interfaces*) datosServerInterfaces;
    int fd_conexion_IO = auxiliarDatosServer->fd_conexion_IO;
    t_log* logger = auxiliarDatosServer->logger;
    free(auxiliarDatosServer);
    recibir_handshake(logger,fd_conexion_IO,"MODULO I/O");
}

void terminar_programa(t_log* logger,t_config* config,int* fd_memoria,int* fd_cpu_dispatch,int* fd_cpu_interrupt)
{
    destruir_log_config(logger,config);
    close(*fd_memoria);
    close(*fd_cpu_dispatch);
    close(*fd_cpu_interrupt);
}
