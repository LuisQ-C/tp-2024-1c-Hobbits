#include "../include/iniciarInterfaz.h"
extern t_log* logger;

void iniciarInterfaz(char* nombreDeInterfaz,  t_config* config, int fd_conexion_kernel, int fd_conexion_memoria )// o hacerlo global? deberiamos agregar el fd de kernel, despues tambien habria que agregar el de la memoria
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
    mandarHandshake(logger,fd_conexion_memoria,"MODULO MEMORIA",tipoYnombreDeInterfaz);
    free(tipoYnombreDeInterfaz);

    if (!(strcmp(tipoDeInterfaz, "GENERICA")))
    {
        log_info(logger,"La interfaz recibida es: GENERICA");
        interfazGenerica(config, fd_conexion_kernel);
    }
    else
    {
        printf("\nERROR\n");
    }
}
void interfazGenerica(t_config* config, int fd_conexion_kernel)//pasa el config para no leer cada vez que inicia una interfaz todo completo
{
    int tiempoDeUnidadDeTrabajo;
    int cantidad_unidad_trabajo;
    tiempoDeUnidadDeTrabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    while(1)
    {
        recv(fd_conexion_kernel,&cantidad_unidad_trabajo,sizeof(int),MSG_WAITALL);
        int resultado = cantidad_unidad_trabajo * tiempoDeUnidadDeTrabajo; 
        usleep(resultado);
        int finalizado = INTERFAZ_LISTA; 
        log_info(logger,"Finalizo el tiempo en espera de la interfaz generica");
        send(fd_conexion_kernel,&finalizado,sizeof(int),0);
    }


}