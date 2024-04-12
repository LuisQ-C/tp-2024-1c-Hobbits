#include "../include/conexionesIO.h"

void conectar_interfaz(t_log* logger, t_config* config,int* fd_conexion_kernel,int* fd_conexion_memoria){
    char* ip;
    char* puerto;
    //CONEXION A KERNEL
    ip = config_get_string_value(config,"IP_KERNEL");
    puerto = config_get_string_value(config,"PUERTO_KERNEL");
    *fd_conexion_kernel = crear_conexion(ip,puerto,logger,"KERNEL");
    //CONEXION A MEMORIA
    ip = config_get_string_value(config,"IP_MEMORIA");
    puerto = config_get_string_value(config,"PUERTO_MEMORIA");
    *fd_conexion_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    
}

void terminar_programa(t_log* logger, t_config* config,int* fd_conexion_kernel,int* fd_conexion_memoria)
{
    destruir_log_config(logger,config);
    close(*fd_conexion_kernel);
    close(*fd_conexion_memoria);
}