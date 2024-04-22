#include "../include/archivos.h"


/*Pasa todo el contenido del archivo a un char**, se debe destruirlo luego de utilizar la funcion*/
char** pasarArchivoEstructura(FILE* f)
{   
    char** archivoInstrucciones = string_array_new();
    char* instruccionLeida;
    while(!feof(f))
    {
        instruccionLeida = string_new();
        fgets(instruccionLeida,30,f);
        string_array_push(&archivoInstrucciones,instruccionLeida);
    }
    return archivoInstrucciones;
}