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
#include "logs_obligatorios.h"


void dialFS(t_config* config,int fd_kernel,int fd_memoria);
t_bitarray* cargar_bitmap_nuevo(FILE* arch_bitmap,int cant_bloques,char* ruta_bitmap);
t_bitarray* cargar_bitmap_existente(FILE* arch_bitmap,int cant_bloques, char* ruta_bitmap);
void imprimir_bitmap();

void* cargar_block_fs_existente(FILE* arch_bloques,int tam_block_fs, char* ruta_bloques);
void* cargar_block_fs_nuevo(FILE* arch_bloques,int tam_block_fs, char* ruta_bloques);


// Manejo bitmap
int espacio_libre_bitmap(int cant_bloques);
int espacio_contiguo_bitmap(int bloque_inicial,int tamanio, int tam_bloque, int cant_bloques);
int bloque_libre(int cant_bloques);
void asignar_bloque(int bloque_a_asignar);
void liberar_bloques(int bloque_inicial,int tamanio,int tam_bloque);
int calcular_bloques (int tamanio_config, int tam_bloque);
int calcular_puntero_inicial(int bloque_inicial, int tam_bloque);

void iniciar_compactacion(char* ruta_archivo_madre, char* ruta_base, int tam_bloque, int tam_block_fs, char* archivo_truncar);
void modificar_metadata(char* nombre_archivo,char* path_base,int nuevo_tamanio, int nuevo_bloque_inicial);
void memcpy_datos_origen_destino(int puntero_destino, int puntero_origen, int tamanio, int tamanio_block_fs);
void reasignar_bitmap(int bloque_inicial_nuevo, int bloque_inicial_anterior, int tam_bloque, int tamanio_config);
void asignar_bloques_adicionales(int primer_bloque, int cant_bloques_asignar);
void remover_archivo_truncar(t_list* lista, char* nombre_archivo);

#endif