#include "../include/manejo_interfaces.h"

extern t_slist *lista_procesos_blocked;

t_list_io* agregar_interfaz_lista(char* nombre,int tipo, int fd_interfaz)
{
    t_list_io* nueva_interfaz = malloc(sizeof(t_list_io));

    nueva_interfaz->nombre_interfaz = string_new();
    string_append(&nueva_interfaz->nombre_interfaz,nombre);
    nueva_interfaz->tipo_interfaz = tipo;
    nueva_interfaz->fd_interfaz = fd_interfaz;
    nueva_interfaz->mutex_cola = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(nueva_interfaz->mutex_cola,NULL);
    nueva_interfaz->hay_proceso_cola = malloc(sizeof(sem_t));
    sem_init(nueva_interfaz->hay_proceso_cola,0,0);
    nueva_interfaz->cola_procesos_blocked = queue_create();

    pthread_mutex_lock(lista_procesos_blocked->mutex);
    list_add(lista_procesos_blocked->lista,nueva_interfaz);
    pthread_mutex_unlock(lista_procesos_blocked->mutex);

    return nueva_interfaz;

}
/**************
QUITAR INTERFAZ DE LA LISTA

>LO REMOVEMOS DE LA LISTA CON list_remove_element()
>LIBERAMOS LOS RECURSOS DE ESE ELEMENTO CON liberar_recursos_interfaz()

********************/
void quitar_interfaz_lista(t_list_io* interfaz)
{
    pthread_mutex_lock(lista_procesos_blocked->mutex);
    list_remove_element(lista_procesos_blocked->lista,interfaz);
    pthread_mutex_unlock(lista_procesos_blocked->mutex);
    liberar_recursos_interfaz(interfaz);
}
void liberar_recursos_interfaz(t_list_io* interfaz)
{
    free(interfaz->nombre_interfaz);
    pthread_mutex_destroy(interfaz->mutex_cola);
    free(interfaz->mutex_cola);
    sem_destroy(interfaz->hay_proceso_cola);
    free(interfaz->hay_proceso_cola);
}


/**********************
 
 CREAR SLIST (LISTA CON MUTEX)

**********************/
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

/**********************
 
 DESTRUIR SLIST (LISTA CON MUTEX)

**********************/
void slist_destroy(t_slist* slist){
    pthread_mutex_destroy(slist->mutex);
    free(slist->mutex);
    list_destroy(slist->lista);
    free(slist);
}


bool sinterfaz_name_already_took(char* nombre_interfaz)
{
    pthread_mutex_lock(lista_procesos_blocked->mutex);
    bool nombre_tomado = existe_interfaz(nombre_interfaz);
    pthread_mutex_unlock(lista_procesos_blocked->mutex);
    
    return nombre_tomado;
}

bool lista_interfaces_is_empty(){
    bool isEmpty;
    pthread_mutex_lock(lista_procesos_blocked->mutex);
    isEmpty = list_is_empty(lista_procesos_blocked->lista);
    pthread_mutex_unlock(lista_procesos_blocked->mutex);
    return isEmpty;
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
            printf("\ntipo admitido\n");
            return true;
        }
        else
        {
            printf("\ntipo inadmitido\n");
            return false;
        }
    }
    else{
        pthread_mutex_unlock(lista_procesos_blocked->mutex);
        printf("\nno existe la interfaz flaco anda programa\n");
        return false;
    }

}
t_list_io* slist_buscar_interfaz(char* nombre)
{
    pthread_mutex_lock(lista_procesos_blocked->mutex);
    t_list_io* encontrada = buscar_interfaz(nombre);
    pthread_mutex_unlock(lista_procesos_blocked->mutex);
    return encontrada;
}
bool admite_tipo(int operacion_solicitada,int tipo_interfaz)
{
    //HACER QUE SI ES IO GEN SLEEP el operacion_solicitad, SE COMPARE el tipo IO_GEN CON tipo_interfaz
    printf("OPERACION SOLICITDA: %d",operacion_solicitada);
    printf("TIPO INTERFAZ: %d",tipo_interfaz);
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

void* pop_elemento_cola_io(t_list_io* interfaz_lista)
{
    pthread_mutex_lock(interfaz_lista->mutex_cola);
    void* solicitud_io = queue_pop(interfaz_lista->cola_procesos_blocked);
    pthread_mutex_unlock(interfaz_lista->mutex_cola);
    return solicitud_io;
}

void* peek_elemento_cola_io(t_list_io* interfaz_lista)
{
    pthread_mutex_lock(interfaz_lista->mutex_cola);
    void* solicitud_io = queue_peek(interfaz_lista->cola_procesos_blocked);
    pthread_mutex_unlock(interfaz_lista->mutex_cola);
    return solicitud_io;
}

void push_elemento_cola_io(t_list_io* interfaz_lista,void* elemento_agregar)
{
    pthread_mutex_lock(interfaz_lista->mutex_cola);
    queue_push(interfaz_lista->cola_procesos_blocked,elemento_agregar);
    pthread_mutex_unlock(interfaz_lista->mutex_cola);
}

void cola_io_iterate(t_list_io* interfaz_lista, void(*closure)(void*)){
    pthread_mutex_lock(interfaz_lista->mutex_cola);
    list_iterate(interfaz_lista->cola_procesos_blocked->elements, (void*) closure);
    pthread_mutex_unlock(interfaz_lista->mutex_cola);
}

bool cola_io_is_empty(t_list_io* interfaz_lista){
    bool isEmpty;
    pthread_mutex_lock(interfaz_lista->mutex_cola);
    isEmpty = queue_is_empty(interfaz_lista->cola_procesos_blocked);
    pthread_mutex_unlock(interfaz_lista->mutex_cola);
    return isEmpty;
}

//