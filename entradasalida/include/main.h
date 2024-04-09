#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include <pthread.h>

void conectar_interfaz(t_log* logger, t_config* config);
#endif