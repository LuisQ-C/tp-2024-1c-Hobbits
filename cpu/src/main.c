#include "../include/main.h"

t_log* logger;
int main(int argc, char* argv[]) {
    
    logger = iniciar_logger("cpu.log", "MODULO CPU", 1, LOG_LEVEL_DEBUG);
    int server_fd = iniciar_servidor(logger,"0.0.0.0","8097");
    log_info(logger,"Servidor listo para recibir cliente!");
    
    while(1){
        int cliente_fd = esperar_cliente(server_fd,logger);
    }
    log_destroy(logger);//liberamos el logger, preguntar si hay que hacer close cuando cerramos servidor
    return 0;
}