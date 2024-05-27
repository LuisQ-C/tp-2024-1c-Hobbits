#ifndef CONSOLA_H_
#define CONSOLA_H_

#include<commons/string.h>
#include<commons/config.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include "../../utils/include/protocolo.h"
#include "main.h"
#include "planificadorLP.h"
#include "planificadorCP.h"

void iniciar_consola(void* fd_info);
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

void cambiar_grado_de_multiprogramacion(int nuevo_grado_mp);
void bajar_grado();

void detener_cola_new(void* arg);
void detener_cola_ready(void* arg);
void detener_cola_exec(void* arg);
void detener_cola_blocked(void* arg);

//copio el logger y config del kernel


int asignar_pid();
t_registros_generales iniciar_registros_vacios();
t_pcb* crear_pcb();

#endif