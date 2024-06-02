#ifndef MANEJO_RECURSOS_H_
#define MANEJO_RECURSOS_H_

#include<commons/string.h>
#include<commons/config.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include "../../utils/include/protocolo.h"
#include "../../utils/include/listas.h"
#include <stdbool.h>

typedef struct{
    char* nombre;
    int instancias_recurso;
    t_squeue* cola_blocked;
} t_recurso;

#endif