#ifndef MEMORIA_USER_SPACE_H_
#define MEMORIA_USER_SPACE_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/bitarray.h>
#include "../../utils/include/logsConfigs.h"
#include "../../utils/include/protocolo.h"
#include "archivos.h"
#include <math.h>
#include "conexionesMemoria.h"
#include "tabla_paginas.h"
#include "bitmap.h"

void* inicializar_user_space();
void agregar_paginas_proceso(int pag_necesarias,t_list* tabla_paginas);
int calcular_pag_necesarias(int espacio_necesario,int tam_pagina);
int modificar_espacio_proceso(int pid,int tam,int tam_pagina);
void liberar_paginas_proceso(int pag_quitar,t_list* tabla_paginas);
void memcpy_pagina_frame(int dir_fisica,int inicio, void* data, int tam);


#endif