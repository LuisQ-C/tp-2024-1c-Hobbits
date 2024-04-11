#include "../include/conexionesMemoria.h"

typedef struct
{
    int fd_escucha_interfaces;
    t_log* logger;
} t_datos_server_interfaces;

void iniciar_conexiones(t_log* logger,t_config* config,int* server_fd,int* fd_cpu,int* fd_kernel)
{
    //FALTA IP, esta hardcodadeado
    char* puerto;
    //pthread_t hiloConexionIO;
    //t_datos_server_interfaces* datosServerIO = malloc(sizeof(t_datos_server_interfaces));
    //INICIA SERVIDOR
    puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
    *server_fd = iniciar_servidor(logger,"0.0.0.0",puerto);
    log_info(logger,"Servidor listo para recibir cliente!");
    //ESPERAR CPU
    *fd_cpu = esperar_cliente(*server_fd,logger,"CPU");
    //ESPERAR KERNEL
    *fd_kernel = esperar_cliente(*server_fd,logger,"KERNEL");
    //ESPERAR INFINITAMENTE IOS
    //datosServerIO->fd_escucha_memoria = *server_fd;
    //datosServerIO->logger= logger;
    //pthread_create(&hiloConexionIO,NULL,(void*) atender_interfaces,(void*) datosServerIO);
    //pthread_join(hiloConexionIO,NULL);
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

void terminar_programa(t_log* logger,t_config* config,int* fd_cpu,int* fd_kernel){
    destruir_log_config(logger,config);
    close(*fd_cpu);
    close(*fd_kernel);
}


/*void atender_interfaces(void* datosServerIO)
{
    t_datos_server_interfaces* auxiliarDatosServer = (t_datos_server_interfaces*) datosServerIO;
    int fd_escucha_interfaces = auxiliarDatosServer->fd_escucha_memoria;
    t_log* logger = auxiliarDatosServer->logger;
    free(auxiliarDatosServer);
    while(1)
    {
        int fd_conexion_IO = esperar_cliente(fd_escucha_interfaces,logger,"INTERFAZ I/O");
    }
}*/