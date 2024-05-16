#ifndef KERNEL_MANEJO_INTERFACES_H_
#define KERNEL_MANEJO_INTERFACES_H_

#include <stdio.h>
#include <stdlib.h>
//#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
//#include "../../utils/include/logsConfigs.h"
#include <pthread.h>
#include <stdbool.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>


typedef struct
{
    t_list* lista;
    pthread_mutex_t* mutex;
}t_slist;

typedef struct 
{
    char* nombre_interfaz;
    int tipo_interfaz;
    int fd_interfaz;
    t_queue* cola_procesos_blocked;
} t_list_io;

typedef struct
{
    int tiempo;
    t_pcb* pcb;
} t_elemento_iogenerica;



void agregar_interfaz_lista(char* nombre,int tipo, int fd_interfaz);
t_slist* slist_create();
void slist_destroy(t_slist* slist);

bool slist_comprobate_io(char* nombreInterfaz,int operacion_solicitada);
bool admite_tipo(int operacion_solicitada,int tipo_interfaz);
bool existe_interfaz(char* nombre);
t_list_io* buscar_interfaz(char* nombre);
void* pop_elemento_cola_io(char* nombre);

#endif