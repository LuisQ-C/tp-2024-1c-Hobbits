#include "../include/main.h"
 //el extern que va por delante es cuando queremos utilizar este logger en muchos archivos o lugares, pra no redeclarar lo mismo
t_log* logger;
t_config* config;
int server_fd = 0;
int fd_cpu = 0;
int fd_kernel = 0;

int main(int argc, char* argv[]) {

    logger = iniciar_logger("memoria.log", "MODULO MEMORIA", 1, LOG_LEVEL_DEBUG);
    config = iniciar_config("memoria.config",logger);
    iniciar_conexiones(logger,config,&server_fd,&fd_cpu,&fd_kernel);
    //recibirHandshake(logger,fd_cpu,"MODULO CPU",1);
    manejarConexionCPU();
    while(escucharConexionesIO(logger,server_fd));
    terminar_programa(logger,config,&fd_cpu,&fd_kernel);
    return 0; //puede ir exit_sucess
}
void manejarConexionCPU()
{
    int cod_op;
    while(1)
    {
        recv(fd_cpu, &cod_op, sizeof(cod_op), MSG_WAITALL);
		switch (cod_op) {
		case HANDSHAKE:
			recibir_handshake(logger,fd_cpu,"MODULO CPU");
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            break;
			//return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
        break;
    }
}



/*
    pthread_t hiloKernel;
    datos* args = malloc(sizeof(datos));
    args->fd = esperar_cliente(server_fd,logger);
    pthread_create(&hiloKernel,
                    NULL,
                    atender_cliente,
                    (void*) args);
    pthread_join(hiloKernel,NULL);
void* atender_cliente(void* args)
{
    datos* args2 = (datos*) args;
    int fd_conexion = args2->fd;
    while(1)
    {
        int32_t handshake = 6;
        int32_t resultOk = 0;
        int32_t resultError = -1;
        printf("El numero de hanshake antes de recibir: %d",handshake);
        recv(fd_conexion, &handshake, sizeof(int32_t), MSG_WAITALL);
        printf("El numero de hanshake recibido es: %d",handshake);
        if (handshake == 1) {
            send(fd_conexion, &resultOk, sizeof(int32_t), 0);
        } else {
            send(fd_conexion, &resultError, sizeof(int32_t), 0);
        }
        break;
    }
    close(fd_conexion);
    return NULL;
}*/




