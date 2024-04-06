#include "../include/main.h"

t_log* logger;
typedef struct
{
    int fd_conexion;
}datos;

int main(int argc, char* argv[]) {
    
    logger = iniciar_logger("cpu.log", "MODULO CPU", 1, LOG_LEVEL_DEBUG);
    int server_fd = iniciar_servidor(logger,"0.0.0.0","8097");
    log_info(logger,"Servidor listo para recibir cliente!");
    /*
    while(1){
        int cliente_fd = esperar_cliente(server_fd,logger);
    }*/
    pthread_t hiloKernel;
    datos* args = malloc(sizeof(datos));
    args->fd_conexion = esperar_cliente(server_fd,logger);
    pthread_create(&hiloKernel,
                    NULL,
                    (void*) atender_cliente,
                    (void*) args);
    pthread_join(hiloKernel,NULL);
    
    //pthread_detach(thread);
    //}
    
    log_destroy(logger);//liberamos el logger, preguntar si hay que hacer close cuando cerramos servidor
    return 0;
}
void* atender_cliente(void* args)
{
    
    datos* args2 = (datos*) args;
    int fd_conexion = args2->fd_conexion;
    while(1)
    {
        int32_t handshake = 6;
        int32_t resultOk = 0;
        int32_t resultError = -1;
        printf("\nNumero de handshake antes de recibir:%d ",handshake);
        recv(fd_conexion, &handshake, sizeof(int32_t), MSG_WAITALL);
        printf("\nNumero de handshake recibido:%d ",handshake);
        if (handshake == 2) {
            printf("\nEL result enviado es correcto");
            send(fd_conexion, &resultOk, sizeof(int32_t), 0);
        } else {
            printf("\nEL result enviado es error");
            send(fd_conexion, &resultError, sizeof(int32_t), 0);
        }
        break;
    }
    
    return NULL;
}