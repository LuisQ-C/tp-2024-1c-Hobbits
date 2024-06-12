#ifndef MEMORIA_TABLA_PAGINAS_H_
#define MEMORIA_TABLA_PAGINAS_H_

#include <stdlib.h>
#include <stdio.h>
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"
#include <commons/bitarray.h>
#include <pthread.h>
#include "archivos.h"

typedef struct 
{
    int marco;
    
}t_entrada_pagina;


t_list* crear_tabla_paginas();
//void recepcionar_consulta_marcos(int fd_cpu);
int consultar_marco(int num_pagina,t_list* tabla_pag);
t_list* sbuscar_tabla_pid(int pid);
void destruir_tabla_paginas(t_list* tabla_paginas);
void liberar_frames_tabla(t_list* tabla_paginas);
void liberar_frame(t_entrada_pagina* entrada);

#endif