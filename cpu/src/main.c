#include "../include/main.h"

t_log* logger;
t_log* logger_obligatorio;
t_config* config;
t_registro_cpu registro = {0,0,0,0,0,0,0,0,0,0,0};
pthread_mutex_t mutex_interrupcion;
pthread_mutex_t mutex_pid;
sem_t semaforo_pcb_recibido;
config_memoria config_mem;
t_tlb tlb_scheduler;


int MOTIVO_INTERRUPCION = -1;
int PID_ACTUAL = -1;
int MOTIVO_DESALOJO = -1;
int INSTANTE_REFERENCIA = 0;
int PID_INTERRUMPIR =-1;

// NO BORRAR NUNCA


int main(int argc, char* argv[])
{
    //NO TOCAR ANDA PERFECTO
    int fd_conexion_memoria = 0;
    int server_fd_escucha_dispatch = 0;
    int server_fd_escucha_interrupt = 0;
    

    int cliente_fd_conexion_interrupt = 0;
    int cliente_fd_conexion_dispatch = 0;
    logger = iniciar_logger("cpu.log", "MODULO CPU", 1, LOG_LEVEL_TRACE); //QUEDA PARA SIEMPRE
    logger_obligatorio = iniciar_logger("memoria_oblig.log","MODULO CPU-OBL",1,LOG_LEVEL_DEBUG);
    config = iniciar_config("cpu.config",logger); //DESTRUIRLO CUANDO TERMINEMOS DE LEER LOS DATOS
    iniciar_tlb();
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
    recibir_datos_config_memoria(&config_mem.tam_memoria,&config_mem.tam_pagina,&config_mem.retardo_memoria,fd_conexion_memoria,logger);
    calcular_marcos();


    //PRUEBA 

    


   // printf("\nTAM_MEMORIA: %d\n",parametros_mem.tam_memoria);
    //printf("\nTAM_PAGINA: %d\n",parametros_mem.tam_pagina);
    //printf("\nCANT_MARCOS: %d\n",parametros_mem.cant_marcos);
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

// MAIN DE PRUEBAS 
/*
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
    
    iniciar_tlb(&config_mem.cant_entradas);
    agregar_entrada_tlb(0,1,1);
    agregar_entrada_tlb(0,2,3);
    agregar_entrada_tlb(0,3,4);
    agregar_entrada_tlb(0,5,7);
    //agregar_entrada_tlb(0,8,9);
    int i = buscar_entrada_tlb(0,1);
    i = buscar_entrada_tlb(0,1);
    i = buscar_entrada_tlb(0,2);
    i = buscar_entrada_tlb(0,3);
    i = buscar_entrada_tlb(0,5);
    //i = buscar_entrada_tlb(0,8);
    i = buscar_entrada_tlb(0,1);
    i = buscar_entrada_tlb(0,2);

    //recibir_datos_config_memoria(&config_mem.tam_memoria,&config_mem.tam_pagina,fd_conexion_memoria,logger);

    printf("\nPID: 0  PAGINA: 2  MARCO: %d \n",i);

    i = buscar_entrada_tlb(5,2);

    printf("\nPID: 0  PAGINA: 2  MARCO: %d \n",i);


    void _list_elements(t_entrada_tlb* entrada) {
                printf("\nPID: %d  PAGINA: %d  MARCO: %d INSTANTE: %d \n",entrada->pid,entrada->pagina,entrada->marco,entrada->instante_referencia);
                }

    list_iterate(tlb_scheduler.tlb->elements, (void*) _list_elements);


    
    
    //INICIALIZO EL MUTEX Y EL SEMAFORO
    //config_mem.tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
    //config_mem.tam_pagina = config_get_int_value(config,"TAM_PAGINA");
    //printf("\nTAM_PAGINA: %d\n",config_mem.tam_pagina);
    //printf("\nTAM_MEMORIA: %d\n",config_mem.tam_memoria);
    //calcular_marcos();
    




    
    //t_list* lista_frames = list_create();

    int frame = 2;
    int frame2 = 120;

    //list_add(lista_frames,&frame);
    //list_add(lista_frames,&frame2);

    //t_list* lista_recorrer = crear_paquete_escritura(63,12,lista_frames,2);
    


    //list_iterate(lista_recorrer, (void*) _list_elements);
   
    
    //t_list* lista_pruebas = armar_solicitud_frames(30,10,fd_conexion_memoria);

    

    //list_iterate(lista_pruebas, (void*) _list_elements);


    
    recibir_operacion(fd_conexion_memoria);
    t_list* paquete = recibir_paquete(fd_conexion_memoria);
    t_prueba* new = list_get(paquete,0);
    t_prueba* new2 = list_get(paquete,1);
    
    printf("\nVALOR A1: %d\n",new->a);
    printf("\nVALOR B1: %d\n",new->b);
    printf("\nVALOR B1: %d\n",new->c);
    printf("\nVALOR A2: %d\n",new2->a);
    printf("\nVALOR B2: %d\n",new2->b);
    printf("\nVALOR B2: %d\n",new2->c);

    return 0;
}
*/