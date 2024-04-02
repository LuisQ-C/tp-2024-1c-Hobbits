#include "../include/main.h"

int main(int argc, char* argv[]) {
    int conexion;
    char* ip;
    char* puerto;
    

    t_log* logger;
    t_config* config;

    logger = iniciar_logger();
    config = iniciar_config(logger);
    
    //paso datos de ip y puerto
    ip = config_get_string_value(config,"IP_MEMORIA");
    puerto = config_get_string_value(config,"PUERTO_MEMORIA");
    log_info(logger,"Lei la IP %s y el puerto %s", ip, puerto);

    //EL SERVIDOR YA ESTA CORRIENDO

    conexion = crear_conexion(ip,puerto);

    liberar_conexion(conexion);

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
