#include "../include/manejo_interfaces.h"

extern t_list *lista_procesos_blocked;

void agregar_interfaz_lista(char* nombre,int tipo, int fd_interfaz)
{
    t_list_io* nueva_interfaz = malloc(sizeof(t_list_io));

    nueva_interfaz->nombre = nombre;
    nueva_interfaz->tipo_interfaz = tipo;
    nueva_interfaz->fd_interfaz = fd_interfaz;
    nueva_interfaz->cola_procesos_blocked = queue_create();

    list_add(lista_procesos_blocked,nueva_interfaz);

}