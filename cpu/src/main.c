#include "../include/main.h"

t_log* logger;
int main(int argc, char* argv[]) {
    
    logger = log_create("cpu.log", "MODULO CPU", 1, LOG_LEVEL_DEBUG);
    int server_fd = iniciar_servidor(logger,"0.0.0.0","8097");
    log_info(logger,"Servidor listo para recibir cliente!");
    
    while(1){
        int cliente_fd = esperar_cliente(server_fd,logger);
    }
    return 0;
}