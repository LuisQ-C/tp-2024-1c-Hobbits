#ifndef INICIAR_PLANIFICADOR_H_
#define INICIAR_PLANIFICADOR_H_

#include "../../utils/include/listas.h"
#include "planificadorLP.h"
#include "planificadorCP.h"
#include "manejo_recursos.h"
//asdasdasd

void iniciar_cosas_necesarias_planificador(void);
void iniciar_recursos();
t_recurso* crear_recurso(char* nombre, int cantInstancias);

#endif