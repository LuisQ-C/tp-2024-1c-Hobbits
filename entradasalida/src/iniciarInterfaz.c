#include "../include/iniciarInterfaz.h"
#include "../include/interfazGenerica.h"
extern t_log* logger;
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