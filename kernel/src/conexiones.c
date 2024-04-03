#include "../include/conexiones.h"

int generar_conexiones(t_log* logger,int* fd_memoria,int* fd_cpu,t_config* config){
    char* ip;
    char* puerto;

    ip = config_get_string_value(config,"IP_MEMORIA");
    puerto = config_get_string_value(config,"PUERTO_MEMORIA");

    *fd_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    
    ip = config_get_string_value(config,"IP_CPU");
    puerto = config_get_string_value(config,"PUERTO_CPU");

    *fd_cpu = crear_conexion(ip,puerto,logger,"CPU");

    return fd_memoria != 0 && fd_cpu != 0;
}
