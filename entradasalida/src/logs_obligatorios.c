#include "../include/logs_obligatorios.h"

void logear_truncate(t_log* logger, int pid, char* nombre_archivo, int tamanio_truncar)
{
    log_info(logger,"DialFS - Truncar Archivo: \"PID: %d - Truncar Archivo: %s - Tamaño: %d\"",pid,nombre_archivo,tamanio_truncar);
}
/*
void modificar_metadata(t_config* config_metadata, int tamanio, int bloque_inicial)
{   
    char* tamanio_nuevo = string_itoa(tamanio);
    char* bloque_inicial_nuevo = string_itoa(bloque_inicial);
    config_set_value(config_metadata,"TAMANIO",tamanio_nuevo);
    config_set_value(config_metadata,"BLOQUE_INICIAL",bloque_inicial_nuevo);
    free(tamanio_nuevo);
    free(bloque_inicial_nuevo);
}*/
