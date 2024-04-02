#include "main.h"
 //el extern que va por delante es cuando queremos utilizar este logger en muchos archivos o lugares, pra no redeclarar lo mismo
 t_log* logger;
int main(int argc, char* argv[]) {
    //char* puerto = "4444";
    //iniciar_memoria();
    logger = log_create("hola.log", "Servidor", 1, LOG_LEVEL_DEBUG);
    int server_fd = iniciar_servidor(logger);
    log_info(logger,"Servidor listo para recibir cliente!");
    int cliente_fd = esperar_cliente(server_fd,logger);
    return 0;
}




