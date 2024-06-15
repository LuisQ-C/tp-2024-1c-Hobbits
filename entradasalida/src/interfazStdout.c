#include "../include/interfazStdout.h"

extern t_log* logger;
/*
typedef struct{
    int base;
    int tamanio;
    int direccion_fisica;
}t_porcion_dato_out;*/

void stdOut(t_config* config, int fd_conexion_kernel,int fd_conexion_memoria){

    int base=-99999;
    int direccionFisica=-8888;
    int tamanio=-3;
    int pid;
    void* cadenaEntera;

while(1){

    recibir_operacion(fd_conexion_kernel);
    
    t_list* listaRecibida= recibir_paquete(fd_conexion_kernel); //recibo lista 
    //t_list* listaRecibida = lista_pruebas_out();

    int* lista_PID = list_get(listaRecibida,0);
    pid  = *lista_PID;
    log_info(logger,"STDOUT - Operación: \"PID: %d - Operacion: IO_STDOUT_WRITE\"",pid);
    //PID+TAMANIOFRAGMENTO+DIRECCIONFISICA
    int tamanio_total = 0;

    for(int i=1;i<list_size(listaRecibida);i++)
    {
        t_porcion_dato* dato = list_get(listaRecibida,i);
        tamanio_total += dato->tamanio;
    }

    cadenaEntera = malloc(tamanio_total+1);

    for(int i=1; i<list_size(listaRecibida); i++)
    {
    t_porcion_dato* datoAenviar = list_get(listaRecibida,i);
    //cadenaDeCaracteres+base+tamanio+direccionfisica
    t_paquete* infoAenviar = crear_paquete(LECTURA);
    base= datoAenviar->base;
    direccionFisica= datoAenviar->direccion_fisica;
    tamanio= datoAenviar-> tamanio;

    agregar_a_paquete(infoAenviar,&pid,sizeof(int));
    //agregar_a_paquete(infoAenviar,cadenaDeCaracteres,strlen(cadenaDeCaracteres));
    //agregar_a_paquete(infoAenviar,&base,sizeof(int));
    agregar_a_paquete(infoAenviar,&tamanio,sizeof(int));
    agregar_a_paquete(infoAenviar,&direccionFisica,sizeof(int));
    enviar_paquete(infoAenviar, fd_conexion_memoria);
    eliminar_paquete(infoAenviar);
    //////espero respuesta

    //cadenaEntera = realloc(cadenaEntera,tamanio_control);
    void* datoRecibido = malloc(tamanio);

    recv(fd_conexion_memoria, datoRecibido, tamanio,MSG_WAITALL);//reci bloqueante
    //char* fragmentoRecibido = (char*) datoRecibido;
    memcpy(cadenaEntera+base,datoRecibido,tamanio);
    free(datoRecibido);
    //string_append(&cadenaEntera, fragmentoRecibido);

    }


    char* casteo_string = (char*) cadenaEntera;

    //char* copia_string = malloc(tamanio_total+1);
    //memcpy(copia_string,cadenaEntera,tamanio_total);
    casteo_string[tamanio_total] = '\0';
    log_info(logger,"La cadena recibida es: %s",casteo_string); //HACERLO CON UN LOGGER APARTE
    int respuesta = INTERFAZ_LISTA;

    send(fd_conexion_kernel,&respuesta,sizeof(int),0);

}

}
/*
t_list* lista_pruebas_out()
{
    t_list* lista = list_create();
    int* pid = malloc(sizeof(int));
    *pid = 805;
    list_add(lista,pid);
    aniadir_porcion_out(lista,0,2,5);
    aniadir_porcion_out(lista,2,2,10);
    aniadir_porcion_out(lista,4,2,15);
    return lista;

}

void aniadir_porcion_out(t_list* lista,int base,int tamanio,int dir_fisica)
{
    t_porcion_dato_out* nueva_porcion = malloc(sizeof(t_porcion_dato_out));
    nueva_porcion->base = base;
    nueva_porcion->tamanio= tamanio;
    nueva_porcion->direccion_fisica = dir_fisica;
    list_add(lista,nueva_porcion);
}*/