#include "../include/main.h"
t_log* logger;
t_config* config;
int fd_memoria = 0;
int fd_cpu_dispatch = 0;
int fd_cpu_interrupt = 0;
int fd_escucha_interfaces = 0;


int main(int argc, char* argv[]) {

    logger = iniciar_logger("kernel.log","Kernel",1,LOG_LEVEL_INFO);
    config = iniciar_config("kernel.config",logger);

    iniciar_conexiones(config,logger,&fd_memoria,&fd_cpu_dispatch,&fd_cpu_interrupt,&fd_escucha_interfaces); //falta manejo errores por si falla
    while(escucharConexionesIO(logger,fd_escucha_interfaces));
    terminar_programa();
    return 0;
}

void terminar_programa()
{
    destruir_log_config(logger,config);
    close(fd_memoria);
    close(fd_cpu_dispatch);
    close(fd_cpu_interrupt);
}