#ifndef CONSOLA_H_
#define CONSOLA_H_

#include<commons/string.h>
#include<commons/config.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>

void iniciar_consola(void);
bool validar_instrucciones_leidas(char* leido);
void instrucciones_consola(char* leido);
char** custom_completion(const char* text, int start, int end);
char* custom_completion_generator(const char* text, int state);

void ejecutar_script(char* path);
void iniciar_proceso(char* path);
void finalizar_proceso(int pid);
void detener_planificacion(void);
void iniciar_planificacion(void);
void multiprogramacion(int valor);
void proceso_estado(void);

#endif