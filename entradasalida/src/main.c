#include "../include/main.h"

t_log* logger;

int main(int argc, char* argv[]) {
     char* nombreDeInterfaz;
    int fd_conexion_kernel = 0;
    int fd_conexion_memoria = 0;
   // t_log* logger;
    t_config* config;
    logger = iniciar_logger("entrada_salida.log","MODULO INTERFAZ",1,LOG_LEVEL_DEBUG);
    config = iniciar_config("entrada_salida.config",logger);
    if(!iniciar_conexiones(logger,config,&fd_conexion_kernel,&fd_conexion_memoria))
    {
        log_error(logger,"Error al crear conexiones iniciales");
        exit(1);
    }
    /*mandarHandshake(logger,fd_conexion_kernel,"MODULO KERNEL","INTERFAZ CUALQUIERA");
    mandarHandshake(logger,fd_conexion_memoria,"MODULO MEMORIA","INTERFAZ CUALQUIERA");
    manejarConexion(logger,&fd_conexion_kernel,&fd_conexion_memoria);*/
    
   
    nombreDeInterfaz = readline(">");
    printf("%s\n", nombreDeInterfaz);
    log_info(logger,"El tipo de interfaz recibida es: %s", nombreDeInterfaz);
    iniciarInterfaz(nombreDeInterfaz, config, fd_conexion_kernel );
    free(nombreDeInterfaz);

    //terminar_programa(logger,config,&fd_conexion_kernel,&fd_conexion_memoria);
    return 0;
}
