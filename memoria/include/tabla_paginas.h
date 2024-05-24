#ifndef MEMORIA_TABLA_PAGINAS_H_
#define MEMORIA_TABLA_PAGINAS_H_

#include <stdlib.h>
#include <stdio.h>
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"

typedef struct 
{
    int marco;
    int validez_marco;
    
}t_tabla_pag;

t_tabla_pag* crear_tabla_paginas(int tam_memoria,int tam_pagina);
int consultar_marco(int num_pagina,t_tabla_pag* tabla_pag);

#endif