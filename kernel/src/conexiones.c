#include "../include/conexiones.h"

extern t_log* logger;
extern t_log* logger_obligatorio;
extern sem_t planificacion_blocked_iniciada;
extern sem_t proceso_en_cola_ready;

extern t_squeue *lista_procesos_ready;
extern t_squeue *lista_procesos_exit;
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
    *fd_escucha_interfaces = iniciar_servidor(logger,NULL,puerto);
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
    

    //int codigo_operacion;

    

    char** tipo_nombre_io = string_split(interfaz_conectada,"-");
    int tipo = string_to_type(tipo_nombre_io[0]);

    //CHEQUEO INTERFACES EXISTENES Y TIPOS CORRECTOS
    if(sinterfaz_name_already_took(tipo_nombre_io[1]))
    {
        enviar_handshake_error(logger,fd_conexion_IO,interfaz_conectada);
        close(fd_conexion_IO);
        return;
    }else
    {
        enviar_handshake_ok(logger,fd_conexion_IO,interfaz_conectada);
    }

    if (tipo==-1)
    {
        log_warning(logger,"Tipo Invalido, finalizando hilo");
        return;
    }
    
    
    

    t_list_io* interfaz_agregada = agregar_interfaz_lista(tipo_nombre_io[1],tipo,fd_conexion_IO);

    string_array_destroy(tipo_nombre_io);

    free(interfaz_conectada);


    /*******************
    DECIDIMOS A QUE FUNCION IR DEPENDIENDO DEL TIPO DE INTERFAZ
    **************/
    switch(tipo)
    {
        case IO_GEN_SLEEP:
            atender_interfaz_generica(interfaz_agregada);
            break;
        case IO_STDIN_READ:
            atender_interfaz_stdin_stdout(interfaz_agregada,IO_STDIN_READ);
            break;
        case IO_STDOUT_WRITE:
            atender_interfaz_stdin_stdout(interfaz_agregada,IO_STDOUT_WRITE);
            break;
        case IO_FS:
            atender_interfaz_dial_fs(interfaz_agregada, IO_FS);
            break;
        /*
       case IO_FS_CREATE:
            atender_interfaz_dial_fs(interfaz_agregada,IO_FS_CREATE);// fs-write truncate y esas cosas
            break;
        case IO_FS_DELETE:
            atender_interfaz_dial_fs(interfaz_agregada,IO_FS_DELETE);// fs-write truncate y esas cosas
            break;
        case IO_FS_TRUNCATE:
            atender_interfaz_dial_fs(interfaz_agregada,IO_FS_TRUNCATE);// fs-write truncate y esas cosas
            break;
        case IO_FS_WRITE:
            atender_interfaz_dial_fs(interfaz_agregada,IO_FS_WRITE);// fs-write truncate y esas cosas
            break;
        case IO_FS_READ:
            atender_interfaz_dial_fs(interfaz_agregada,IO_FS_READ);// fs-write truncate y esas cosas
            break;
        */
    }

    
}

int string_to_type(char* tipo)
{
    if(!strcmp("GENERICA",tipo))
    {
        return IO_GEN_SLEEP;
        
    }
    else if(!strcmp("STDIN",tipo))
    {
        return IO_STDIN_READ;
    }
    else if(!strcmp("STDOUT",tipo))
    {
        return IO_STDOUT_WRITE;
    }
    else if(!strcmp("DIALFS", tipo)){
        return IO_FS;
    }
    else{
        return -1;
    }
    /*
    else if(!strcmp("IO_FS_CREATE",tipo))//CAmbiar al tipo q corresponda
    {
        return IO_FS_CREATE;
    }
     else if(!strcmp("IO_FS_DELETE",tipo))
    {
        return IO_FS_DELETE;
    }
     else if(!strcmp("IO_FS_TRUNCATE",tipo))
    {
        return IO_FS_TRUNCATE;
    }
     else if(!strcmp("IO_FS_WRITE",tipo))
    {
        return IO_FS_WRITE;
    }
     else if(!strcmp("IO_FS_READ",tipo))
    {
        return IO_FS_READ;
    }
    else
    {
        return -1;
    }*/
}

void atender_interfaz_generica(t_list_io* interfaz)
{
    int respuesta, err_recv, err_send;
    t_elemento_iogenerica* solicitud_io=NULL;
    while(1)
    {
        sem_wait(interfaz->hay_proceso_cola);
        
        if(!cola_io_is_empty(interfaz))
        {
            solicitud_io = peek_elemento_cola_io(interfaz);
            int tiempo_dormicion = solicitud_io->tiempo;
            err_send = enviar_solicitud_io_generico(solicitud_io->pcb->pid,tiempo_dormicion,interfaz->fd_interfaz);
            if(err_send == -1){
                log_info(logger, "SE DESCONECTA LA INTERFAZ %s", interfaz->nombre_interfaz);    
                break;
            }
            err_recv = recv(interfaz->fd_interfaz,&respuesta,sizeof(int),MSG_WAITALL);
            
            if(err_recv == 0)
            {
                log_info(logger, "SE DESCONECTA LA INTERFAZ %s", interfaz->nombre_interfaz);
            break;
            }

            if(respuesta==INTERFAZ_LISTA) 
            {
                //log_info(logger, "COMPLETO LA SOLICITUD");
            }
        }

        
        
        sem_wait(&planificacion_blocked_iniciada);

        if(!cola_io_is_empty(interfaz))
        {
            pop_elemento_cola_io(interfaz);
            mandar_pcb_cola_correspondiente(solicitud_io->pcb,solicitud_io->cola_destino);
            free(solicitud_io);
        }

        sem_post(&planificacion_blocked_iniciada);
        
    }

    while(!cola_io_is_empty(interfaz)){
        solicitud_io = pop_elemento_cola_io(interfaz);
        manejar_fin_con_motivo(INVALID_INTERFACE, solicitud_io->pcb);
        free(solicitud_io);
    }
    
    quitar_interfaz_lista(interfaz);
    //DESTRUIRSE A SI MISMO, SACARSE DE LA LISTA, ENVIAR TODOS A WAIT
}

void atender_interfaz_stdin_stdout(t_list_io* interfaz, int tipo_interfaz)
{
    int respuesta, err_recv, err_send;
    t_elemento_io_in_out* solicitud_io=NULL;
    while(1)
    {
        sem_wait(interfaz->hay_proceso_cola);
        
        if(!cola_io_is_empty(interfaz))
        {
            solicitud_io = peek_elemento_cola_io(interfaz);
            
            int cant_direcciones = list_size(solicitud_io->direcciones_fisicas);

            err_send = enviar_solicitud_stdin_stdout(solicitud_io->pcb->pid,solicitud_io->direcciones_fisicas,interfaz->fd_interfaz,cant_direcciones,tipo_interfaz);
            
            if(err_send == -1){
                log_info(logger, "SE DESCONECTA LA INTERFAZ %s", interfaz->nombre_interfaz);    
                break;
            }

            err_recv = recv(interfaz->fd_interfaz,&respuesta,sizeof(int),MSG_WAITALL);
            
            if(err_recv == 0)
            {
                log_info(logger, "SE DESCONECTA LA INTERFAZ %s", interfaz->nombre_interfaz);    

            break;
            }

            if(respuesta==INTERFAZ_LISTA)
            {
            //printf("\nTERMINO LA SOLICITUD CORRECTAMENTE\n");
            }
        }

        
        
        sem_wait(&planificacion_blocked_iniciada);

        if(!cola_io_is_empty(interfaz))
        {
            pop_elemento_cola_io(interfaz);
            mandar_pcb_cola_correspondiente(solicitud_io->pcb,solicitud_io->cola_destino);
            list_destroy_and_destroy_elements(solicitud_io->direcciones_fisicas,(void*) liberar_elemento);
            free(solicitud_io);
        }

        sem_post(&planificacion_blocked_iniciada);
        
    }

    while(!cola_io_is_empty(interfaz)){
        solicitud_io = pop_elemento_cola_io(interfaz);
        manejar_fin_con_motivo(INVALID_INTERFACE, solicitud_io->pcb);
        list_destroy_and_destroy_elements(solicitud_io->direcciones_fisicas, (void*) liberar_elemento);
        free(solicitud_io);
    }
    
    quitar_interfaz_lista(interfaz);

    //DESTRUIRSE A SI MISMO, SACARSE DE LA LISTA, ENVIAR TODOS A EXIT
    
}

void atender_interfaz_dial_fs(t_list_io* interfaz, int tipo_interfaz)
{
    int respuesta, err_recv, err_send;
    t_elemento_io_fs* solicitud_dial_fs=NULL;

    while(1)
    {

        sem_wait(interfaz->hay_proceso_cola);
        
        if(!cola_io_is_empty(interfaz))
        {   

            solicitud_dial_fs = peek_elemento_cola_io(interfaz);
            int cant_direcciones = 0;
            if(solicitud_dial_fs->direcciones_fisicas != NULL)
            {
                cant_direcciones = list_is_empty(solicitud_dial_fs->direcciones_fisicas) ? 0 : list_size(solicitud_dial_fs->direcciones_fisicas);
            }
            err_send = enviar_solicitud_dial_fs(solicitud_dial_fs->pcb->pid, solicitud_dial_fs->nombre_archivo, solicitud_dial_fs->tamanio, solicitud_dial_fs->direcciones_fisicas, cant_direcciones, interfaz->fd_interfaz, solicitud_dial_fs->puntero, solicitud_dial_fs->codOp);

            if(err_send == -1){
                log_info(logger, "SE DESCONECTA LA INTERFAZ %s", interfaz->nombre_interfaz);    
                break;
            }

            err_recv = recv(interfaz->fd_interfaz,&respuesta,sizeof(int),MSG_WAITALL);
            
            if(err_recv == 0)
            {
                log_info(logger, "SE DESCONECTA LA INTERFAZ %s", interfaz->nombre_interfaz);    

            break;
            }

            if(respuesta==INTERFAZ_LISTA)
            {
            //printf("\nTERMINO LA SOLICITUD CORRECTAMENTE\n");
            }
        }

        sem_wait(&planificacion_blocked_iniciada);

        if(!cola_io_is_empty(interfaz))
        {
            pop_elemento_cola_io(interfaz);
            free(solicitud_dial_fs->nombre_archivo);
            mandar_pcb_cola_correspondiente(solicitud_dial_fs->pcb,solicitud_dial_fs->cola_destino);
            if(solicitud_dial_fs->direcciones_fisicas!=NULL)
                list_destroy_and_destroy_elements(solicitud_dial_fs->direcciones_fisicas,(void*) liberar_elemento);
            
            free(solicitud_dial_fs);
        }

        sem_post(&planificacion_blocked_iniciada);
        
    }

    while(!cola_io_is_empty(interfaz)){
        solicitud_dial_fs = pop_elemento_cola_io(interfaz);
        manejar_fin_con_motivo(INVALID_INTERFACE, solicitud_dial_fs->pcb);
        if(solicitud_dial_fs->direcciones_fisicas != NULL)
            list_destroy_and_destroy_elements(solicitud_dial_fs->direcciones_fisicas, (void*) liberar_elemento);
        free(solicitud_dial_fs->nombre_archivo);
        free(solicitud_dial_fs);
    }
    
    quitar_interfaz_lista(interfaz);
    
}
//###########################


void mandar_pcb_cola_correspondiente(t_pcb* pcb, int cola_destino)
{
    if(cola_destino == READY)
    {
        mostrar_cola_ready();
        log_info(logger_obligatorio, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY", pcb->pid);
        cambiar_a_ready(pcb);
        sem_post(&proceso_en_cola_ready);
    }
    else if(cola_destino == READYPLUS)
    {
        mostrar_cola_ready_plus();
        log_info(logger_obligatorio, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY+", pcb->pid);
        cambiar_a_ready_plus(pcb);
        sem_post(&proceso_en_cola_ready);
    }
    else if(cola_destino == COLA_EXIT)
    {
        squeue_push(lista_procesos_exit,pcb);
        log_info(logger_obligatorio, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: EXIT", pcb->pid);
    }
    else if(cola_destino == COLA_EXIT_USUARIO)
    {
        manejar_fin_con_motivo(INTERRUPTED_BY_USER_BLOCKED,pcb);
        //ACA IRIA LA FUNCION MANEJAR MOTIVO QUE HIZO LUIS
    }
    else
    {
        log_warning(logger,"COLA DESTINO INVALIDA");
    }
}



void terminar_programa(t_log* logger,t_config* config,int* fd_memoria,int* fd_cpu_dispatch,int* fd_cpu_interrupt)
{
    destruir_log_config(logger,config);
    close(*fd_memoria);
    close(*fd_cpu_dispatch);
    close(*fd_cpu_interrupt);
}
