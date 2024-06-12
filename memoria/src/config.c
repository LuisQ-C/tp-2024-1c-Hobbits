#include "../include/config.h"

extern t_config* config;
extern t_datos_memoria datos_memoria;

void tomar_datos_config_y_destruir()
{
    char* puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
    char* path_instrucciones = config_get_string_value(config,"PATH_INSTRUCCIONES");

    datos_memoria.puerto_escucha = string_duplicate(puerto_escucha);
    datos_memoria.path_instrucciones = string_duplicate(path_instrucciones);

    datos_memoria.tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
    datos_memoria.tam_pagina = config_get_int_value(config,"TAM_PAGINA");
    datos_memoria.retardo_respuesta = config_get_int_value(config,"RETARDO_RESPUESTA");
    
    int cant_marcos = datos_memoria.tam_memoria / datos_memoria.tam_pagina;
    datos_memoria.cantidad_marcos = cant_marcos;

    config_destroy(config);
}

