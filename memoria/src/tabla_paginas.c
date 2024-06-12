#include "../include/tabla_paginas.h"

extern t_log* logger;
extern t_list* instrucciones_procesos;
extern pthread_mutex_t mutex_lista_procesos;
extern t_bitarray* bitmap;
extern pthread_mutex_t mutex_bitmap;


t_list* crear_tabla_paginas()
{
    t_list* lista = list_create();
    return lista;
}

int consultar_marco(int num_pagina,t_list* tabla_pag)
{
    t_entrada_pagina* entrada = list_get(tabla_pag,num_pagina); // MUTEAR TP?
    if(entrada != NULL)
    {
        return entrada->marco;
    }
    log_error(logger,"NO TE PERTENECE FLACO");
    return MARCO_INVALIDO;
}

t_list* sbuscar_tabla_pid(int pid)
{
    int _es_el_proceso(t_proceso *p)
    {
        int encontrado = p->pid == pid;
        return encontrado;
    }

    pthread_mutex_lock(&mutex_lista_procesos);
    t_proceso* auxiliar = list_find(instrucciones_procesos, (void*) _es_el_proceso);
    pthread_mutex_unlock(&mutex_lista_procesos);
    return auxiliar->tabla_paginas;
}

void liberar_frames_tabla(t_list* tabla_paginas)
{
    pthread_mutex_lock(&mutex_bitmap);
    list_iterate(tabla_paginas,(void*) liberar_frame);
    pthread_mutex_unlock(&mutex_bitmap);
}

void liberar_frame(t_entrada_pagina* entrada)
{
    bitarray_clean_bit(bitmap,entrada->marco);
}

void destruir_tabla_paginas(t_list* tabla_paginas)
{
    list_destroy_and_destroy_elements(tabla_paginas,(void*) liberar_elemento);
}