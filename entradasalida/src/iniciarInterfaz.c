#include "../include/iniciarInterfaz.h"
extern t_log* logger;

void iniciarInterfaz(char* nombreDeInterfaz,  t_config* config, int fd_conexion_kernel, int fd_conexion_memoria )// o hacerlo global? deberiamos agregar el fd de kernel, despues tambien habria que agregar el de la memoria
{
    char* tipoYnombreDeInterfaz = string_new();
    int resultado_handshake_kernel;
    int resultado_handshake_memoria;
    char* tipoDeInterfaz;
    //int* tiempoDeUnidadDeTrabajo;
    tipoDeInterfaz = config_get_string_value(config, "TIPO_INTERFAZ");
    //tiempoDeUnidadDeTrabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    string_append(&tipoYnombreDeInterfaz, tipoDeInterfaz);
    string_append(&tipoYnombreDeInterfaz, "-");
    string_append(&tipoYnombreDeInterfaz, nombreDeInterfaz);
    resultado_handshake_kernel = mandarHandshake(logger,fd_conexion_kernel,"MODULO KERNEL",tipoYnombreDeInterfaz);
    resultado_handshake_memoria = mandarHandshake(logger,fd_conexion_memoria,"MODULO MEMORIA",tipoYnombreDeInterfaz);

    if(resultado_handshake_kernel == HANDSHAKE_DENEGADO || resultado_handshake_memoria == HANDSHAKE_DENEGADO)
    {
        return;
    }

    free(tipoYnombreDeInterfaz);

    if (!(strcmp(tipoDeInterfaz, "GENERICA")))
    {
        log_info(logger,"La interfaz recibida es: GENERICA");
        interfazGenerica(config,fd_conexion_kernel);
    }
    else if (!(strcmp(tipoDeInterfaz, "STDIN")))
    {
        log_info(logger,"La interfaz recibida es: STDIN");
        stDin(config, fd_conexion_kernel,fd_conexion_memoria);
    }
    else  if (!(strcmp(tipoDeInterfaz, "STDOUT")))
    {
        log_info(logger,"La interfaz recibida es: STDOUT");
        stdOut(config, fd_conexion_kernel, fd_conexion_memoria);
    }
    else  if (!(strcmp(tipoDeInterfaz, "DIALFS")))
    {
        log_info(logger,"La interfaz recibida es: DIALFS");
        dialFS(config, fd_conexion_kernel, fd_conexion_memoria);
    }

    else
    {
        printf("\nERROR\n");
    }
}

/*
void interfazGenerica(t_config* config, int fd_conexion_kernel)//pasa el config para no leer cada vez que inicia una interfaz todo completo
{
    int tiempoDeUnidadDeTrabajo;
    int cantidad_unidad_trabajo;
    int pid;
    tiempoDeUnidadDeTrabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    while(1)
    {
        //recv(fd_conexion_kernel,&cantidad_unidad_trabajo,sizeof(int),MSG_WAITALL);
        recibir_operacion(fd_conexion_kernel);
        recibir_solicitud_io_generico(&pid,&cantidad_unidad_trabajo,fd_conexion_kernel);
        int resultado = cantidad_unidad_trabajo * tiempoDeUnidadDeTrabajo; 
        log_info(logger,"Generica - Operación: \"PID: %d - Operacion: IO_GEN_SLEEP\"",pid);
        usleep(resultado);
        int finalizado = INTERFAZ_LISTA; 
        send(fd_conexion_kernel,&finalizado,sizeof(int),0);
    }


}*/