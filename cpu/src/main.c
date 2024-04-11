#include "../include/main.h"

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

