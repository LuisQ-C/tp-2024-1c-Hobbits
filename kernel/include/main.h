#ifndef  MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include "../../utils/include/sockets.h"
//#include<commons/log.h> //utils ya tiene un include de log
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include"../include/conexiones.h"

t_log* iniciar_logger(void);
t_config* iniciar_config(t_log*);

#endif