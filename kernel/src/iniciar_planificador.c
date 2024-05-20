#include "../include/iniciar_planificador.h"
#include "../include/main.h"

extern t_config* config;
t_squeue *lista_procesos_new;
t_squeue *lista_procesos_ready;
t_squeue *lista_procesos_exec;
t_squeue *lista_procesos_exit;
t_slist *lista_procesos_blocked;

sem_t grado_de_multiprogramacion;
sem_t proceso_en_cola_new;
sem_t proceso_en_cola_ready;
sem_t ejecutar_proceso;
sem_t pasar_a_ejecucion;

sem_t planificacion_new_iniciada;
sem_t planificacion_ready_iniciada;
sem_t planificacion_exec_iniciada;
sem_t planificacion_blocked_iniciada;

bool planificacion_iniciada = false;
//Hola, soy un comentario

void iniciar_cosas_necesarias_planificador(){

    int multiprog = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    //printf("multiprog, %d", multiprog);
    lista_procesos_new = squeue_create();
    lista_procesos_ready = squeue_create();
    lista_procesos_exec = squeue_create();
    lista_procesos_exit = squeue_create();
    lista_procesos_blocked = slist_create();
    sem_init(&pasar_a_ejecucion,0,1);
    sem_init(&proceso_en_cola_new, 0, 0);
    sem_init(&proceso_en_cola_ready, 0, 0);
    sem_init(&ejecutar_proceso, 0, 0);
    sem_init(&planificacion_new_iniciada, 0, 0);
    sem_init(&planificacion_ready_iniciada, 0, 0);
    sem_init(&planificacion_exec_iniciada, 0, 0);
    sem_init(&planificacion_blocked_iniciada,0,0);
    sem_init(&grado_de_multiprogramacion, 0, multiprog);
    iniciar_PLP();
    iniciar_PCP();

    //squeue_destroy(lista_procesos_new);
    //squeue_destroy(lista_procesos_ready);
    //sem_destroy(&un_semaforo); //remover luego

}