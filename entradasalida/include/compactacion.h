#ifndef IO_COMPACTACION_H
#define IO_COMPACTACION_H

#include<stdio.h>
#include<stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"

typedef struct 
{
    char* nombre_metadata;
    int bloque_inicial;
    int tamanio;
}t_metadata;

void crear_archivo_metadatas(char* ruta_archivo_madre);
void agregar_nombre_metadatas(char* nombre, char* ruta_archivo_madre);
void eliminar_nombre_metadatas(char* nombre, char* ruta_archivo_madre, char* ruta_base);
t_list* obtener_nombres_metadatas(char* ruta_archivo_madre);
t_list* obtener_nombres_metadatas_con_atributos(char* ruta_archivo_madre, char* ruta_base);
void iterar_imprimir_atributos(t_list* lista_metadatas);
void imprimir_atributos(t_metadata* metadata);
void liberar_t_metadata(t_metadata* self);
void ordenar_lista_por_bloque_inicial(t_list* lista);
bool bloque_inicial_menor(t_metadata* actual, t_metadata* siguiente);


#endif