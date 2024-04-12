#include "../include/main.h"

t_log* logger;
t_config* config;
int fd_conexion_memoria = 0;
int server_fd_escucha_dispatch = 0;
int server_fd_escucha_interrupt = 0;
int cliente_fd_conexion_interrupt = 0;
int cliente_fd_conexion_dispatch = 0;

int main(int argc, char* argv[]) 
{
    logger = iniciar_logger("cpu.log", "MODULO CPU", 1, LOG_LEVEL_DEBUG);
    config = iniciar_config("cpu.config",logger);
    iniciar_conexiones(logger,config,&fd_conexion_memoria,&server_fd_escucha_dispatch,&server_fd_escucha_interrupt,&cliente_fd_conexion_dispatch,&cliente_fd_conexion_interrupt);
    mandarHandshake(logger,fd_conexion_memoria,"MODULO MEMORIA");
    terminar_programa(logger,config,&fd_conexion_memoria,&cliente_fd_conexion_dispatch,&cliente_fd_conexion_interrupt);
    //close(server_fd_escucha_dispatch);
    //close(server_fd_escucha_interrupt);

    return 0;
}

