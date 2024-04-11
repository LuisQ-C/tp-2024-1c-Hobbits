#include "../include/conexionesCPU.h"

typedef struct
{
    int fd_conexion;
}datos;

void iniciar_conexiones(t_log* logger,t_config* config,int* fd_conexion_memoria,int* server_fd_escucha_dispatch, int* server_fd_escucha_interrupt, int* cliente_fd_conexion_dispatch, int* cliente_fd_conexion_interrupt)
{
    char* ip;
    char* puerto;
    //PRIMERO SE CONECTA A MEMORIA
    ip=config_get_string_value(config,"IP_MEMORIA");
    puerto=config_get_string_value(config,"PUERTO_MEMORIA");
    *fd_conexion_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    //LUEGO EMPIEZA A ESCUCHAR CONEXIONES ENTRANTES
    puerto=config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
    *server_fd_escucha_dispatch = iniciar_servidor(logger,"0.0.0.0",puerto);
    log_info(logger,"Servidor escuchando en puerto dispatch");
    puerto=config_get_string_value(config,"PUERTO_ESCUCHA_INTERRUPT");
    *server_fd_escucha_interrupt = iniciar_servidor(logger,"0.0.0.0",puerto);
    log_info(logger,"Servidor escuchando en puerto interrupt");
    //ESPERA A QUE SE LE CONECTE EL KERNEL
    *cliente_fd_conexion_dispatch = esperar_cliente(*server_fd_escucha_dispatch,logger,"KERNEL_DISPATCH");
    *cliente_fd_conexion_interrupt = esperar_cliente(*server_fd_escucha_interrupt,logger,"KERNEL_INTERRUPT");
}

void terminar_programa(t_log* logger, t_config* config, int* fd_conexion_memoria,int* cliente_fd_conexion_dispatch,int*cliente_fd_conexion_interrupt)
{
    destruir_log_config(logger,config);
    close(*fd_conexion_memoria);
    close(*cliente_fd_conexion_dispatch);
    close(*cliente_fd_conexion_interrupt);
}

/*
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
*/