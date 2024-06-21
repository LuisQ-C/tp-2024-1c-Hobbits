#include "../include/compactacion.h"


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


/*CONVERTIR EL ARCHIVO A UNA LISTA*/




