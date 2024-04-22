#include "../include/archivos.h"


/*Pasa todo el contenido del archivo a un char**, se debe destruirlo luego de utilizar la funcion*/
char** pasarArchivoEstructura(FILE* f)
{   
    char** archivoInstrucciones = string_array_new();
    //char* instruccionLeida;
    char* instruccionLeida = string_new();
    size_t longitud = 1;
    while(getline(&instruccionLeida, &longitud,f) != -1)
    {
        string_array_push(&archivoInstrucciones,instruccionLeida);
        instruccionLeida = string_new();
        longitud = 1;
    }
    free(instruccionLeida);
    //HAGO FREE PARA LIBERAR EL ULTIMO STRING NEW QUE RESERVE
    //Y QUE NO SE OCUPO PORQUE LLEGUE AL FIN DEL ARCHIVO
    //free(instruccionLeida);
    /*while(!feof(f))
    {
        instruccionLeida = string_new();
        fgets(instruccionLeida,30,f);
        string_array_push(&archivoInstrucciones,instruccionLeida);
    }*/
    //free(instruccionLeida);
    return archivoInstrucciones;
}