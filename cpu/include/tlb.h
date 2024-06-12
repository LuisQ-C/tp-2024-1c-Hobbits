#ifndef CPU_TLB_H_
#define CPU_TLB_H_

#include <stdlib.h>
#include <stdio.h>
#include "../../utils/include/protocolo.h"
#include "../../utils/include/logsConfigs.h"
#include <commons/collections/queue.h>
#include <stdbool.h>
#include "conexionesCPU.h"

typedef struct 
{
    int pid;
    int pagina;
    int marco;
    int instante_referencia;
}t_entrada_tlb;

typedef struct
{
    t_queue* tlb;
    int algoritmo_reemplazo;
    int cant_entradas;
}t_tlb;



void iniciar_tlb();


void agregar_entrada_tlb(int pid,int pagina,int marco);
void agregar_entrada_tlb_fifo(int pid,int pagina,int marco);
void agregar_entrada_tlb_lru(int pid,int pagina,int marco);

bool hay_espacio_tlb();

int buscar_entrada_tlb(int pid,int pagina);
void quitar_entrada(int tipo_algoritmo);
void quitar_tlb_fifo();
void quitar_tlb_lru();
int buscar_instante_menor();


#endif