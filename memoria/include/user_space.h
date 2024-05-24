#ifndef MEMORIA_USER_SPACE_H_
#define MEMORIA_USER_SPACE_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/bitarray.h>
#include "../../utils/include/logsConfigs.h"

void* inicializar_user_space(t_config* config);
void* inicializar_bitmap(t_config* config,void* user_space);

#endif