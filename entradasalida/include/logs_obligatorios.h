#ifndef IO_LOGS_OBLIGATORIOS_H
#define IO_LOGS_OBLIGATORIOS_H

#include<stdio.h>
#include<stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"

/**LOGS_OBLIGATORIOS**/
void logear_truncate(t_log* logger, int pid, char* nombre_archivo, int tamanio_truncar);


/**METADATAS**/
//void modificar_metadata(t_config* config_metadata, int tamanio, int bloque_inicial);
#endif