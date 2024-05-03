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

void iniciarInterfaz(char* nombreDeInterfaz,  t_config* config, int fd_conexion_kernel )// o hacerlo global? deberiamos agregar el fd de kernel, despues tambien habria que agregar el de la memoria
{
    char* tipoYnombreDeInterfaz = string_new();

    char* tipoDeInterfaz;
    //int* tiempoDeUnidadDeTrabajo;
    tipoDeInterfaz = config_get_string_value(config, "TIPO_INTERFAZ");
    //tiempoDeUnidadDeTrabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    string_append(&tipoYnombreDeInterfaz, tipoDeInterfaz);
    string_append(&tipoYnombreDeInterfaz, "-");
    string_append(&tipoYnombreDeInterfaz, nombreDeInterfaz);
    mandarHandshake(logger,fd_conexion_kernel,"MODULO KERNEL",tipoYnombreDeInterfaz);
    free(tipoYnombreDeInterfaz);
    //mandarHandshake(logger,fd_conexion_memoria,"MODULO MEMORIA","INTERFAZ CUALQUIERA");
        if (!(strcmp(tipoDeInterfaz, "GENERICO")))
            {
                log_info(logger,"La interfaz recibida es: GENERICA");
                interfazGenerica(config, fd_conexion_kernel);
            }
}

void interfazGenerica( t_config* config, int fd_conexion_kernel)//pasa el config para no leer cada vez que inicia una interfaz todo completo
{
    int tiempoDeUnidadDeTrabajo;
    int cantidad_unidad_trabajo;
    tiempoDeUnidadDeTrabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    while(1)
        {
    recv(fd_conexion_kernel,&cantidad_unidad_trabajo,sizeof(int),MSG_WAITALL);
        int resultado = cantidad_unidad_trabajo * tiempoDeUnidadDeTrabajo; //cantidad viene de kernel
        usleep(resultado);
        int finalizado = 4; // aca deberia ir el valor del ENUM INTERFAZLISTA
        log_info(logger,"Finalizo el tiempo en espera de la interfaz generica");
        send(fd_conexion_kernel,&finalizado,sizeof(int),0);
        }


}


