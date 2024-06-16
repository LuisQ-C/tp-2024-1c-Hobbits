#include "../include/interfazStdin.h"

extern t_log* logger;
//IO_STDIN_READ (Interfaz, Registro Dirección, Registro Tamaño): Esta instrucción solicita al 
//Kernel que mediante la interfaz ingresada se lea desde el STDIN (Teclado) un valor cuyo 
//tamaño está delimitado por el valor del Registro Tamaño y el mismo se guarde a partir de la 
//Dirección Lógica almacenada en el Registro Dirección.
/*
typedef struct{
    int base;
    int tamanio;
    int direccion_fisica;
}t_porcion_dato_in;*/

void stDin(t_config* config, int fd_conexion_kernel,int fd_conexion_memoria){

int base=-99999;
int direccionFisica=-8888;
int tamanio=-3;
int pid;
char* cadenaDeCaracteres;
int resultado;

while(1){
    recibir_operacion(fd_conexion_kernel);
    t_list* listaRecibida= recibir_paquete(fd_conexion_kernel); //recibo lista 
    //t_list* listaRecibida = lista_pruebas();
    int* lista_PID = list_get(listaRecibida,0);
    pid  = *lista_PID;
    log_info(logger,"STDIN - Operación: \"PID: %d - Operacion: IO_STDIN_READ\"",pid);
    
    cadenaDeCaracteres = readline(">");
    
    void * cadenaAenviar = (void*) cadenaDeCaracteres;

    for(int i=1; i<list_size(listaRecibida); i++){
        t_porcion_dato* datoAenviar = list_get(listaRecibida,i);
        //cadenaDeCaracteres+base+tamanio+direccionfisica
        t_paquete* infoAenviar = crear_paquete(ESCRITURA);
        base= datoAenviar->base;
        direccionFisica= datoAenviar->direccion_fisica;
        tamanio= datoAenviar-> tamanio;
        agregar_a_paquete(infoAenviar,&pid,sizeof(int));
        agregar_a_paquete(infoAenviar,cadenaAenviar,strlen(cadenaDeCaracteres));
        agregar_a_paquete(infoAenviar,&base,sizeof(int));
        agregar_a_paquete(infoAenviar,&tamanio,sizeof(int));
        agregar_a_paquete(infoAenviar,&direccionFisica,sizeof(int));
        enviar_paquete(infoAenviar, fd_conexion_memoria);
        eliminar_paquete(infoAenviar);
        recv(fd_conexion_memoria,&resultado,sizeof(int),MSG_WAITALL);
    }

    free(cadenaAenviar);
    list_destroy_and_destroy_elements(listaRecibida, (void*) liberar_elemento);

    int respuesta = INTERFAZ_LISTA;
    send(fd_conexion_kernel,&respuesta,sizeof(int),0);

}

}
/*
t_list* lista_pruebas()
{
    t_list* lista = list_create();
    int* pid = malloc(sizeof(int));
    *pid = 805;
    list_add(lista,pid);
    aniadir_porcion(lista,0,2,5);
    aniadir_porcion(lista,2,2,10);
    aniadir_porcion(lista,4,2,15);
    return lista;

}

void aniadir_porcion(t_list* lista,int base,int tamanio,int dir_fisica)
{
    t_porcion_dato_in* nueva_porcion = malloc(sizeof(t_porcion_dato_in));
    nueva_porcion->base=base;
    nueva_porcion->tamanio= tamanio;
    nueva_porcion->direccion_fisica = dir_fisica;
    list_add(lista,nueva_porcion);
}*/