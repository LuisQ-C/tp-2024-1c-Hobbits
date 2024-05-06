#include "../include/archivos.h"

extern t_list* instrucciones_procesos;


/*Pasa todo el contenido del archivo a un char**, se debe destruirlo luego de utilizar la funcion*/
char** pasarArchivoEstructura(FILE* f)
{   
    char** archivoInstrucciones = string_array_new();
    
    char* instruccionLeida = string_new();
    //char* instruccion_perfecta = string_new();
    size_t longitud = 1;
    while(getline(&instruccionLeida, &longitud,f) != -1)
    {
        if(string_contains(instruccionLeida,"\n"))
        {
            /*instruccion_perfecta= string_substring(instruccionLeida,0,string_length(instruccionLeida)-1);
            string_array_push(&archivoInstrucciones,instruccion_perfecta);
            
            
            instruccionLeida = string_new();
            instruccion_perfecta = string_new();
            // PROBAR FREE ACA TMB
            longitud = 1;*/
            
            int posicion = string_length(instruccionLeida)-1;
            instruccionLeida[posicion] = '\0';
        }
        //else{
            string_array_push(&archivoInstrucciones,instruccionLeida);
            //free(instruccionLeida);
            instruccionLeida = string_new(); // ESTE CAPA NO TIENE Q IR
             // PROBAR FREE ACA TMB
            longitud = 1;
        //}
    }
    free(instruccionLeida);
    //free(instruccion_perfecta);
    return archivoInstrucciones;
}

/* Añade el pid  y las intrucciones del proceso a la lista instrucciones_procesos*/
void agregar_proceso_lista(int pid,FILE* f)
{
    t_proceso* proceso_creado = malloc(sizeof(t_proceso));
    proceso_creado->pid = pid;
    proceso_creado->instrucciones = pasarArchivoEstructura(f);
    list_add(instrucciones_procesos,proceso_creado);
}

/* Libera las estructuras asociadas al proceso */
void destruir_proceso_lista(t_proceso* proceso_a_destruir)
{
    string_array_destroy(proceso_a_destruir->instrucciones);
    free(proceso_a_destruir);
}

/* Quitar el proceso (pid) de la lista de procesos*/
void quitar_proceso_lista(int pid)
{
    t_proceso* proceso = buscar_proceso_pid(pid);
    list_remove_element(instrucciones_procesos,proceso); //devuelve 0 si no encuentra el elemento
    destruir_proceso_lista(proceso);
}

t_proceso* buscar_proceso_pid(int pid)
{
    int _es_el_proceso(t_proceso *p)
    {
        int encontrado = p->pid == pid;
        return encontrado;
    }
    return list_find(instrucciones_procesos, (void*) _es_el_proceso);
}




/*
bool buscar_proceso_pid(t_proceso* proceso)
{
    if(proceso-> pid == *pid)
    {
        return true;
    }
    return false;
}*/