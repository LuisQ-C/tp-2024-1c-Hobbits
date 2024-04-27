#ifndef MEMORIA_ARCHIVOS_H_
#define MEMORIA_ARCHIVOS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/string.h>
#include <commons/collections/list.h>

typedef struct 
{
    int pid;
    char** instrucciones;
} t_proceso;

char** pasarArchivoEstructura(char* path);
void agregar_proceso_lista(int pid,char* path);
void destruir_proceso_lista(t_proceso* proceso_a_destruir);
t_proceso* buscar_proceso_pid(int pid);
//bool _es_el_proceso(t_proceso *p);
void quitar_proceso_lista(int pid);
//bool buscar_proceso_pid(t_proceso* proceso,int pid);


#endif