#include "../include/main.h"

int main(int argc, char* argv[]) {
    t_log* logger;
    t_config* config;
    logger = iniciar_logger("entrada_salida.log","MODULO INTERFAZ",1,LOG_LEVEL_DEBUG);
    config = iniciar_config("entrada_salida.config",logger);
    conectar_interfaz(logger,config);

    
   return 0;
}
void conectar_interfaz(t_log* logger, t_config* config)
{
    char* ip;
    char* puerto;
    //CONEXION A KERNEL
    ip = config_get_string_value(config,"IP_KERNEL");
    puerto = config_get_string_value(config,"PUERTO_KERNEL");
    int fd_conexion_kernel = crear_conexion(ip,puerto,logger,"KERNEL");
    //CONEXION A MEMORIA
    ip = config_get_string_value(config,"IP_MEMORIA");
    puerto = config_get_string_value(config,"PUERTO_MEMORIA");
    int fd_conexion_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    

}
