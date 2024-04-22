#include "../include/conexionesMemoria.h"
//extern t_log* logger;
typedef struct
{
    int fd_conexion_IO;
    t_log* logger;
} t_datos_server_interfaces;

typedef struct main
{
    int fd;
    t_log* logger;
}info_fd_hilos;

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

void inicializar_hilos(t_log* logger, int fd_cpu, int fd_kernel)
{
    //HILO PARA COMUNICACION CON CPU
    pthread_t hiloCPU;
    info_fd_hilos* info_fd_cpu = malloc(sizeof(info_fd_hilos));
    info_fd_cpu->fd = fd_cpu;
    info_fd_cpu->logger = logger;
    pthread_create(&hiloCPU,NULL,(void*) conexionCPU,(void*) info_fd_cpu);
    pthread_detach(hiloCPU);
    //HILO PARA COMUNICACION CON KERNEL
    pthread_t hiloKernel;
    info_fd_hilos* info_fd_kernel = malloc(sizeof(info_fd_hilos));
    info_fd_kernel->fd = fd_kernel;
    info_fd_kernel->logger = logger;
    pthread_create(&hiloKernel,NULL,(void*) conexionKernel,(void*) info_fd_kernel);
    pthread_detach(hiloKernel);
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

    int codigoOperacion = recibir_operacion(fd_conexion_IO);

    if(codigoOperacion == -1)
    {
        log_error(logger,"Error al recibirOperacion");
        //return;
    }

    char* interfazConectada = recibir_mensaje(fd_conexion_IO,logger);
    recibir_handshake(logger,fd_conexion_IO,interfazConectada);
    free(interfazConectada);
}

void conexionCPU(void* info_fd_cpu)
{
    info_fd_hilos* fd_recibido = info_fd_cpu;
    int fd_cpu = fd_recibido->fd;
    t_log* logger = fd_recibido->logger;
    free(fd_recibido);

    int codigoOperacion;
    while(1)
    {
        codigoOperacion = recibir_operacion(fd_cpu);

        if(codigoOperacion == -1)
        {
            log_error(logger,"Error al recibirOperacion");
            return;
        }

		switch (codigoOperacion) {
		case HANDSHAKE:
            char* moduloConectado = recibir_mensaje(fd_cpu,logger);
			recibir_handshake(logger,fd_cpu, moduloConectado);
            free(moduloConectado);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            return;
			//return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			return;
		}
    }
}
void conexionKernel(void* info_fd_kernel)
{
    info_fd_hilos* fd_recibido = info_fd_kernel;
    int fd_kernel = fd_recibido->fd;
    t_log* logger = fd_recibido->logger;
    free(fd_recibido);

    int codigoOperacion;
    while(1)
    {
        codigoOperacion = recibir_operacion(fd_kernel);

        if(codigoOperacion == -1)
        {
            log_error(logger,"Error al recibirOperacion");
            return;
        }

		switch (codigoOperacion) {
		case HANDSHAKE:
            char* moduloConectado = recibir_mensaje(fd_kernel,logger);
			recibir_handshake(logger,fd_kernel, moduloConectado);
            free(moduloConectado);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            return;
			//return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			return;
		}
    }
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

