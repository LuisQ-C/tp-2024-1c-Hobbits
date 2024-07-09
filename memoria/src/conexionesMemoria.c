#include "../include/conexionesMemoria.h"
extern t_log* logger_obligatorio;
extern t_log* logger;
extern t_config* config;
extern t_datos_memoria datos_memoria;
extern void* user_space;

typedef struct
{
    int fd_conexion_IO;
} t_datos_server_interfaces;



int iniciar_conexiones(int* server_fd,int* fd_cpu,int* fd_kernel)
{
    //FALTA IP, esta hardcodadeado
    char* puerto;
    //INICIA SERVIDOR
    puerto = string_duplicate(datos_memoria.puerto_escucha);
    *server_fd = iniciar_servidor(logger,NULL,puerto);
    free(puerto);
    log_info(logger,"Servidor listo para recibir cliente!");
    //ESPERAR CPU
    *fd_cpu = esperar_cliente(*server_fd,logger,"CPU");
    //ESPERAR KERNEL
    *fd_kernel = esperar_cliente(*server_fd,logger,"KERNEL");
    
    return *server_fd != 0 && *fd_cpu != 0 && *fd_kernel != 0;
}

void realizar_handshakes_memoria(int fd_cpu, int fd_kernel)
{
    realizar_handshake_cpu(fd_cpu);
    realizar_handshake_kernel(fd_kernel);
}

void inicializar_hilos(int fd_cpu, int fd_kernel)
{
    iniciar_hilo_cpu(fd_cpu);
    iniciar_hilo_kernel(fd_kernel);
}

int escucharConexionesIO(int fd_escucha_interfaces){
    int fd_conexion_IO = esperar_cliente(fd_escucha_interfaces,logger,"INTERFAZ I/O");
    int err;
    pthread_t conexionesIO;
    t_datos_server_interfaces* datosServerInterfaces = malloc(sizeof(t_datos_server_interfaces));
    datosServerInterfaces->fd_conexion_IO = fd_conexion_IO;
    err = pthread_create(&conexionesIO,NULL,(void*) procesarConexionesIO,(void*) datosServerInterfaces);
    if(err != 0)
    {
        log_error(logger,"Error al crear el hilo de conexion IO");
        perror("\nError hilo");
        exit(1);
    }
    pthread_detach(conexionesIO);
    
    
    return 1;
}

void procesarConexionesIO(void* datosServerInterfaces){
    t_datos_server_interfaces* auxiliarDatosServer = (t_datos_server_interfaces*) datosServerInterfaces;
    int fd_conexion_IO = auxiliarDatosServer->fd_conexion_IO;
    free(auxiliarDatosServer);

    int codigoOperacion = recibir_operacion(fd_conexion_IO);

    if(codigoOperacion == -1)
    {
        log_error(logger,"Error al recibirOperacion");
        //return;
    }
    int retardo = datos_memoria.retardo_respuesta;
    char* interfazConectada = recibir_mensaje(fd_conexion_IO,logger);
    enviar_handshake_ok(logger,fd_conexion_IO,interfazConectada);
    char** nombre_tipo = string_split(interfazConectada,"-");
    char* nombre = string_duplicate(nombre_tipo[1]);

    
    //RECIBE ESCRITURA O LECTURA
    while(1)
    {
        int codigo_op = recibir_operacion(fd_conexion_IO);

        if(codigo_op == DESCONEXION)
        {
            log_warning(logger,"Se desconecto la interfaz: %s",nombre);
            break;
        }
        else if(codigo_op == ERROR)
        {
            log_error(logger,"Error en el socket con la interfaz: %s",nombre);
            break;
        }

        switch(codigo_op)
        {
            case LECTURA:
            {
                int pid,tamanio_fragmento,direccion_fisica;
                void* dato_leido = recibir_peticion_lectura(&pid,&tamanio_fragmento,&direccion_fisica,fd_conexion_IO);
        
                memcpy(dato_leido,user_space + direccion_fisica,tamanio_fragmento);
                log_info(logger_obligatorio,"Acceso a espacio de usuario: \"PID: %d - Accion: LEER - Direccion fisica: %d - Tamaño %d\"",pid,direccion_fisica,tamanio_fragmento);

                usleep(retardo*1000); 
                send(fd_conexion_IO,dato_leido,tamanio_fragmento,0);
                free(dato_leido);
                break;
            }
            case ESCRITURA:
            {
                int resultado = 1;
                int pid,tamanio,base,direccion_fisica;
                void* dato_escribir = recibir_peticion_escritura(&pid,&base,&tamanio,&direccion_fisica,fd_conexion_IO);
                memcpy_pagina_frame(direccion_fisica,base,dato_escribir,tamanio);
                log_info(logger_obligatorio,"Acceso a espacio de usuario: \"PID: %d - Accion: ESCRIBIR - Direccion fisica: %d - Tamaño %d\"",pid,direccion_fisica,tamanio);

                free(dato_escribir);
                usleep(retardo*1000);
                send(fd_conexion_IO,&resultado,sizeof(int),0); // ENVIA OK SI ESTA OK
                break;
            }
            case -1:
            {
                log_error(logger, "el cliente se desconecto. Terminando servidor");
                break;
            }
		    default:
            {
                log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			    break;
            }
        }
    }

    string_array_destroy(nombre_tipo);
    free(nombre);
    free(interfazConectada);
    
}

void terminar_programa(int* fd_cpu,int* fd_kernel){
    destruir_log_config(logger,config);
    close(*fd_cpu);
    close(*fd_kernel);
}

