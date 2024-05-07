#include "../include/main.h"

t_log* logger;
t_config* config;

typedef struct
{
    uint8_t AX;
    uint8_t BX;
    uint8_t CX;
    uint8_t DX;
    uint32_t EAX;
    uint32_t EBX;
    uint32_t ECX;
    uint32_t EDX;
}registrosGenerales;
typedef struct
{
   int pid;
   uint32_t pc;
   int quantum;
   char* estado;
   registrosGenerales* registrosCPU;
} pcb;


int main(int argc, char* argv[]) {
    int fd_memoria = 0;
    int fd_cpu_dispatch = 0;
    int fd_cpu_interrupt = 0;
    int fd_escucha_interfaces = 0;

    logger = iniciar_logger("kernel.log","Kernel",1,LOG_LEVEL_DEBUG);
    config = iniciar_config("kernel.config",logger);

    if(!iniciar_conexiones(config,logger,&fd_memoria,&fd_cpu_dispatch,&fd_cpu_interrupt,&fd_escucha_interfaces))
    {
        log_error(logger,"Error al crear conexiones iniciales");
        exit(1);
    };

    realizar_handshakes_kernel(fd_memoria,fd_cpu_dispatch,fd_cpu_interrupt);
    
    info_fd* info_fd_consola = malloc(sizeof(info_fd));
    info_fd_consola->fd_cpu_dispatch = fd_cpu_dispatch;
    info_fd_consola->fd_cpu_interrupt = fd_cpu_interrupt;
    info_fd_consola->fd_memoria = fd_memoria;
    info_fd_consola->fd_escucha_interfaces = fd_escucha_interfaces;


    iniciar_cosas_necesarias_planificador();
    //Llamamos la inicialización de la consola
    //iniciar_PLP();
    pthread_t hilo_consola;
    pthread_create(&hilo_consola, NULL, (void*) iniciar_consola, (void *) info_fd_consola);
    pthread_detach(hilo_consola);
    //Hilo Dispatch
    /*
    pthread_t hilo_dispatch;
    pthread_create(&hilo_dispatch,NULL,(void*) conexion_dispatch,(void*) dispatch);
    pthread_detach(hilo_dispatch);*/

    
    while(escucharConexionesIO(logger,fd_escucha_interfaces));

    terminar_programa(logger,config,&fd_memoria,&fd_cpu_dispatch,&fd_cpu_interrupt);
    return 0;
}













/*
*Consola interactiva en la cual el usuario ingresa lo que quiere realizar (Hilo)
*Diagrama de estados: cinco estados (new,ready,exec,blocked,exit), habra un cola por cada una?
    *Dentro de blocked habra multiples colas (una por cada IO conectada, una por cada recurso que administra el kernel)
*El kernel crea el PCB con los datos minimos: PID,PC,QUANTUM,REGISTROS CPU y se pueden aniadir mas(estado por ej)
    *Se manda el PCB al cpu a traves de la conexion DISPATCH
*Planificador de largo plazo (Creacion y eliminacion procesos) OTRO HILO?
    *Creacion proceso: Se debe comunicar a memoria que debe crear un proceso correspondiente a archivo de pseudocodigo pasado
    por parametro, inica sin espacio en memoria, tabla paginas vacia. Si el grado de multiprogr. lo permite pasa de new a ready, sino no.
    *Eliminacion procesos: Si un proceso llega a EXIT (por solicitud CPU, por error, o por ejecucion desde consola interact.) el kernel
    solicita a memoria liberar estructuras asociadas al proceso y marcar como libre el espacio que ocupaba.
        *Si el proceso se encontraba exec en CPU, mandar senial interrupcion por INTERRUPT y esperar a que retorne el contexto antes
        de liberar recursos.
*Planificador corto plazo: Proceso en READY se planificaran por FIFO, ROUNDROBIN, O VRR OTRO HILOS?
    *Se elige el siguiente proceso que pasa a exec (CPU), se le envia el contexto al cpu a traves de DISPATCH, kernel
    queda a la espera de recibir el context actualizado junto con MOTIVO DESALOJO por el cual fue desplazado a manejar??
    *Si el algoritmo utilizado requiere desalojar al proceso, se enviar una interrupcion por INTERRUPT
    *Si el motivo de desalojo al recibir el context fue "replanificar" se replanifica definiendo el prox proceso que pasa a exec.
    La cpu se queda esperando
*Manejo recursos: En el config contamos con RECURSOS (lista con recurs. disp) y INSTANCIA RECURSOS (cant instancias en lista ordenada como la anterior)
    *El kernel recibe context ejecucion desalojado por WAIT, kernel verifica que exista recurso y resta 1 instancia si es asi. Si no se puede restar,
    el proceso se suma a la cola del recurso en BLOCKED.
    *El kernel recibe context ejecuion desalojado por SIGNAL, kernel verifica que recurso existe y suma 1 instancia si es asi. Si habia un proceso esperando
    este recurso lo saca de BLOCKED. Se devuelve a ejecucion el proceso que peticiona SIGNAL, se lo devuelve a ready?
*/
