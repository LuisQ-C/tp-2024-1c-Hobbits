#include "../include/logs_obligatorios.h"

void logear_truncate(t_log* logger, int pid, char* nombre_archivo, int tamanio_truncar)
{
    log_info(logger,"DialFS - Truncar Archivo: \"PID: %d - Truncar Archivo: %s - Tamaño: %d\"",pid,nombre_archivo,tamanio_truncar);
}

void logear_creacion_archivo(t_log* logger, int pid, char* nombre_archivo)
{
    log_info(logger,"DialFS - Crear Archivo: \"PID: %d - Crear Archivo: %s\"",pid,nombre_archivo);
}

void logear_eliminacion_archivo(t_log* logger, int pid, char* nombre_archivo)
{
    log_info(logger,"DialFS - Eliminar Archivo: \"PID: %d - Eliminar Archivo: %s\"",pid,nombre_archivo);
}

void logear_lectura_archivo(t_log* logger, int pid, char* nombre_archivo, int tam_leer, int puntero_archivo)
{
    log_info(logger,"DialFS - Leer Archivo: \"PID: %d - Leer Archivo: %s - Tamaño a Leer: %d - Puntero Archivo: %d\"",pid,nombre_archivo,tam_leer,puntero_archivo);
}

void logear_escritura_archivo(t_log* logger, int pid, char* nombre_archivo, int tam_escribir, int puntero_archivo)
{
    log_info(logger,"DialFS - Escribir Archivo: \"PID: %d - Escribir Archivo: %s - Tamaño a Escribir: %d - Puntero Archivo: %d\"",pid,nombre_archivo,tam_escribir,puntero_archivo);
}

void logear_inicio_compactacion(t_log* logger, int pid)
{
     log_info(logger,"DialFS - Inicio Compactacion: \"PID: %d - Inicio Compactacion.\"",pid);
}

void logear_fin_compactacion(t_log* logger, int pid)
{
    log_info(logger,"DialFS - Fin Compactacion: \"PID: %d - Fin Compactacion.\"",pid);
}

void logear_operacion_dialFS(t_log* logger, int pid, int operacion_realizar)
{
    switch(operacion_realizar)
    {
        case IO_FS_CREATE:
        {
            log_info(logger,"DialFS - Operacion: \"PID: %d - Operacion: IO_FS_CREATE\"",pid);
            break;
        }
        case IO_FS_DELETE:
        {
            log_info(logger,"DialFS - Operacion: \"PID: %d - Operacion: IO_FS_DELETE\"",pid);
            break;
        }
        case IO_FS_TRUNCATE:
        {
            log_info(logger,"DialFS - Operacion: \"PID: %d - Operacion: IO_FS_TRUNCATE\"",pid);
            break;
        }
        case IO_FS_WRITE:
        {
            log_info(logger,"DialFS - Operacion: \"PID: %d - Operacion: IO_FS_WRITE\"",pid);
            break;
        }
        case IO_FS_READ:
        {
            log_info(logger,"DialFS - Operacion: \"PID: %d - Operacion: IO_FS_READ\"",pid);
            break;
        }
        default:
        {
            log_warning(logger,"OPERACION SOLICITADA AL dialFS DESCONOCIDA");
            break;
        }
    }
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
