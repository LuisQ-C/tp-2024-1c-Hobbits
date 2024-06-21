#ifndef IO_COMPACTACION_H
#define IO_COMPACTACION_H

#include<stdio.h>
#include<stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"


void crear_archivo_metadatas(char* ruta_archivo_madre);

void agregar_nombre_metadatas(char* nombre, char* ruta_archivo_madre);

void eliminar_nombre_metadatas(char* nombre, char* ruta_archivo_madre, char* ruta_base);

#endif