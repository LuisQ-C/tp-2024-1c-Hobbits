#include "../include/main.h"
 //el extern que va por delante es cuando queremos utilizar este logger en muchos archivos o lugares, pra no redeclarar lo mismo
t_log* logger;
t_config* config;

typedef struct main
{
    int fd;
    t_log* logger;
}info_fd_hilos;

int main(int argc, char* argv[]) {
    FILE* archivoPseudocodigo = fopen("codigoPrueba.txt","r+");
    //char* instruccion = leerCadenaArchivos(f);
    char** instrucciones = pasarArchivoEstructura(archivoPseudocodigo);
    fclose(archivoPseudocodigo);

    //string_array_destroy(instrucciones);
    //ESTO ANDA NO BORRARLO
    int server_fd = 0;
    int fd_cpu = 0;
    int fd_kernel = 0;

    logger = iniciar_logger("memoria.log", "MODULO MEMORIA", 1, LOG_LEVEL_DEBUG);
    config = iniciar_config("memoria.config",logger);
    //
    
    //
    if(!iniciar_conexiones(logger,config,&server_fd,&fd_cpu,&fd_kernel))
    {
        log_error(logger,"Error al crear conexiones iniciales");
        exit(1);
    }
    //recibirHandshake(logger,fd_cpu,"MODULO CPU",1);
    //manejarConexionCPU(logger,&fd_cpu);

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
    //////
    /*
    uint32_t pc_recibido;
    recv(fd_cpu, &pc_recibido, sizeof(int32_t), MSG_WAITALL);
    enviar_mensaje(instrucciones[pc_recibido],fd_cpu,INSTRUCCION);
    */
    //DESTRUIR ESTRUCTURA CON SUS STRINGS DENTRO
    //string_array_destroy(instrucciones);
    //
    while(escucharConexionesIO(logger,server_fd));
    terminar_programa(logger,config,&fd_cpu,&fd_kernel);
    return 0; //puede ir exit_sucess
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
        //SI recv devuelve 0 significa que la conexion se cerro del otro lado, del lado del kernel
        /*if(bytes==0)
        {
            log_error(logger, "el cliente se desconecto. Terminando servidor");
            return;
        }
        //SI recv retorna -1 significa que hubo un error mas grave
        else if(bytes==-1)
        {
            log_error(logger,"error de fd_conexion_dispatch en CPU");
            return;
        }*/
        if(codigoOperacion == -1)
        {
            log_error(logger,"Error de fd_conexion_dispatch en CPU");
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
            break;
			//return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
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
        //SI recv devuelve 0 significa que la conexion se cerro del otro lado, del lado del kernel
        /*if(bytes==0)
        {
            log_error(logger, "el cliente se desconecto. Terminando servidor");
            return;
        }
        //SI recv retorna -1 significa que hubo un error mas grave
        else if(bytes==-1)
        {
            log_error(logger,"error de fd_conexion_dispatch en CPU");
            return;
        }*/
        if(codigoOperacion == -1)
        {
            log_error(logger,"Error de fd_conexion_memoria en KERNEL");
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
            break;
			//return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
    }
}










