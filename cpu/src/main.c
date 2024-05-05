#include "../include/main.h"

t_log* logger;
t_config* config;
t_registro_cpu registro = {0,0,0,0,0,0,0,0,0,0,0};
pthread_mutex_t mutex_interrupcion;
pthread_mutex_t mutex_pid;
sem_t semaforo_pcb_recibido;


int HAY_INTERRUPCION = 0;
int PID_ACTUAL = -1;
int MOTIVO_DESALOJO = -1;

int main(int argc, char* argv[])
{
    //NO TOCAR ANDA PERFECTO
    int fd_conexion_memoria = 0;
    int server_fd_escucha_dispatch = 0;
    int server_fd_escucha_interrupt = 0;

    int cliente_fd_conexion_interrupt = 0;
    int cliente_fd_conexion_dispatch = 0;
    logger = iniciar_logger("cpu.log", "MODULO CPU", 1, LOG_LEVEL_DEBUG); //QUEDA PARA SIEMPRE
    config = iniciar_config("cpu.config",logger); //DESTRUIRLO CUANDO TERMINEMOS DE LEER LOS DATOS
    //INICIALIZO EL MUTEX Y EL SEMAFORO
    pthread_mutex_init(&mutex_interrupcion,NULL);
    pthread_mutex_init(&mutex_pid,NULL);
    sem_init(&semaforo_pcb_recibido,0,0);
   
    if(!iniciar_conexiones(&fd_conexion_memoria,&server_fd_escucha_dispatch,&server_fd_escucha_interrupt,&cliente_fd_conexion_dispatch,&cliente_fd_conexion_interrupt))
    {
        log_error(logger,"Error al crear conexiones iniciales");
        exit(1);
    }

    realizar_handshakes_cpu(fd_conexion_memoria,cliente_fd_conexion_dispatch,cliente_fd_conexion_interrupt);

    //memoria tambien queda habilitado porque mandarHandshake es bloqueante
    habilitar_dispatch_interrupt(cliente_fd_conexion_dispatch,cliente_fd_conexion_interrupt,fd_conexion_memoria);

    
    
    //destruyo todo
    pthread_mutex_destroy(&mutex_interrupcion);
    pthread_mutex_destroy(&mutex_pid);
    sem_destroy(&semaforo_pcb_recibido);
    terminar_programa(&fd_conexion_memoria,&cliente_fd_conexion_dispatch,&cliente_fd_conexion_interrupt);
    //
    return 0;
}