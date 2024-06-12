#ifndef MEMORIA_CONFIG
#define MEMORIA_CONFIG

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/string.h>
typedef struct 
{
    char* puerto_escucha;
    int tam_memoria;
    int tam_pagina;
    char* path_instrucciones;
    int retardo_respuesta;
    int cantidad_marcos;
}t_datos_memoria;

void tomar_datos_config_y_destruir();

#endif