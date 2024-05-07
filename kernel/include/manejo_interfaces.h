#ifndef KERNEL_MANEJO_INTERFACES_H_
#define KERNEL_MANEJO_INTERFACES_H_

#include <stdio.h>
#include <stdlib.h>
//#include "../../utils/include/sockets.h"
#include "../../utils/include/protocolo.h"
//#include "../../utils/include/logsConfigs.h"
//#include <pthread.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>

typedef struct 
{
    char* nombre;
    int tipo_interfaz;
    int fd_interfaz;
    t_queue* cola_procesos_blocked;
} t_list_io;

/*

*/

void agregar_interfaz_lista(char* nombre,int tipo, int fd_interfaz);

#endif