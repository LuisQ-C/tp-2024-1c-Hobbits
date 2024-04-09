#include "../include/main.h"


typedef struct
{
    int fd_conexion;
}datos;


int main(int argc, char* argv[]) 
{
    t_log* logger;
    t_config* config;
    logger = iniciar_logger("cpu.log", "MODULO CPU", 1, LOG_LEVEL_DEBUG);
    config = iniciar_config("cpu.config",logger);
    iniciar_conexiones(config,logger);
    
    
    /*
    pthread_t hiloKernel;
    datos* args = malloc(sizeof(datos));
    args->fd_conexion = esperar_cliente(server_fd,logger);
    pthread_create(&hiloKernel,
                    NULL,
                    atender_cliente,
                    (void*) args);
    pthread_join(hiloKernel,NULL);
    */
    //pthread_detach(thread);
    //}
    
    log_destroy(logger);//liberamos el logger, preguntar si hay que hacer close cuando cerramos servidor
    config_destroy(config);
    //close(server_fd);
    return 0;
}
void iniciar_conexiones(t_config* config,t_log* logger)
{
    char* ip;
    char* puerto;
    ip=config_get_string_value(config,"IP_MEMORIA");
    puerto=config_get_string_value(config,"PUERTO_MEMORIA");
    //PRIMERO SE CONECTA A MEMORIA
    int fd_conexion_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    //LUEGO EMPIEZA A ESCUCHAR CONEXIONES ENTRANTES
    puerto=config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
    int server_fd_escucha_dispatch = iniciar_servidor(logger,"0.0.0.0",puerto);
    log_info(logger,"Servidor escuchando en puerto dispatch");
    puerto=config_get_string_value(config,"PUERTO_ESCUCHA_INTERRUPT");
    int server_fd_escucha_interrupt = iniciar_servidor(logger,"0.0.0.0",puerto);
    log_info(logger,"Servidor escuchando en puerto interrupt");
    //ESPERA A QUE SE LE CONECTE EL KERNEL
    int cliente_fd_conexion_dispatch = esperar_cliente(server_fd_escucha_dispatch,logger,"KERNEL_DISPATCH");
    int cliente_fd_conexion_interrupt = esperar_cliente(server_fd_escucha_interrupt,logger,"KERNEL_INTERRUPT");
}
void* atender_cliente(void* args)
{
    datos* args2 = (datos*) args;
    int fd_conexion = args2->fd_conexion;
    free(args2);
    while(1)
    {
        int32_t handshake = 6;
        int32_t resultOk = 0;
        int32_t resultError = -1;
        printf("\nNumero de handshake antes de recibir:%d ",handshake);
        recv(fd_conexion, &handshake, sizeof(int32_t), MSG_WAITALL);
        printf("\nNumero de handshake recibido:%d ",handshake);
        if (handshake == 1) {
            printf("\nEL result enviado es correcto");
            send(fd_conexion, &resultOk, sizeof(int32_t), 0);
        } else {
            printf("\nEL result enviado es error");
            send(fd_conexion, &resultError, sizeof(int32_t), 0);
        }
        break;
    }
    close(fd_conexion);
    return NULL;
}