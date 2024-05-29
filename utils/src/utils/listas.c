#include "../include/listas.h"

t_squeue* squeue_create(){

    t_squeue* squeue = malloc(sizeof(t_squeue));

    squeue->mutex = malloc(sizeof(pthread_mutex_t));
    if (squeue->mutex == NULL) {
        perror("Error al asignar memoria para el mutex");
        free(squeue); // Liberar la memoria asignada previamente
        return NULL;
    }

    if(pthread_mutex_init(squeue->mutex, NULL) != 0){
        perror("No se puede iniciar el mutex");
        free(squeue->mutex);
    }
    squeue->cola = queue_create();

    return squeue;
}

void squeue_destroy(t_squeue* queue){
    pthread_mutex_destroy(queue->mutex);
    free(queue->mutex);
    queue_destroy(queue->cola);
    free(queue);
}

void* squeue_pop(t_squeue* queue){
    pthread_mutex_lock(queue->mutex);
    void* elemento = queue_pop(queue->cola);
    pthread_mutex_unlock(queue->mutex);
    
    return elemento;
}

void squeue_push(t_squeue* queue, void* elemento){
    pthread_mutex_lock(queue->mutex);
    queue_push(queue->cola, elemento);
    pthread_mutex_unlock(queue->mutex);
}

void* squeue_peek(t_squeue* queue){
    pthread_mutex_lock(queue->mutex);
    void* elemento = queue_peek(queue->cola);
    pthread_mutex_unlock(queue->mutex);

    return elemento;
}

void squeue_iterate(t_squeue* squeue, void(*closure)(void*)){
    pthread_mutex_lock(squeue->mutex);
    list_iterate(squeue->cola->elements, (void*) closure);
    pthread_mutex_unlock(squeue->mutex);
}

bool squeue_is_empty(t_squeue* squeue){
    bool isEmpty;
    pthread_mutex_lock(squeue->mutex);
    isEmpty = queue_is_empty(squeue->cola);
    pthread_mutex_unlock(squeue->mutex);

    return isEmpty;
}

void* squeue_remove_by_condition(t_squeue* squeue, bool(*condition)(void*)){
    void* elemento;
    pthread_mutex_lock(squeue->mutex);
    elemento = list_remove_by_condition(squeue->cola->elements, (void*) condition);
    pthread_mutex_unlock(squeue->mutex);
    return elemento;
}

void* squeue_find(t_squeue* squeue, void(*closure)(void*)){
    void* elemento;
    pthread_mutex_lock(squeue->mutex);
    elemento = list_find(squeue->cola->elements, (void*) closure);
    pthread_mutex_unlock(squeue->mutex);

    return elemento;
}

bool squeue_any_satisfy(t_squeue* squeue, bool(*condition)(void*)){
    bool found;
    pthread_mutex_lock(squeue->mutex);
    found = list_any_satisfy(squeue->cola->elements, (void*) condition);
    pthread_mutex_unlock(squeue->mutex);
    return found;
}