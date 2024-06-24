#include "../include/compactacion.h"
//extern void* block_fs;
//extern void* data_bitmap;

/*****************
FUNCIONES ARCHIVO DONDE SE GUARDAN LOS NOMBRES DE LOS METADATAS
*****************/

/*CREAR EL ARCHIVO*/

void crear_archivo_metadatas(char* ruta_archivo_madre)
{
    FILE* archivo_nombre_metadatas = fopen(ruta_archivo_madre,"w");
    fclose(archivo_nombre_metadatas);
}

/*AGREGAR UN NOMBRE AL ARCHIVO*/

void agregar_nombre_metadatas(char* nombre, char* ruta_archivo_madre)
{
    char* nombre_metadata = string_duplicate(nombre);
    FILE* archivo_madre = fopen(ruta_archivo_madre,"a+");
    string_append(&nombre_metadata,"\n");
    //fprintf(archivo_madre,"%s",nombre);
    fputs(nombre_metadata,archivo_madre);
    free(nombre_metadata);
    fclose(archivo_madre);
}

/*ELIMINAR UN NOMBRE DEL ARCHIVO*/
void eliminar_nombre_metadatas(char* nombre, char* ruta_archivo_madre, char* ruta_base)
{
    char* ruta_arch_aux = string_new();
    string_append(&ruta_arch_aux,ruta_base);
    string_append(&ruta_arch_aux,"/arch_aux.txt");

    char* nombre_metadata = string_duplicate(nombre);
    FILE* archivo_madre = fopen(ruta_archivo_madre,"r+");
    fseek(archivo_madre,0,SEEK_SET);

    FILE* archivo_aux = fopen(ruta_arch_aux,"w+");
    size_t longitud = 1;
    char* nombre_leido = string_new();
    while(getline(&nombre_leido,&longitud,archivo_madre) != -1)
    {
        if(string_contains(nombre_leido,"\n"))
            nombre_leido[string_length(nombre_leido)-1]='\0';
        
        if(strcmp(nombre,nombre_leido) != 0)
        {
            string_append(&nombre_leido,"\n");
            fputs(nombre_leido,archivo_aux);
        }
    
        longitud = 1;
        //free(nombre_leido); el getline es capo y se encarga
    }
    
    fclose(archivo_madre);
    fclose(archivo_aux);

    remove(ruta_archivo_madre);
    rename(ruta_arch_aux,ruta_archivo_madre);

    free(nombre_leido);
    free(ruta_arch_aux);
    free(nombre_metadata);
}
/**/



/*OBTENER NOMBRES DE LOS METADATAS Y SUS ATRIBUTOS*/
t_list* obtener_nombres_metadatas_con_atributos(char* ruta_archivo_madre, char* ruta_base)
{
    //char* nombre_leido;
    //char* ruta_archivo;
    //t_config* config_metadata;
    //t_metadata* nuevo_metadata;
    t_list* nombres_metadatas_atributos = list_create();
    t_list* nombre_metadatas = obtener_nombres_metadatas(ruta_archivo_madre);

    t_list_iterator* iterar_nombres = list_iterator_create(nombre_metadatas);

    while(list_iterator_has_next(iterar_nombres))
    {
        //LEO EL SIGUIENTE NOMBRE DE LA LISTA
        char* nombre_leido = list_iterator_next(iterar_nombres);
        //ARMO LA RUTA PARA ACCEDER A LOS DATOS DEL METADATA
        char* ruta_archivo = string_new();
        string_append(&ruta_archivo,ruta_base);
        string_append(&ruta_archivo,"/");
        string_append(&ruta_archivo,nombre_leido);
        
        //CREO EL CONFIG
        t_config* config_metadata = config_create(ruta_archivo);
        //CREO UN NUEVO ELEMENTO PARA LA LISTA QUE CONTIENE METADATAS Y ATRIBUTOS
        t_metadata* nuevo_metadata = malloc(sizeof(t_metadata));
        nuevo_metadata->nombre_metadata = string_duplicate(nombre_leido);
        nuevo_metadata->bloque_inicial = config_get_int_value(config_metadata,"BLOQUE_INICIAL");
        nuevo_metadata->tamanio = config_get_int_value(config_metadata,"TAMANIO");
        list_add(nombres_metadatas_atributos,nuevo_metadata);

        config_destroy(config_metadata);
        free(ruta_archivo);
        
    }
    list_iterator_destroy(iterar_nombres);
    list_destroy_and_destroy_elements(nombre_metadatas,(void*)liberar_elemento);
    

    return nombres_metadatas_atributos; //DEBE LIBERARSE LUEGO DE UTILIZARSE
}

void liberar_t_metadata(t_metadata* self)
{
    free(self->nombre_metadata);
    free(self);
}


/*PARA PRUEBAS, IMPRIMIR LA LISTA DE METADATAS CON ATRIBUTOS*/
void iterar_imprimir_atributos(t_list* lista_metadatas)
{
    list_iterate(lista_metadatas,(void*)imprimir_atributos);
}
void imprimir_atributos(t_metadata* metadata)
{
    printf("\nNOMBRE METADATA: %s\n",metadata->nombre_metadata);
    printf("\nBLOQUE_INICIAL: %d\n",metadata->bloque_inicial);
    printf("\nTAMANIO: %d\n",metadata->tamanio);
}


/*CONVERTIR EL ARCHIVO DE LOS NOMBRES A UNA LISTA*/
t_list* obtener_nombres_metadatas(char* ruta_archivo_madre)
{
    t_list* nombres_metadatas = list_create();

    FILE* archivo_madre = fopen(ruta_archivo_madre,"r+");
    fseek(archivo_madre,0,SEEK_SET);
    size_t longitud = 1;

    char* nombre_leido = string_new();

    while(getline(&nombre_leido,&longitud,archivo_madre) != -1)
    {
        if(string_contains(nombre_leido,"\n"))
            nombre_leido[string_length(nombre_leido)-1]='\0';

        list_add(nombres_metadatas,nombre_leido);

        longitud = 1;
        //free(nombre_leido); el getline es capo y se encarga
        nombre_leido = string_new();
    }

    fclose(archivo_madre);
    free(nombre_leido);

    return nombres_metadatas;
}

void ordenar_lista_por_bloque_inicial(t_list* lista)
{
    list_sort(lista,(void*)bloque_inicial_menor);
}

bool bloque_inicial_menor(t_metadata* actual, t_metadata* siguiente)
{
    return actual->bloque_inicial < siguiente->bloque_inicial;
}






