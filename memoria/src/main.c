#include "../include/main.h"
 //el extern que va por delante es cuando queremos utilizar este logger en muchos archivos o lugares, pra no redeclarar lo mismo
t_log* logger;
t_config* config;


int main(int argc, char* argv[]) {
    int server_fd = 0;
    int fd_cpu = 0;
    int fd_kernel = 0;

    logger = iniciar_logger("memoria.log", "MODULO MEMORIA", 1, LOG_LEVEL_DEBUG);
    config = iniciar_config("memoria.config",logger);
    if(!iniciar_conexiones(logger,config,&server_fd,&fd_cpu,&fd_kernel))
    {
        log_error(logger,"Error al crear conexiones iniciales");
        exit(1);
    }
    //recibirHandshake(logger,fd_cpu,"MODULO CPU",1);
    manejarConexionCPU(logger,&fd_cpu);
    recibir_handshake(logger,fd_kernel,"MODULO KERNEL");
    while(escucharConexionesIO(logger,server_fd));
    terminar_programa(logger,config,&fd_cpu,&fd_kernel);
    return 0; //puede ir exit_sucess
}









