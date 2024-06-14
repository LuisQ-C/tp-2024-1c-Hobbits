#ifndef MANEJO_RECURSOS_H_
#define MANEJO_RECURSOS_H_

#include<commons/string.h>
#include<commons/config.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include "../../utils/include/protocolo.h"
#include "../../utils/include/listas.h"
#include "iniciar_planificador.h"
#include <stdbool.h>

typedef struct{
    char* nombre;
    int instancias_recurso;
    t_squeue* cola_blocked;
} t_recurso;

typedef struct{
    char* nombre;
    int cantInstanciasUtil;
} t_instancias_usadas;

void iniciar_recursos();
t_recurso* crear_recurso(char* nombre, int cantInstancias);
bool existe_recurso(char* nombre);
t_recurso* buscar_recurso(char* nombre);

void liberar_recursos(int pid);

#endif