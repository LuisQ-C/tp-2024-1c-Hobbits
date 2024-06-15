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
//#include "planificadorCP.h"



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
    int cola_destino;
    int tiempo;
    t_pcb* pcb;
} t_elemento_iogenerica;

typedef struct
{
    int cola_destino;
    t_list* direcciones_fisicas;
    t_pcb* pcb;
} t_elemento_io_in_out;




t_list_io* agregar_interfaz_lista(char* nombre,int tipo, int fd_interfaz);
void quitar_interfaz_lista(t_list_io* interfaz);
void liberar_recursos_interfaz(t_list_io* interfaz);
t_slist* slist_create();
void slist_destroy(t_slist* slist);

bool sinterfaz_name_already_took(char* nombre_interfaz);
bool slist_comprobate_io(char* nombreInterfaz,int operacion_solicitada);
t_list_io* slist_buscar_interfaz(char* nombre);
bool admite_tipo(int operacion_solicitada,int tipo_interfaz);
bool existe_interfaz(char* nombre);
t_list_io* buscar_interfaz(char* nombre);
void* pop_elemento_cola_io(t_list_io* interfaz_lista);
void* peek_elemento_cola_io(t_list_io* interfaz_lista);
void push_elemento_cola_io(t_list_io* interfaz_lista,void* elemento_agregar);

bool cola_io_is_empty(t_list_io* interfaz_lista);
bool lista_interfaces_is_empty();
void cola_io_iterate(t_list_io* interfaz_lista, void(*closure)(void*));

bool slist_find_pcb_iterating_each_queue(t_slist* slist, int pid);
bool verificar_pid_de_solicitud(void* solicitud_io,int tipo_interfaz, int pid_buscado, int indice, t_list_iterator* bloqueados_interfaz);

#endif