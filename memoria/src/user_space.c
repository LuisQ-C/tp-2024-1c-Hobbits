#include "../include/user_space.h"

extern t_log* logger;
extern t_log* logger_obligatorio;
extern void* user_space;
extern t_bitarray* bitmap;
extern t_list* instrucciones_procesos;
extern t_datos_memoria datos_memoria;
extern pthread_mutex_t mutex_lista_procesos;
extern pthread_mutex_t mutex_user_space;
extern pthread_mutex_t mutex_bitmap;

void* inicializar_user_space()
{
    int tam_memoria = datos_memoria.tam_memoria;
    void* user_space = malloc(tam_memoria);
    return user_space;
}

void memcpy_pagina_frame(int dir_fisica,int inicio, void* data, int tam)
{
    pthread_mutex_lock(&mutex_user_space);
    memcpy(user_space+dir_fisica,data+inicio,tam);
    pthread_mutex_unlock(&mutex_user_space);
}

int modificar_espacio_proceso(int pid,int tam,int tam_pagina)
{
    
    t_list* tabla_paginas = sbuscar_tabla_pid(pid); //BUSCAMOS LA TP CORRESPONDIENTE AL PROCESO

    int tam_proceso = list_size(tabla_paginas) * tam_pagina;  //OBTENEMOS EL TAMANIO PROCESO CANT_PAG*TAM_PAG
    int espacio_necesario = tam - tam_proceso;                //OBTENEMOS EL ESPACIO NECESARIO (PUEDE DAR NEGATIVO)
    int pag_necesarias = calcular_pag_necesarias(espacio_necesario,tam_pagina);//CALCULA PAGINAS A QUITAR O AGREGAR
    int valor_abs_mod = fabs(espacio_necesario);

    if(tam <= tam_proceso && tam >= tam_proceso-tam_pagina)
    {
        printf("\n YA CONTAS CON ESA CANTIDAD DE ESPACIO \n");
        return MODIFICACION_NULA;
    }
    else
    {
        if(espacio_necesario > 0)
        {   
            if(espacio_libre_bitmap()<pag_necesarias)
            {
                return OUT_OF_MEMORY;
            }
            else
            {
                agregar_paginas_proceso(pag_necesarias,tabla_paginas);
                log_info(logger_obligatorio,"Ampliacion de Proceso:\"PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d\"",pid,tam_proceso,valor_abs_mod);
                return PROCESO_AMPLIADO;
            }
        }
        else
        {
            liberar_paginas_proceso(pag_necesarias,tabla_paginas);
            log_info(logger_obligatorio,"Reduccion de Proceso:\"PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d\"",pid,tam_proceso,valor_abs_mod);
            return PROCESO_REDUCIDO;
        }
    }
}

/**********************
AGREGAR PAGINAS PROCESO

>POR CADA PAGINA QUE SE DESEE AGREGAR SE CREAR UN t_entrada_pagina Y SE LE ASIGNARA EL PRIMER MARCO LIBRE QUE SE ENCUENTRE
>SE ANIADIRA A LA TP DE ESE PROCESO
>MARCAMOS EL MARCO SELECCIONADO COMO "OCUPADO"

**************************/
void agregar_paginas_proceso(int pag_agregar,t_list* tabla_paginas)
{
    t_entrada_pagina* pagina_a_aniadir; 
    for(int i = 0; i<pag_agregar;i++)
    {
        pagina_a_aniadir = malloc(sizeof(t_entrada_pagina)); 
        asignar_marco(pagina_a_aniadir);
        //printf("\nMARCO ASIGNADO: %d\n",pagina_a_aniadir->marco); PRUEBAS
        list_add(tabla_paginas,pagina_a_aniadir);
    }
}

/**********************
LIBERAR PAGINAS PROCESO

>SE LIBERAN LAS PAGINAS NECESARIAS 
>MARCAMOS EL MARCO QUE YA NO SE UTILIZA COMO "LIBRE"
>NO SE SOBREESCRIBEN LOS DATOS EN MEMORIA

**************************/
void liberar_paginas_proceso(int pag_quitar,t_list* tabla_paginas)
{
    int tamanio = list_size(tabla_paginas);
    for(int i=0; i<pag_quitar;i++)
    {
        t_entrada_pagina* auxiliar = list_get(tabla_paginas,tamanio-1);
        pthread_mutex_lock(&mutex_bitmap);
        liberar_frame(auxiliar);
        pthread_mutex_unlock(&mutex_bitmap);
        list_remove_element(tabla_paginas,auxiliar);
        free(auxiliar);
        tamanio--;
    }
}

int calcular_pag_necesarias(int espacio_necesario,int tam_pagina)
{
    double dtam_pagina = tam_pagina;
    int paginas_necesarias;
    if(espacio_necesario<0)
    {
        paginas_necesarias = fabs(ceil(espacio_necesario/dtam_pagina));
    }
    else 
    {
        paginas_necesarias = ceil(espacio_necesario/dtam_pagina); 
    }
    return paginas_necesarias;
}






