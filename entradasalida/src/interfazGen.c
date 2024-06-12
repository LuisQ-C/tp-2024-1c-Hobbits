#include "../include/interfazGen.h"

extern t_log* logger;

void interfazGenerica(t_config* config, int fd_conexion_kernel)//pasa el config para no leer cada vez que inicia una interfaz todo completo
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