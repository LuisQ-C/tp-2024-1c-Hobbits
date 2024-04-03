#include "../include/main.h"

int main(int argc, char* argv[]) {
    int fd_memoria = 0;
    int fd_cpu = 0;


    t_log* logger;
    t_config* config;

    logger = iniciar_logger();
    config = iniciar_config(logger);

    if(!generar_conexiones(logger,&fd_memoria,&fd_cpu,config)){
        exit(1);
    }
    liberar_conexion(fd_memoria);
    liberar_conexion(fd_cpu);

}
t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("kernel.log","Kernel",1,LOG_LEVEL_INFO);
	if(nuevo_logger == NULL){
		perror("No se pudo crear el logger!");
		abort();
	}
	return nuevo_logger;
}
t_config* iniciar_config(t_log* logger)
{
	t_config* nuevo_config;
	nuevo_config = config_create("kernel.config"); // PARA EJECUTAR EN CONSOLA: DESDE KERNEL -> ./BIN/KERNEL
	if(nuevo_config == NULL){
		log_error(logger,"No se pudo crear el config!");
		abort();
	}
	return nuevo_config;
}
