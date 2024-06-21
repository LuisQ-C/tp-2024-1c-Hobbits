#ifndef INTERFAZ_DIALFS_H_
#define INTERFAZ_DIALFS_H_

#include<stdio.h>
#include<stdlib.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <math.h>
#include <readline/readline.h>
#include <commons/string.h>
#include "compactacion.h"


void dialFS(t_config* config,int fd_kernel,int fd_memoria);
t_bitarray* cargar_bitmap_nuevo(FILE* arch_bitmap,int cant_bloques);
t_bitarray* cargar_bitmap_existente(FILE* arch_bitmap,int cant_bloques);
void imprimir_bitmap();

void* cargar_block_fs_existente(FILE* arch_bloques,int tam_block_fs);
void* cargar_block_fs_nuevo(FILE* arch_bloques,int tam_block_fs);


// Manejo bitmap
int espacio_libre_bitmap(int cant_bloques);
int espacio_contiguo_bitmap(int bloque_inicial,int tamanio, int tam_bloque);
int bloque_libre(int cant_bloques);
void asignar_bloque(int bloque_a_asignar);
void liberar_bloques(int bloque_inicial,int tamanio,int tam_bloque);

#endif