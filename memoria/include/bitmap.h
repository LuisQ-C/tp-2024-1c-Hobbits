#ifndef MEMORIA_BITMAP_H_
#define MEMORIA_BITMAP_H_

#include <stdlib.h>
#include <stdio.h>
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"
#include <commons/bitarray.h>
#include <pthread.h>
#include "archivos.h"
#include <math.h>
#include "config.h"

t_bitarray* crear_bitmap();
void inicializar_bitmap(t_bitarray* nuevo_bitmap);
int espacio_libre_bitmap();
int marco_libre();
void asignar_marco(t_entrada_pagina* pagina_a_aniadir);

#endif