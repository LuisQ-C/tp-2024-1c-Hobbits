#include "../include/conexiones.h"

extern t_log* logger;
extern sem_t planificacion_blocked_iniciada;
extern t_squeue *lista_procesos_ready;
typedef struct
{
    int fd_conexion_IO;
    t_log* logger;
} t_datos_server_interfaces;

int iniciar_conexiones(t_config* config,t_log* logger,int* fd_memoria,int* fd_cpu_dispatch, int* fd_cpu_interrupt,int* fd_escucha_interfaces)
{
    char* ip;
    char* puerto;
    //CONECTARSE A MEMORIA
    ip = config_get_string_value(config,"IP_MEMORIA");
    puerto = config_get_string_value(config,"PUERTO_MEMORIA");
    *fd_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    //CONECTARSE A CPU A TRAVES DE DISPATCH E INTERRUPT
    ip = config_get_string_value(config,"IP_CPU");
    puerto = config_get_string_value(config,"PUERTO_CPU_DISPATCH");
    *fd_cpu_dispatch = crear_conexion(ip,puerto,logger,"CPU-DISPATCH");
    puerto = config_get_string_value(config,"PUERTO_CPU_INTERRUPT");
    *fd_cpu_interrupt = crear_conexion(ip,puerto,logger,"CPU-INTERRUPT");
    //LEVANTAR SERVER PARA I/O CON UN HILO
    ip = config_get_string_value(config,"IP_KERNEL");
    puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
    *fd_escucha_interfaces = iniciar_servidor(logger,ip,puerto);
    return *fd_memoria != 0 && *fd_cpu_dispatch != 0 && *fd_cpu_interrupt != 0 && *fd_escucha_interfaces != 0;
}

void realizar_handshakes_kernel(int fd_memoria,int fd_cpu_dispatch, int fd_cpu_interrupt)
{
    mandarHandshake(logger,fd_memoria,"MODULO MEMORIA","MODULO KERNEL");
    mandarHandshake(logger,fd_cpu_dispatch,"MODULO CPU DISPATCH","MODULO KERNEL-DISPATCH");
    mandarHandshake(logger,fd_cpu_interrupt,"MODULO CPU INTERRUPT","MODULO KERNEL-INTERRUPT");
}

int escucharConexionesIO(t_log* logger,int fd_escucha_interfaces){
    int err;
    int fd_conexion_IO = esperar_cliente(fd_escucha_interfaces,logger,"INTERFAZ I/O");
    pthread_t conexionesIO;
    t_datos_server_interfaces* datosServerInterfaces = malloc(sizeof(t_datos_server_interfaces));
    datosServerInterfaces->fd_conexion_IO= fd_conexion_IO;
    datosServerInterfaces->logger = logger;
    err = pthread_create(&conexionesIO,NULL,(void*) procesarConexionesIO,(void*) datosServerInterfaces);
    if(err != 0)
    {
        log_error(logger,"Error al crear el hilo de conexion IO");
        perror("\nError creando hilo IO");
        exit(1);
    }
    pthread_detach(conexionesIO);
    
    
    return 1;
}
/***************************
          HILOS IOS
******************************/
void procesarConexionesIO(void* datosServerInterfaces){
    t_datos_server_interfaces* auxiliarDatosServer = (t_datos_server_interfaces*) datosServerInterfaces;
    int fd_conexion_IO = auxiliarDatosServer->fd_conexion_IO;
    t_log* logger = auxiliarDatosServer->logger;
    free(auxiliarDatosServer);

    recibir_operacion(fd_conexion_IO);
    char* interfaz_conectada = recibir_mensaje(fd_conexion_IO,logger);
    enviar_handshake_ok(logger,fd_conexion_IO,interfaz_conectada);

    //int codigo_operacion;

    char** tipo_nombre_io = string_split(interfaz_conectada,"-");
    int tipo;

    if(strcmp("GENERICA",tipo_nombre_io[0]))
    {
        tipo = IO_GEN_SLEEP;
        
    }
    else if(strcmp("STDIN",tipo_nombre_io[0]))
    {
        tipo = IO_STDIN_READ;
    }
    else if(strcmp("STDOUT",tipo_nombre_io[0]))
    {
        tipo = IO_STDOUT_WRITE;
    }
    else if(strcmp("DIALFS",tipo_nombre_io[0]))
    {
        tipo = IO_FS;
    }
    else
    {
        log_warning(logger,"Tipo Invalido");
        return;
    }

    t_list_io* interfaz_agregada = agregar_interfaz_lista(tipo_nombre_io[1],tipo,fd_conexion_IO);

    string_array_destroy(tipo_nombre_io);
    free(interfaz_conectada);


    /*******************
    WHILE 1 PARA INTERFAZ GENERICA
    **************/

    while(1)
    {
        /*
        codigo_operacion = recibir_operacion(fd_conexion_IO);

        if(codigo_operacion == DESCONEXION)
        {
        log_error(logger,"TE DESCONECTASTE FLACO (KERNEL-IO)");
        //SACARLO DE LA LISTA A LA IO
        return;
        }
        else if(codigo_operacion == ERROR)
        {
        log_warning(logger,"ERROR EN EL RECIBIR_OPERACION (KERNEL-IO)");
        return;
        }*/

        sem_wait(interfaz_agregada->hay_proceso_cola);

        sem_wait(&planificacion_blocked_iniciada);          //no podrian trabajar 2 colas al mismo tiempo
        t_elemento_iogenerica* solicitud_io = pop_elemento_cola_io(interfaz_agregada);
        
        int respuesta;
        send(fd_conexion_IO,&solicitud_io->tiempo,sizeof(int),0);
        recv(fd_conexion_IO,&respuesta,sizeof(int),MSG_WAITALL);
        if(respuesta==4)//hardcoadeado el 4
        {
            printf("\nTERMINO LA SOLICITUD CORRECTAMENTE\n");
        }
        //transicion a ready
        cambiar_a_ready(solicitud_io->pcb);

        log_info(logger, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY", solicitud_io->pcb->pid);

        free(solicitud_io);

        sem_post(&planificacion_blocked_iniciada);
        
    }
}

void terminar_programa(t_log* logger,t_config* config,int* fd_memoria,int* fd_cpu_dispatch,int* fd_cpu_interrupt)
{
    destruir_log_config(logger,config);
    close(*fd_memoria);
    close(*fd_cpu_dispatch);
    close(*fd_cpu_interrupt);
}
