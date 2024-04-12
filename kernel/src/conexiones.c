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
    int fd_escucha_interfaces;
    t_log* logger;
} t_datos_server_interfaces;

void iniciar_conexiones(t_config* config,t_log* logger,int* fd_memoria,int* fd_cpu_dispatch, int* fd_cpu_interrupt,int* fd_escucha_interfaces)
{
    char* ip;
    char* puerto;
    //pthread_t conexionesIO;
    //t_datos_server_interfaces* datosServerInterfaces = malloc(sizeof(t_datos_server_interfaces));
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
    //datosServerInterfaces->fd_escucha_interfaces = *fd_escucha_interfaces;
    //datosServerInterfaces->logger = logger;
    //pthread_create(&conexionesIO,NULL,(void*) escucharConexionesIO,(void*) datosServerInterfaces);
    //pthread_join(conexionesIO,NULL);
    //pthread_detach(conexionesIO);
    //log_info(logger,"Sigue ejecutando");
}

int escucharConexionesIO(t_log* logger,int fd_escucha_interfaces){
    int fd_conexion_IO = esperar_cliente(fd_escucha_interfaces,logger,"INTERFAZ I/O");
    pthread_t conexionesIO;
    t_datos_server_interfaces* datosServerInterfaces = malloc(sizeof(t_datos_server_interfaces));
    datosServerInterfaces->fd_escucha_interfaces = fd_escucha_interfaces;
    datosServerInterfaces->logger = logger;
    pthread_create(&conexionesIO,NULL,(void*) procesarConexionesIO,(void*) datosServerInterfaces);
    pthread_detach(conexionesIO);
    
    
    return 1;
}

void procesarConexionesIO(void* datosServerInterfaces){
    t_datos_server_interfaces* auxiliarDatosServer = (t_datos_server_interfaces*) datosServerInterfaces;
    int fd_escucha_interfaces = auxiliarDatosServer->fd_escucha_interfaces;
    t_log* logger = auxiliarDatosServer->logger;
    free(auxiliarDatosServer);
   //int a;
   //scanf("%d",&a);
    //while(1);
    log_info(logger,"CERRANDO HILO");
}

void terminar_programa(t_log* logger,t_config* config,int* fd_memoria,int* fd_cpu_dispatch,int* fd_cpu_interrupt)
{
    destruir_log_config(logger,config);
    close(*fd_memoria);
    close(*fd_cpu_dispatch);
    close(*fd_cpu_interrupt);
}
/*void escucharConexionesIO(void* datosServerInterfaces)
{
    t_datos_server_interfaces* auxiliarDatosServer = (t_datos_server_interfaces*) datosServerInterfaces;
    int fd_escucha_interfaces = auxiliarDatosServer->fd_escucha_interfaces;
    t_log* logger = auxiliarDatosServer->logger;
    free(auxiliarDatosServer);
    while(1)
    {
    }
}*/










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
