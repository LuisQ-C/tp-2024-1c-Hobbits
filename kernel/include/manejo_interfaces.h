#ifndef KERNEL_MANEJO_INTERFACES_H_
#define KERNEL_MANEJO_INTERFACES_H_

#include <stdio.h>
#include <stdlib.h>
//#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
//#include "../../utils/include/logsConfigs.h"
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include "../../utils/include/listas.h"


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
    sem_t* hay_proceso_cola;
    pthread_mutex_t* mutex_cola;
    t_queue* cola_procesos_blocked;
} t_list_io;

typedef struct
{
    int tiempo;
    t_pcb* pcb;
} t_elemento_iogenerica;



t_list_io* agregar_interfaz_lista(char* nombre,int tipo, int fd_interfaz);
void quitar_interfaz_lista(t_list_io* interfaz);
void liberar_recursos_interfaz(t_list_io* interfaz);
t_slist* slist_create();
void slist_destroy(t_slist* slist);

bool slist_comprobate_io(char* nombreInterfaz,int operacion_solicitada);
t_list_io* slist_buscar_interfaz(char* nombre);
bool admite_tipo(int operacion_solicitada,int tipo_interfaz);
bool existe_interfaz(char* nombre);
t_list_io* buscar_interfaz(char* nombre);
void* pop_elemento_cola_io(t_list_io* interfaz_lista);
void push_elemento_cola_io(t_list_io* interfaz_lista,void* elemento_agregar);

#endif