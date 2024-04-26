#include "../include/conexionesMemoria.h"
extern t_log* logger;
extern t_config* config;
typedef struct
{
    int fd_conexion_IO;
} t_datos_server_interfaces;



int iniciar_conexiones(int* server_fd,int* fd_cpu,int* fd_kernel)
{
    //FALTA IP, esta hardcodadeado
    char* puerto;
    //INICIA SERVIDOR
    puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
    *server_fd = iniciar_servidor(logger,"0.0.0.0",puerto);
    log_info(logger,"Servidor listo para recibir cliente!");
    //ESPERAR CPU
    *fd_cpu = esperar_cliente(*server_fd,logger,"CPU");
    //ESPERAR KERNEL
    *fd_kernel = esperar_cliente(*server_fd,logger,"KERNEL");
    
    return *server_fd != 0 && *fd_cpu != 0 && *fd_kernel != 0;
}

void realizar_handshakes_memoria(int fd_cpu, int fd_kernel)
{
    realizar_handshake_cpu(fd_cpu);
    realizar_handshake_kernel(fd_kernel);
}

void inicializar_hilos(int fd_cpu, int fd_kernel)
{
    iniciar_hilo_cpu(fd_cpu);
    iniciar_hilo_kernel(fd_kernel);
}

int escucharConexionesIO(int fd_escucha_interfaces){
    int fd_conexion_IO = esperar_cliente(fd_escucha_interfaces,logger,"INTERFAZ I/O");
    int err;
    pthread_t conexionesIO;
    t_datos_server_interfaces* datosServerInterfaces = malloc(sizeof(t_datos_server_interfaces));
    datosServerInterfaces->fd_conexion_IO = fd_conexion_IO;
    err = pthread_create(&conexionesIO,NULL,(void*) procesarConexionesIO,(void*) datosServerInterfaces);
    if(err != 0)
    {
        log_error(logger,"Error al crear el hilo de conexion IO");
        perror("\nError hilo");
        exit(1);
    }
    pthread_detach(conexionesIO);
    
    
    return 1;
}

void procesarConexionesIO(void* datosServerInterfaces){
    t_datos_server_interfaces* auxiliarDatosServer = (t_datos_server_interfaces*) datosServerInterfaces;
    int fd_conexion_IO = auxiliarDatosServer->fd_conexion_IO;
    free(auxiliarDatosServer);

    int codigoOperacion = recibir_operacion(fd_conexion_IO);

    if(codigoOperacion == -1)
    {
        log_error(logger,"Error al recibirOperacion");
        //return;
    }

    char* interfazConectada = recibir_mensaje(fd_conexion_IO,logger);
    enviar_handshake_ok(logger,fd_conexion_IO,interfazConectada);
    free(interfazConectada);
}





void terminar_programa(int* fd_cpu,int* fd_kernel){
    destruir_log_config(logger,config);
    close(*fd_cpu);
    close(*fd_kernel);
}

