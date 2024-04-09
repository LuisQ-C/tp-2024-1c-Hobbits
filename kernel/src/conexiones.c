#include "../include/conexiones.h"
typedef struct
{
    int fd;
} datosHiloCPU;
typedef struct
{
    int fd;
} datosHiloMemoria;

typedef struct
{
    char* ip;
    char* puerto;
    t_log* logger;
} t_datos_server_interfaces;

/*
int generar_conexiones(t_log* logger,int* fd_memoria,int* fd_cpu,t_config* config){
    char* ip;
    char* puerto;

    
    
    ip = config_get_string_value(config,"IP_MEMORIA");
    puerto = config_get_string_value(config,"PUERTO_MEMORIA");

    *fd_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    
    ip = config_get_string_value(config,"IP_CPU");
    puerto = config_get_string_value(config,"PUERTO_CPU");

    *fd_cpu = crear_conexion(ip,puerto,logger,"CPU");
    //HILOS
    crear_hilos(fd_cpu,fd_memoria);
    
    return fd_memoria != 0 && fd_cpu != 0;
}*/
void iniciar_conexiones(t_config* config,t_log* logger)
{
    char* ip;
    char* puerto;
    pthread_t conexionesIO;
    t_datos_server_interfaces* datosServerInterfaces = malloc(sizeof(t_datos_server_interfaces));
    //CONECTARSE A MEMORIA
    ip = config_get_string_value(config,"IP_MEMORIA");
    puerto = config_get_string_value(config,"PUERTO_MEMORIA");
    int fd_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    //CONECTARSE A CPU A TRAVES DE DISPATCH E INTERRUPT
    ip = config_get_string_value(config,"IP_CPU");
    puerto = config_get_string_value(config,"PUERTO_CPU_DISPATCH");
    int fd_cpu_dispatch = crear_conexion(ip,puerto,logger,"CPU-DISPATCH");
    puerto = config_get_string_value(config,"PUERTO_CPU_INTERRUPT");
    int fd_cpu_interrupt = crear_conexion(ip,puerto,logger,"CPU-INTERRUPT");
    //LEVANTAR SERVER PARA I/O CON UN HILO
    datosServerInterfaces->ip = config_get_string_value(config,"IP_KERNEL");
    datosServerInterfaces->puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
    datosServerInterfaces->logger = logger;
    pthread_create(&conexionesIO,NULL,(void*) escucharConexionesIO,(void*) datosServerInterfaces);
    pthread_join(conexionesIO,NULL);

    log_info(logger,"Sigue ejecutando");
}
void escucharConexionesIO(void* datosServerInterfaces)
{
    t_datos_server_interfaces* auxiliarDatosServer = (t_datos_server_interfaces*) datosServerInterfaces;
    char* ip = auxiliarDatosServer->ip;
    char* puerto = auxiliarDatosServer->puerto;
    t_log* logger = auxiliarDatosServer->logger;
    free(auxiliarDatosServer);
    int fd_escucha_interfaces = iniciar_servidor(logger,ip,puerto);
    while(1)
    {
        int fd_conexion_IO = esperar_cliente(fd_escucha_interfaces,logger,"INTERFAZ I/O");
    }
}














/*
void escuchar_interfaces()
{
    while(1)
    {
        pthread_t hiloConexionIO;
        pthread_create(&hiloConexionIO,NULL,(void*) gestionarConexionConInterfaces,NULL);
        pthread_detach(hiloConexionIO);
    }
}
void crear_hilos(int* fd_cpu,int* fd_memoria)
{
    pthread_t hiloMemoria, hiloCPU;
    datosHiloCPU* fileDescriptorCPU = malloc(sizeof(datosHiloCPU));
    datosHiloMemoria* fileDescriptorMemoria = malloc(sizeof(datosHiloMemoria));
    fileDescriptorCPU->fd = *fd_cpu;
    fileDescriptorMemoria->fd = *fd_memoria;
    pthread_create(&hiloCPU,NULL,gestionarConexionCPU, (void*) fileDescriptorCPU);
    pthread_create(&hiloMemoria,NULL,gestionarConexionMemoria,(void*) fileDescriptorMemoria);
    pthread_join(hiloCPU,NULL);
    pthread_join(hiloMemoria,NULL);
}
void* gestionarConexionConInterfaces(void* args)
{
    return NULL;
}
void* gestionarConexionCPU(void* fileDescriptorCPU)
{
    datosHiloCPU* recepcionFD = (datosHiloCPU*) fileDescriptorCPU;
    int fd_cpu = recepcionFD->fd;
    free(recepcionFD);
    int32_t result = 3;
    int32_t handshake = 1;
    while(1)
    {   
        printf("\n %d", result);
        send(fd_cpu,&handshake,sizeof(int32_t),0);
        recv(fd_cpu,&result,sizeof(int32_t),MSG_WAITALL);
        printf("\n %d", result);
        if(result==0)
        {
            printf("\nLA CPU TE ACEPTO!");
        }else{
            printf("\nLA CPU TE DENEGO!");
        }
        break;
    }
    close(fd_cpu);
    return NULL;
}
void* gestionarConexionMemoria(void* fileDescriptorMemoria)
{
    datosHiloMemoria* recepcionFD = (datosHiloMemoria*) fileDescriptorMemoria;
    int fd_memoria = recepcionFD->fd;
    free(recepcionFD);
    int32_t result;
    int32_t handshake = 1;
    while(1)
    {
        send(fd_memoria,&handshake,sizeof(int32_t),0);
        recv(fd_memoria,&result,sizeof(int32_t),MSG_WAITALL);
        if(result==0)
        {
            printf("\nLA MEMORIA TE ACEPTO!");
        }else{
            printf("\nLA MEMORIA TE DENEGO!");
        }
        break;
    }
    close(fd_memoria);
    return NULL;
}*/
