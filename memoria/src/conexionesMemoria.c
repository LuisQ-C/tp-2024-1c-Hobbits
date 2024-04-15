#include "../include/conexionesMemoria.h"

typedef struct
{
    int fd_conexion_IO;
    t_log* logger;
} t_datos_server_interfaces;

int iniciar_conexiones(t_log* logger,t_config* config,int* server_fd,int* fd_cpu,int* fd_kernel)
{
    //FALTA IP, esta hardcodadeado
    char* puerto;
    //INICIA SERVIDOR
    puerto = config_get_string_value(config,"PUERTO_ESCUCHA");
    *server_fd = iniciar_servidor(logger,"0.0.0.0",puerto);
    log_info(logger,"Servidor listo para recibir cliente!");
    //ESPERAR CPU
    *fd_cpu = esperar_cliente(*server_fd,logger,"CPU");
    //ESPERAR KERNEL
    *fd_kernel = esperar_cliente(*server_fd,logger,"KERNEL");
    
    return *server_fd != 0 && *fd_cpu != 0 && *fd_kernel != 0;
}

int escucharConexionesIO(t_log* logger,int fd_escucha_interfaces){
    int fd_conexion_IO = esperar_cliente(fd_escucha_interfaces,logger,"INTERFAZ I/O");
    int err;
    pthread_t conexionesIO;
    t_datos_server_interfaces* datosServerInterfaces = malloc(sizeof(t_datos_server_interfaces));
    datosServerInterfaces->fd_conexion_IO = fd_conexion_IO;
    datosServerInterfaces->logger = logger;
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
    t_log* logger = auxiliarDatosServer->logger;
    free(auxiliarDatosServer);
    recibir_handshake(logger,fd_conexion_IO,"MODULO I/O");
}

void manejarConexionCPU(t_log* logger,int* fd_cpu)
{
    int cod_op, bytes;
    while(1)
    {
        bytes = recv(*fd_cpu, &cod_op, sizeof(cod_op), MSG_WAITALL);
        //SI recv devuelve 0 significa que la conexion se cerro del otro lado, del lado del kernel
        if(bytes==0)
        {
            log_error(logger, "el cliente se desconecto. Terminando servidor");
            return;
        }
        //SI recv retorna -1 significa que hubo un error mas grave
        else if(bytes==-1)
        {
            log_error(logger,"error de fd_conexion_dispatch en CPU");
            return;
        }
		switch (cod_op) {
		case HANDSHAKE:
			recibir_handshake(logger,*fd_cpu,"MODULO CPU");
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            break;
			//return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
        break; //NO SE DEBERIA ROMPER EL WHILE, ESTO VA EN UN HILO APARTE
    }
}

void terminar_programa(t_log* logger,t_config* config,int* fd_cpu,int* fd_kernel){
    destruir_log_config(logger,config);
    close(*fd_cpu);
    close(*fd_kernel);
}

