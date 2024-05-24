#include "../include/user_space.h"

void* inicializar_user_space(t_config* config)
{
    int tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
    void* user_space = malloc(tam_memoria);
    return user_space;
}

void* inicializar_bitmap(t_config* config,void* user_space)
{
    int tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
    int tam_pagina = config_get_int_value(config,"TAM_PAGINA");

    int cant_marcos = tam_memoria / tam_pagina; 
    t_bitarray* bitmap = bitarray_create_with_mode(user_space,cant_marcos/8,LSB_FIRST);
    return bitmap;
}