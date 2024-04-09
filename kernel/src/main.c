#include "../include/main.h"

int main(int argc, char* argv[]) {
    //int fd_memoria = 0;
    //int fd_cpu = 0;


    t_log* logger;
    t_config* config;

    logger = iniciar_logger("kernel.log","Kernel",1,LOG_LEVEL_INFO);
    config = iniciar_config("kernel.config",logger);

    iniciar_conexiones(config,logger); //falta manejo errores por si falla

    //liberar_conexion(fd_memoria);
    //liberar_conexion(fd_cpu);
    log_destroy(logger);
    config_destroy(config);

    //while(1);
    
    /*
    {
        escuchar_interfaces();
    }*/
    
    //LIBREAMOS LOG, CONFIG Y AMBAS CONEXIONES
    
    

}
