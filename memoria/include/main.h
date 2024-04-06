#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>

#include "../../utils/include/sockets.h"
#include "../../utils/include/logsConfigs.h"
#include <pthread.h>

//extern t_log* logger;

//extern t_log* logger;
void* atender_cliente(void*);
#endif