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

    mandarHandshake(logger,fd_conexion_kernel,"MODULO KERNEL","INTERFAZ GENERICA");
    mandarHandshake(logger,fd_conexion_memoria,"MODULO MEMORIA","INTERFAZ GENERICA");

    /*
    INGRESE TIPO DE INTERFAZ A CREAR:
    GENERICA
    STDIN

    INGRESE NOMBRE: 
    */
    /*
    STRING TIPO = "GENERICA"
    
    */
    /*
    interfazGenerica()
    {
        int unidadesTrabajo;
        recv(fd_conexion_kernel,&unidadesTrabajo,sizeof(int),MSG_WAITALL);
        int resultado = tiempo_unidad_trabajo del config * unidadesTrabajo;
        usleep(resultado);
        int finalizado = INTERFAZ_LISTA;
        send(fd_conexion_kernel,&finalizado,sizeof(int),0);
    }
    interfaz stdin()
    {

    }
    */
    //manejarConexion(logger,&fd_conexion_kernel,&fd_conexion_memoria);
    terminar_programa(logger,config,&fd_conexion_kernel,&fd_conexion_memoria);
    
    return 0;
}


