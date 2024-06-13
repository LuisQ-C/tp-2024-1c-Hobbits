#include "../include/manejo_recursos.h"

extern t_log* logger;
extern t_config* config;


extern t_squeue *lista_procesos_new;
extern t_squeue *lista_procesos_ready;
extern t_squeue *lista_procesos_exec;
extern t_squeue *lista_procesos_exit;
extern t_slist *lista_procesos_blocked;
extern t_slist *lista_recursos_blocked;
extern t_sdictionary *instancias_utilizadas;

void iniciar_recursos(){
    char** lista_recursos = config_get_array_value(config, "RECURSOS");
    char** instancia_por_recurso = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    for (int i = 0; instancia_por_recurso[i] != NULL; i++)
    {
        t_recurso* nuevo_recurso = crear_recurso(lista_recursos[i], atoi(instancia_por_recurso[i]));
        slist_add(lista_recursos_blocked, nuevo_recurso);
    }

    string_array_destroy(lista_recursos);
    string_array_destroy(instancia_por_recurso);

}

t_recurso* crear_recurso(char* nombre, int cantInstancias){
    t_recurso* recurso = malloc(sizeof(t_recurso));
    recurso->nombre = string_new();
    string_append(&recurso->nombre, nombre);
    recurso->instancias_recurso = cantInstancias;
    recurso->cola_blocked = squeue_create();

    return recurso;
}

bool existe_recurso(char* nombre){
    bool _es_el_recurso(t_recurso* r)
    {
        bool encontrado = strcmp(r->nombre, nombre) == 0;
        return encontrado;
    }
    return list_any_satisfy(lista_recursos_blocked->lista, (void*) _es_el_recurso);
}

t_recurso* buscar_recurso(char* nombre){
    bool _es_el_recurso(t_recurso* r){
        bool encontrado = strcmp(r->nombre, nombre) == 0;
        return encontrado;
    }
    return list_find(lista_recursos_blocked->lista, (void*) _es_el_recurso);
}