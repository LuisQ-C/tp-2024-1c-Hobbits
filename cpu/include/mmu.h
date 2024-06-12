#ifndef CPU_MMU_H_
#define CPU_MMU_H_


#include <stdlib.h>
#include <stdio.h>
#include "../../utils/include/protocolo.h"
#include "../../utils/include/logsConfigs.h"
#include <math.h>
#include <stdbool.h>
#include "conexionesCPU.h"
#include "tlb.h"


enum {
    ALCANZA_UNA_PAGINA = 200,
    NO_ALCANZA_UNA_PAGINA
};



int traducir_direccion_pagina(int direccion_logica);
int traducir_direccion_desplazamiento(int direccion_logica,int numero_pagina);


t_list* crear_paquete_escritura(int direccion_logica, int tam_dato,t_list* lista_frames, int cant_pag);
void avanzar_base_restante(int* base, int* restante, int tamanio);

void armar_solicitud_frames(int pagina_inicial, int paginas_totales, int fd_memoria, int pid);

int paginas_necesarias(int desplazamiento, int tamanio_dato);

int alcanza_una_pagina(int espacio_disponible, int tamanio_dato);

int cant_paginas_necesarias(int tamanio);

void asignar(t_porcion_dato* porcion_asignar,int base,int tamanio, int direccion_fisica);

int calcular_direccion_fisica(int nro_frame, int desplazamiento);

int tam_registro(char* registro_chequear);

//SOLICITUD MARCOS

t_list* solicitar_macros(int pagina_inicial, int paginas_totales, int pid, int fd_destinatario);

t_list* solicitar_marcos_sin_tlb(int pagina_inicial, int paginas_totales, int pid, int fd_destinatario);

t_list* solicitar_marcos_con_tlb(int pagina_inicial, int paginas_totales,int pid, int fd_destinatario);

void logear_consulta_marco(int pid, int pagina, int marco);

void logear_lectura_string(int pid, int dir_fis,char* valor_leido);

void logear_lectura_int(int pid, int dir_fis,int valor_leido);

void logear_escritura_string(int pid, int dir_fis,char* valor_escrito);

void logear_escritura_int(int pid, int dir_fis,int valor_escrito);

#endif