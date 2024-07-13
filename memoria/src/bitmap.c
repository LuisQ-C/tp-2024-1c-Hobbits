#include "../include/bitmap.h"

extern void* user_space;
extern t_bitarray* bitmap;
extern t_datos_memoria datos_memoria;
extern pthread_mutex_t mutex_bitmap;

t_bitarray* crear_bitmap()
{
    int tam_memoria = datos_memoria.tam_memoria;
    int tam_pagina = datos_memoria.tam_pagina;
    
    int cant_marcos = tam_memoria / tam_pagina;
    int tamanio_bitmap = ceil(cant_marcos/8.00);
    void* bitmap_user_space = malloc(tamanio_bitmap); //NO SE SI VA ACA EL VOID*
    
    t_bitarray* nuevo_bitmap = bitarray_create_with_mode(bitmap_user_space,tamanio_bitmap,LSB_FIRST);
    inicializar_bitmap(nuevo_bitmap);
    return nuevo_bitmap;
}


void inicializar_bitmap(t_bitarray* nuevo_bitmap)
{
    int tam = bitarray_get_max_bit(nuevo_bitmap);
    //printf("\nTAMANIO BITMAP: %d\n",tam);
    for(int i=0;i<tam;i++)
    {   
        bitarray_clean_bit(nuevo_bitmap,i);      
    }  
    
}

int espacio_libre_bitmap()
{
    int marcos_libres = 0;
    int valor;
    pthread_mutex_lock(&mutex_bitmap);
    for(int i=0;i<datos_memoria.cantidad_marcos;i++)
    {
        valor = bitarray_test_bit(bitmap,i);
        if(valor==0)
            marcos_libres++;
    }
    pthread_mutex_unlock(&mutex_bitmap);
    return marcos_libres;
}

/******************
MARCO LIBRE

>DEVUELVE EL PRIMER MARCO LIBRE QUE ENCUENTRA

**********************/
int marco_libre()
{
    int valor;
    int marco_libre=-1;
    //pthread_mutex_lock(&mutex_bitmap);
    for(int i=0;i<datos_memoria.cantidad_marcos;i++)
    {
        valor = bitarray_test_bit(bitmap,i);
        if(valor==0)
        {
            marco_libre = i;
            break;
        }
    } 
    //pthread_mutex_unlock(&mutex_bitmap);
    return marco_libre;
}

void asignar_marco(t_entrada_pagina* pagina_a_aniadir)
{
    pthread_mutex_lock(&mutex_bitmap);
    pagina_a_aniadir->marco = marco_libre();
    bitarray_set_bit(bitmap,pagina_a_aniadir->marco);
    pthread_mutex_unlock(&mutex_bitmap);
}