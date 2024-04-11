#ifndef UTILS_LOGSCONFIGS_H
#define UTILS_LOGSCONFIGS_H

#include <stdlib.h>
#include <stdio.h>
#include<commons/log.h>
#include<commons/config.h>
/*
#include<string.h>
#include<assert.h>
#include <signal.h>
*/

t_log* iniciar_logger(char*,char* ,int ,t_log_level );
t_config* iniciar_config(char*,t_log*);
void destruir_log_config(t_log* logger,t_config* config);
#endif