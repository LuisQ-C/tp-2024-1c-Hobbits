#include "../include/interfazGen.h"

extern t_log* logger;

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
        int resultado = 1000 * cantidad_unidad_trabajo * tiempoDeUnidadDeTrabajo;
        log_info(logger,"Generica - Operación: \"PID: %d - Operacion: IO_GEN_SLEEP\"",pid);
        printf("\nTIEMPO dormir: %d \n",resultado);
        usleep(resultado);
        int finalizado = INTERFAZ_LISTA; 
        send(fd_conexion_kernel,&finalizado,sizeof(int),0);
    }


}