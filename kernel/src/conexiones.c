#include "../include/conexiones.h"
typedef struct
{
    int fd_cpu;
    int fd_memoria;
}datos;


int generar_conexiones(t_log* logger,int* fd_memoria,int* fd_cpu,t_config* config){
    char* ip;
    char* puerto;
    
    ip = config_get_string_value(config,"IP_MEMORIA");
    puerto = config_get_string_value(config,"PUERTO_MEMORIA");

    *fd_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    
    ip = config_get_string_value(config,"IP_CPU");
    puerto = config_get_string_value(config,"PUERTO_CPU");

    *fd_cpu = crear_conexion(ip,puerto,logger,"CPU");
    //HILOS
    datos* args = malloc(sizeof(datos));
    args->fd_cpu = *fd_cpu;
    args->fd_memoria = *fd_memoria;
    pthread_t hiloMemoria, hiloCPU;
    pthread_create(&hiloCPU,NULL,(void*) gestionarConexionCPU, (void*) args);
    pthread_create(&hiloMemoria,NULL,(void*) gestionarConexionMemoria,(void*) args);
    pthread_join(hiloCPU,NULL);
    pthread_join(hiloMemoria,NULL);
    
    //
    //return fd_memoria != 0 && fd_cpu != 0;
    return 1;
}
void* gestionarConexionCPU(void* args)
{
    datos* args2 = (datos*) args;
    int fd_cpu = args2->fd_cpu;
    int32_t result = 3;
    int32_t handshake = 1;
    while(1)
    {   
        printf("\n %d", result);
        send(fd_cpu,&handshake,sizeof(int32_t),0);
        recv(fd_cpu,&result,sizeof(int32_t),MSG_WAITALL);
        printf("\n %d", result);
        if(result==0)
        {
            printf("\nLA CPU TE ACEPTO!");
        }else{
            printf("\nLA CPU TE DENEGO!");
        }
        break;
    }
    return NULL;
}
void* gestionarConexionMemoria(void* args)
{
    datos* args2 = (datos*) args;
    int fd_memoria = args2->fd_memoria;
    int32_t result;
    int32_t handshake = 1;
    while(1)
    {
        send(fd_memoria,&handshake,sizeof(int32_t),0);
        recv(fd_memoria,&result,sizeof(int32_t),MSG_WAITALL);
        if(result==0)
        {
            printf("\nLA MEMORIA TE ACEPTO!");
        }else{
            printf("\nLA MEMORIA TE DENEGO!");
        }
        break;
    }
    return NULL;
}
