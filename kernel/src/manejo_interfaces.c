#include "../include/manejo_interfaces.h"

extern t_slist *lista_procesos_blocked;

void agregar_interfaz_lista(char* nombre,int tipo, int fd_interfaz)
{
    t_list_io* nueva_interfaz = malloc(sizeof(t_list_io));

    nueva_interfaz->nombre_interfaz = nombre;
    nueva_interfaz->tipo_interfaz = tipo;
    nueva_interfaz->fd_interfaz = fd_interfaz;
    nueva_interfaz->cola_procesos_blocked = queue_create();

    list_add(lista_procesos_blocked->lista,nueva_interfaz);

}

t_slist* slist_create(){

    t_slist* slist = malloc(sizeof(t_slist));

    slist->mutex = malloc(sizeof(pthread_mutex_t));
    if (slist->mutex == NULL) {
        perror("Error al asignar memoria para el mutex");
        free(slist); // Liberar la memoria asignada previamente
        return NULL;
    }

    if(pthread_mutex_init(slist->mutex, NULL) != 0){
        perror("No se puede iniciar el mutex");
        free(slist->mutex);
    }

    slist->lista = list_create();

    return slist;
}

void slist_destroy(t_slist* slist){
    pthread_mutex_destroy(slist->mutex);
    free(slist->mutex);
    list_destroy(slist->lista);
    free(slist);
}

bool slist_comprobate_io(char* nombreInterfaz,int operacion_solicitada)
{
    pthread_mutex_lock(lista_procesos_blocked->mutex);
    if(existe_interfaz(nombreInterfaz))
    {
        t_list_io* interfaz_encontrada = buscar_interfaz(nombreInterfaz);
        bool admiteTipo = admite_tipo(operacion_solicitada,interfaz_encontrada->tipo_interfaz);
        pthread_mutex_unlock(lista_procesos_blocked->mutex);
        if(admiteTipo)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else{
        pthread_mutex_unlock(lista_procesos_blocked->mutex);
        return false;
    }

}
bool admite_tipo(int operacion_solicitada,int tipo_interfaz)
{
    //HACER QUE SI ES IO GEN SLEEP el operacion_solicitad, SE COMPARE el tipo IO_GEN CON tipo_interfaz
    return operacion_solicitada == tipo_interfaz; //PARA EL FS TIPO_INTERFAZ DEBE SER UNA LISTA
}

bool existe_interfaz(char* nombre)
{
    bool _es_la_interfaz(t_list_io* p)
    {
        bool encontrado = strcmp(p->nombre_interfaz,nombre) == 0;
        return encontrado;
    }
    return list_any_satisfy(lista_procesos_blocked->lista, (void*) _es_la_interfaz);
}

t_list_io* buscar_interfaz(char* nombre)
{
    bool _es_la_interfaz(t_list_io* p)
    {
        bool encontrado = strcmp(p->nombre_interfaz,nombre) == 0;
        return encontrado;
    }
    return list_find(lista_procesos_blocked->lista, (void*) _es_la_interfaz);
}

//ACCEDE CADA IO A SU COLA Y SACA EL PRIMER ELEMENTO PARA HACER SEND Y LUEGO DEL RECV ENVIARLO A READY

void* pop_elemento_cola_io(char* nombre)
{
    pthread_mutex_lock(lista_procesos_blocked->mutex);
    t_list_io* interfaz_buscada = buscar_interfaz(nombre);
    void* elemento_lista = queue_pop(interfaz_buscada->cola_procesos_blocked);
    pthread_mutex_unlock(lista_procesos_blocked->mutex);
    return elemento_lista;
}

//