#include "../include/manejo_recursos.h"

extern t_log* logger;
extern t_log* logger_obligatorio;
extern t_config* config;


extern t_squeue *lista_procesos_new;
extern t_squeue *lista_procesos_ready;
extern t_squeue *lista_procesos_exec;
extern t_squeue *lista_procesos_exit;
extern t_slist *lista_procesos_blocked;
extern t_slist *lista_recursos_blocked;
extern t_sdictionary *instancias_utilizadas;

extern sem_t grado_de_multiprogramacion;
extern sem_t proceso_en_cola_new;
extern sem_t proceso_en_cola_ready;
extern sem_t ejecutar_proceso;
extern sem_t pasar_a_ejecucion;

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

void liberar_recursos(int pid){
    char* pid_c = string_itoa(pid);
    if(sdictionary_has_key(instancias_utilizadas, pid_c)){
        t_list* lista_rec = sdictionary_get(instancias_utilizadas, pid_c);

        void _liberar_recurso(t_instancias_usadas* iu){
            pthread_mutex_lock(lista_recursos_blocked->mutex);
            t_recurso* recurso_a_liberar = buscar_recurso(iu->nombre);
            pthread_mutex_unlock(lista_recursos_blocked->mutex);
            for (int i = 0; i < iu->cantInstanciasUtil; i++)
            {
                recurso_a_liberar->instancias_recurso++;
                //log_trace(logger, "%d CANT INSTNACIAS REC", recurso_a_liberar->instancias_recurso);
                if(!squeue_is_empty(recurso_a_liberar->cola_blocked)){
                    t_pcb* pcb_aux = squeue_pop(recurso_a_liberar->cola_blocked); //como se esta liberando una instancia tengo que sacar un pcb de bloqueado
                    pcb_aux->estado = READY;
                    mostrar_cola_ready();
                    log_info(logger_obligatorio, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY", pcb_aux->pid);
                    squeue_push(lista_procesos_ready, pcb_aux);
                    sem_post(&proceso_en_cola_ready);
                }
            }
            
        }

        list_iterate(lista_rec, (void*) _liberar_recurso);
        //aca deberia ir un sdicc_remove_and_destroy
    }
    free(pid_c);
}

bool lista_recursos_is_empty(){
    bool isEmpty;
    pthread_mutex_lock(lista_recursos_blocked->mutex);
    isEmpty = list_is_empty(lista_recursos_blocked->lista);
    pthread_mutex_unlock(lista_recursos_blocked->mutex);
    return isEmpty;
}

