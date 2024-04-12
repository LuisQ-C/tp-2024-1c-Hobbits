#include "../include/main.h"

t_log* logger;
t_config* config;

int main(int argc, char* argv[]) {
    int fd_memoria = 0;
    int fd_cpu_dispatch = 0;
    int fd_cpu_interrupt = 0;
    int fd_escucha_interfaces = 0;

    logger = iniciar_logger("kernel.log","Kernel",1,LOG_LEVEL_INFO);
    config = iniciar_config("kernel.config",logger);

    iniciar_conexiones(config,logger,&fd_memoria,&fd_cpu_dispatch,&fd_cpu_interrupt,&fd_escucha_interfaces); //falta manejo errores por si falla
    while(escucharConexionesIO(logger,fd_escucha_interfaces));
    terminar_programa(logger,config,&fd_memoria,&fd_cpu_dispatch,&fd_cpu_interrupt);
    return 0;
}

