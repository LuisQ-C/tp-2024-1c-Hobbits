#ifndef MEMORIA_ARCHIVOS_H_
#define MEMORIA_ARCHIVOS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "tabla_paginas.h"

typedef struct 
{
    int pid;
    char** instrucciones;
    t_list* tabla_paginas;
    // agregar tabla pag
} t_proceso;

char** pasarArchivoEstructura(FILE* f);
void agregar_proceso_lista(int pid,FILE* f);
void destruir_proceso_lista(t_proceso* proceso_a_destruir);
t_proceso* s_buscar_proceso_pid(int pid);
t_proceso* buscar_proceso_pid(int pid);
//bool _es_el_proceso(t_proceso *p);
int quitar_proceso_lista(int pid);
//bool buscar_proceso_pid(t_proceso* proceso,int pid);


#endif