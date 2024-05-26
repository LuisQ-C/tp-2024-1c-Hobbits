#ifndef RR_H_
#define RR_H_

#include "planificadorCP.h"

typedef struct
{
    int quantum;
    int pid;
}data;

void interrupcion_quantum(t_pcb* pcb_auxiliar);
void hilo_quantum(void* arg);

#endif