#include "../include/conexionCPU.h"

extern t_log* logger;
extern t_config* config;
extern t_list* instrucciones_procesos;

void realizar_handshake_cpu(int fd_cpu)
{
    if(recibir_operacion(fd_cpu)==HANDSHAKE)
    {
        char* moduloConectado = recibir_mensaje(fd_cpu,logger);
		enviar_handshake_ok(logger,fd_cpu, moduloConectado);
        free(moduloConectado);
    }else{
        log_error(logger,"Error al realizar el handshake con cpu");
        abort();
    }
}

void iniciar_hilo_cpu(int fd_cpu)
{
    pthread_t hiloCPU;
    info_fd_cpu* info_fd = malloc(sizeof(info_fd_cpu));
    info_fd->fd = fd_cpu;
    pthread_create(&hiloCPU,NULL,(void*) conexionCPU,(void*) info_fd);
    pthread_detach(hiloCPU);
}

void conexionCPU(void* info_fd)
{
    info_fd_cpu* fd_recibido = info_fd;
    int fd_cpu = fd_recibido->fd;
    free(fd_recibido);

    int retardo = config_get_int_value(config,"RETARDO_RESPUESTA");
    //char** instruccionesPrueba = pasarArchivoEstructura("codigoPrueba.txt");
    agregar_proceso_lista(14,"codigoPrueba.txt");
    //agregar_proceso_lista(7,"codigoPrueba2.txt");
    //ESTO DEBERIA IR EN MEMORIA LO DE AGREGARLO SEGUN LO QUE RECIBA

    int codigoOperacion;
    while(1)
    {
        codigoOperacion = recibir_operacion(fd_cpu);

        if(codigoOperacion == DESCONEXION)
        {
            log_error(logger,"TE DESCONECTASTE FLACO (CPU)");
            return;
        }
        else if(codigoOperacion == ERROR)
        {
            log_warning(logger,"ERROR EN EL RECIBIR_OPERACION (CPU)");
            return;
        }

		switch (codigoOperacion) {
        case INSTRUCCION:
            t_list* lista_auxiliar = recibir_paquete(fd_cpu);
            int* pid = list_get(lista_auxiliar,0);
            uint32_t* pc = list_get(lista_auxiliar,1);
            usleep(retardo*1000);
            t_proceso* proceso = buscar_proceso_pid(*pid);
            enviar_mensaje((proceso->instrucciones)[*pc],fd_cpu,INSTRUCCION);
            list_destroy_and_destroy_elements(lista_auxiliar,(void*) liberar_elemento);
            break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            return;
			//return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
    }
}