#include "../include/main.h"

int main(int argc, char* argv[]) {
    int fd_conexion_kernel = 0;
    int fd_conexion_memoria = 0;
    t_log* logger;
    t_config* config;
    logger = iniciar_logger("entrada_salida.log","MODULO INTERFAZ",1,LOG_LEVEL_DEBUG);
    config = iniciar_config("entrada_salida.config",logger);
    if(!iniciar_conexiones(logger,config,&fd_conexion_kernel,&fd_conexion_memoria))
    {
        log_error(logger,"Error al crear conexiones iniciales");
        exit(1);
    }
    mandarHandshake(logger,fd_conexion_kernel,"MODULO KERNEL");
    mandarHandshake(logger,fd_conexion_memoria,"MODULO MEMORIA");
    manejarConexion(logger,&fd_conexion_kernel,&fd_conexion_memoria);
    terminar_programa(logger,config,&fd_conexion_kernel,&fd_conexion_memoria);
    
   return 0;
}


