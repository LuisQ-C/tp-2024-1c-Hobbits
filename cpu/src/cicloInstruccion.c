#include "../include/cicloInstruccion.h"

extern t_registro_cpu registro;


/*
 CICLO_DE_INSTRUCCION(){

    instruccion fetch();
    void decodificar(instruccion);
    tipo ejecutar(instruccion,a,b);
    ir al hilo para chequear interrupcion
}*/

/*
decodificar(){
    case SUM
        b = ax 
        a = bx
        sum(a,b)

    case 
}

*/

char* identificarInstruccion(char* instruccion)
{
    int i=0;
    int largoAccion = contadorHastaPrimerEspacio(instruccion);
    char* accion = malloc(largoAccion+1);
    while(instruccion[i] != ' '){
        accion[i] = instruccion[i];
        i++;
    }
    accion[largoAccion+1]='\0';
    return accion;
}

int contadorHastaPrimerEspacio(char* instruccion)
{
    int i=0;
    int caracteres = 0;

    while(instruccion[i] != '\0'){       
        if(instruccion[i] == ' ')
             break;
        caracteres++;
        i++;
    }
    return caracteres;
}

// CICLO
/*
void ejecutarInstruccion(char* instruccion){
    char* primerRegistro = malloc(3);
    char* segundoRegistro = malloc(3);
    char* accion = malloc(4);
    int i = 0;
    while(instruccion[i] != '\0')
    {
        if(instruccion[i] == ' ')
            
    }
}
*/



