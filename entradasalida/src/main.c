#include "../include/main.h"

int main(int argc, char* argv[]) {
    t_log* logger;
    t_config* config;
    logger = iniciar_logger("entrada_salida.log","MODULO INTERFAZ",1,LOG_LEVEL_DEBUG);
    config = iniciar_config("entrada_salida.config",logger);
    conectar_interfaz(logger,config);

    
   return 0;
}

