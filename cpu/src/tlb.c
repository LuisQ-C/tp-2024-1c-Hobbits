#include "../include/tlb.h"

extern t_tlb tlb_scheduler;
extern t_config* config;
extern int INSTANTE_REFERENCIA;
//extern config_memoria config_mem;

void iniciar_tlb()
{
    tlb_scheduler.cant_entradas = config_get_int_value(config,"CANTIDAD_ENTRADAS_TLB");
    char* algoritmo = config_get_string_value(config,"ALGORITMO_TLB");
    if(string_equals_ignore_case(algoritmo,"FIFO")){
        tlb_scheduler.algoritmo_reemplazo = TLB_FIFO;
    }
    else{
        tlb_scheduler.algoritmo_reemplazo = TLB_LRU;
    }
    tlb_scheduler.tlb = queue_create();
}

void agregar_entrada_tlb(int pid,int pagina,int marco)
{
    (tlb_scheduler.algoritmo_reemplazo == TLB_FIFO) 
        ? agregar_entrada_tlb_fifo(pid,pagina,marco)
        : agregar_entrada_tlb_lru(pid,pagina,marco);
}

void agregar_entrada_tlb_fifo(int pid,int pagina,int marco)
{
    if(!hay_espacio_tlb())
    {
        quitar_tlb_fifo();
    }

    t_entrada_tlb* entrada = malloc(sizeof(t_entrada_tlb));
    entrada->pid = pid;
    entrada->pagina = pagina;
    entrada->marco = marco;
    
    queue_push(tlb_scheduler.tlb,entrada);
}

void  agregar_entrada_tlb_lru(int pid,int pagina,int marco)
{
    if(!hay_espacio_tlb())
    {
        quitar_tlb_lru();
    }

    t_entrada_tlb* entrada = malloc(sizeof(t_entrada_tlb));
    entrada->pid = pid;
    entrada->pagina = pagina;
    entrada->marco = marco;
    entrada->instante_referencia = INSTANTE_REFERENCIA;
    INSTANTE_REFERENCIA++;

    
    queue_push(tlb_scheduler.tlb,entrada);
}

bool hay_espacio_tlb()
{
    int entradas = list_size(tlb_scheduler.tlb->elements);
    return (entradas < tlb_scheduler.cant_entradas) ? true : false; // 32 harcorded tiene q ir datos_entradastlb
}

int buscar_entrada_tlb(int pid,int pagina)
{
    int _es_la_pagina(t_entrada_tlb* p)
    {
        int encontrado = (p->pid == pid) && (p->pagina == pagina);
        return encontrado;
    }

    t_entrada_tlb* auxiliar = list_find(tlb_scheduler.tlb->elements, (void*) _es_la_pagina);
    if(auxiliar == NULL)
    {
        return -1;
    }
    auxiliar->instante_referencia = INSTANTE_REFERENCIA;
    INSTANTE_REFERENCIA++;
    return auxiliar->marco;
}

/* LIBERAR ENTRADA OBTENIDA */ //CREO Q YA NO HACE FALTA
void quitar_entrada(int tipo_algoritmo)
{
    (tipo_algoritmo == TLB_FIFO) 
          ? quitar_tlb_fifo()
          : quitar_tlb_lru();
}

/* LIBERAR ENTRADA OBTENIDA */
void quitar_tlb_fifo()
{
    t_entrada_tlb* entrada_extraida = queue_pop(tlb_scheduler.tlb);
    free(entrada_extraida);
    
}

/* LIBERAR ENTRADA OBTENIDA */
void quitar_tlb_lru()
{

    int indice = buscar_instante_menor();
    t_entrada_tlb* entrada_extraida = list_remove(tlb_scheduler.tlb->elements,indice);
    free(entrada_extraida); 
}

int buscar_instante_menor()
{
    t_entrada_tlb* entrada_buscada;
    entrada_buscada = list_get(tlb_scheduler.tlb->elements,0);
    int tamanio_tlb = list_size(tlb_scheduler.tlb->elements);
    int indice_entrada = 0;
    for(int i=1 ;i<tamanio_tlb;i++)
    {
        t_entrada_tlb* aux = list_get(tlb_scheduler.tlb->elements,i);
        if(entrada_buscada->instante_referencia > aux->instante_referencia)
        {
            entrada_buscada = aux;
            indice_entrada = i;
        }
    }
    return indice_entrada;
}