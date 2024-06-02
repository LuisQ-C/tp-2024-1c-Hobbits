#ifndef LISTAS_H_
#define LISTAS_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <semaphore.h>
#include <pthread.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>

typedef struct{
    t_queue* cola;
    pthread_mutex_t* mutex;
} t_squeue;

t_squeue* squeue_create(void);
void squeue_destroy(t_squeue* queue);
void* squeue_pop(t_squeue* queue);
void squeue_push(t_squeue* queue, void* elemento);
void* squeue_peek(t_squeue* queue);
void squeue_iterate(t_squeue* squeue, void(*closure)(void*));
bool squeue_is_empty(t_squeue* squeue);
void* squeue_remove_by_condition(t_squeue* squeue, bool(*condition)(void*));
void* squeue_find(t_squeue* squeue, void(*closure)(void*));
bool squeue_any_satisfy(t_squeue* squeue, bool(*condition)(void*));

typedef struct{
    t_dictionary* diccionario;
    pthread_mutex_t* mutex;
} t_sdictionary;

t_sdictionary* sdictionary_create();
void* sdictionary_get(t_sdictionary* dicc, char* key);
void sdictionary_put(t_sdictionary* dicc, char* key, void* elemento);
void* sdictionary_remove(t_sdictionary* dicc, char* key);
void sdictionary_remove_and_destroy(t_sdictionary* dicc, char* key, void(*element_destroyer)(void*));
bool sdictionary_has_key(t_sdictionary* dicc, char* key);

#endif