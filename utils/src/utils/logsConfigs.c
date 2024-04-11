#include "../include/logsConfigs.h"


t_log* iniciar_logger(char* nombreArchivo, char* nombreProceso,int mostrarPorConsola,t_log_level nivel)
{
    t_log* nuevo_logger;
    nuevo_logger = log_create(nombreArchivo,nombreProceso,mostrarPorConsola,nivel);
    if(nuevo_logger == NULL)
    {
        printf("Error al crear el logger.");
        abort();
    }
    return nuevo_logger;
}
t_config* iniciar_config(char* rutaConfig,t_log* logger)
{
	t_config* nuevo_config;
	nuevo_config = config_create(rutaConfig); // PARA EJECUTAR EN CONSOLA: DESDE KERNEL -> ./BIN/KERNEL
	if(nuevo_config == NULL){
		log_error(logger,"Error al crear el config!");
		abort();
	}
	return nuevo_config;
}
void destruir_log_config(t_log* logger,t_config* config)
{
    log_destroy(logger);
    config_destroy(config);
}