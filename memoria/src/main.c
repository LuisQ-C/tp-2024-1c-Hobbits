#include "../include/main.h"

t_log* logger;
t_config* config;
t_list* instrucciones_procesos;
pthread_mutex_t mutex_lista_procesos;


int main(int argc, char* argv[]) {
    
    //ESTO ANDA NO BORRARLO
    int server_fd = 0;
    int fd_cpu = 0;
    int fd_kernel = 0;


    logger = iniciar_logger("memoria.log", "MODULO MEMORIA", 1, LOG_LEVEL_DEBUG);
    config = iniciar_config("memoria.config",logger);
    ////////////////////////////////////////////////////////////////



    /////////////////////////////////////////////////////////////////
    instrucciones_procesos = list_create();
    pthread_mutex_init(&mutex_lista_procesos,NULL);
    //
    
    //
    if(!iniciar_conexiones(&server_fd,&fd_cpu,&fd_kernel))
    {
        log_error(logger,"Error al crear conexiones iniciales");
        exit(1);
    }

    realizar_handshakes_memoria(fd_cpu,fd_kernel);

    inicializar_hilos(fd_cpu,fd_kernel);

    //////
    //
    while(escucharConexionesIO(server_fd));
    terminar_programa(&fd_cpu,&fd_kernel);
    return 0; //puede ir exit_sucess
}

/*
int main(int argc, char* argv[]) {
    
    //ESTO ANDA NO BORRARLO
    int server_fd = 0;
    int fd_cpu = 0;
    int fd_kernel = 0;


    logger = iniciar_logger("memoria.log", "MODULO MEMORIA", 1, LOG_LEVEL_DEBUG);
    config = iniciar_config("memoria.config",logger);
    ////////////////////////////////////////////////////////////////

    void* user_space = inicializar_user_space(config);
    void* bitmap = inicializar_bitmap(config,user_space);
    t_tabla_pag* tablita = crear_tabla_paginas(4096,32);
    int marco = consultar_marco(15,tablita);
    if(marco == MARCO_INVALIDO)
    {
        log_error(logger,"CONSULTASTE UN MARCO INVALIDO PAPAH");
    }

    /////////////////////////////////////////////////////////////////
    return 0; //puede ir exit_sucess
}*/











