#ifndef IO_LOGS_OBLIGATORIOS_H
#define IO_LOGS_OBLIGATORIOS_H

#include<stdio.h>
#include<stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"

/**LOGS_OBLIGATORIOS**/
void logear_truncate(t_log* logger, int pid, char* nombre_archivo, int tamanio_truncar);
void logear_creacion_archivo(t_log* logger, int pid, char* nombre_archivo);
void logear_eliminacion_archivo(t_log* logger, int pid, char* nombre_archivo);
void logear_lectura_archivo(t_log* logger, int pid, char* nombre_archivo, int tam_leer, int puntero_archivo);
void logear_escritura_archivo(t_log* logger, int pid, char* nombre_archivo, int tam_escribir, int puntero_archivo);
void logear_inicio_compactacion(t_log* logger, int pid);
void logear_fin_compactacion(t_log* logger, int pid);
void logear_operacion_dialFS(t_log* logger, int pid, int operacion_realizar);

/**METADATAS**/
//void modificar_metadata(t_config* config_metadata, int tamanio, int bloque_inicial);
#endif