#ifndef CONSOLA_H_
#define CONSOLA_H_

#include<commons/string.h>
#include<commons/config.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include "../../utils/include/protocolo.h"
//#include "../../utils/include/listas.h" //se aniadio solo para los pruebas
#include "main.h"
#include "planificadorLP.h"
#include "planificadorCP.h"
#include "manejar_fin.h"

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
void crear_proceso(void);

void cambiar_grado_de_multiprogramacion(int nuevo_grado_mp);
void bajar_grado();

void detener_cola_new(void* arg);
void detener_cola_ready(void* arg);
void detener_cola_exec(void* arg);
void detener_cola_blocked(void* arg);

void detener_cola_new_confirmacion(void* arg);
void detener_cola_ready_confirmacion(void* arg);
void detener_cola_exec_confirmacion(void* arg);
void detener_cola_blocked_confirmacion(void* arg);

//copio el logger y config del kernel
char* pids_blocked(t_list_io* interfaz);

int asignar_pid();
t_registros_generales iniciar_registros_vacios();
t_pcb* crear_pcb();

void set_plani_iniciada(bool valor);
bool get_plani_iniciada();
void buscar_proceso_finalizar(int pid);
void hilo_elimina_proceso(int* arg);
//bool _elemento_encontrado(t_pcb* pcb);
#endif