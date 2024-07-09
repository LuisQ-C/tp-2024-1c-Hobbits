#include "../include/conexionKernel.h"

extern t_log* logger;
extern t_config* config;
extern t_datos_memoria datos_memoria;
extern t_log* logger_obligatorio;

void realizar_handshake_kernel(int fd_kernel)
{
    if(recibir_operacion(fd_kernel)==HANDSHAKE)
    {
        char* moduloConectado = recibir_mensaje(fd_kernel,logger);
		enviar_handshake_ok(logger,fd_kernel, moduloConectado);
        free(moduloConectado);
    }
    else
    {
        log_error(logger,"Error al realizar handshake con kernel");
        abort();
    }
}

void iniciar_hilo_kernel(int fd_kernel)
{
    pthread_t hiloKernel;
    info_fd_kernel* info_fd = malloc(sizeof(info_fd_kernel));
    info_fd->fd = fd_kernel;
    pthread_create(&hiloKernel,NULL,(void*) conexionKernel,(void*) info_fd);
    pthread_detach(hiloKernel);
}

/* Funcion main del hilo KERNEL-MEMORIA */
void conexionKernel(void* info_fd)
{
    info_fd_kernel* fd_recibido = info_fd;
    int fd_kernel = fd_recibido->fd;
    free(fd_recibido);

    int retardo = datos_memoria.retardo_respuesta;
    int codigoOperacion;
    char* path_config = datos_memoria.path_instrucciones;
    
    while(1)
    {
        codigoOperacion = recibir_operacion(fd_kernel);

        if(codigoOperacion == DESCONEXION)
        {
            log_error(logger,"TE DESCONECTASTE FLACO (KERNEL)");
            return;
        }
        else if(codigoOperacion == ERROR)
        {
            log_warning(logger,"ERROR EN EL RECIBIR_OPERACION (KERNEL)");
            return;
        }

		switch (codigoOperacion) {
        case INICIAR_PROCESO:
        {
            int archivo_resultado;
            int pid;
            char* path_kernel;
            
            recibir_creacion_proceso(&pid,&path_kernel,fd_kernel);
            //log_debug(logger,"ruta archivo: %s",path_kernel);

            char* path_archivo = string_new();
            string_append(&path_archivo,path_config);
            string_append(&path_archivo,path_kernel);

            //ver
            //log_debug(logger,"ruta archivo: %s",path_archivo);
            FILE* archivo_pseudocodigo = fopen(path_archivo,"r");

            //FALLA CUANDO SE ESPECIFICA EL DIRECTORIO SIN NADA, EJ PSEUDOS/ O PSEUDOS
            if(archivo_pseudocodigo != NULL)
            {
                archivo_resultado = ARCHIVO_EXISTE;
                agregar_proceso_lista(pid,archivo_pseudocodigo); //cambiar agregar_proceso_lista y todas las funciones que lo usan
                log_info(logger_obligatorio,"Creación de Tabla de Páginas: \"PID: %d - Tamaño: 0\"",pid);
                usleep(retardo*1000);
                send(fd_kernel,&archivo_resultado,sizeof(int),0);
                fclose(archivo_pseudocodigo);
            }
            else
            {
                archivo_resultado = ARCHIVO_INVALIDO;
                usleep(retardo*1000);
                send(fd_kernel,&archivo_resultado,sizeof(int),0);
            }
            free(path_kernel);
            free(path_archivo);
            //por ahi solo es necesario si el archivo es valido, dentro del if
            break;
        }
        case FINALIZAR_PROCESO:
        {
            int pid;
            int rta;
            recibir_destruccion_proceso(&pid,fd_kernel);
            int cant_pag_eliminadas = quitar_proceso_lista(pid);
            usleep(retardo*1000);
            if(cant_pag_eliminadas == -1)
            {
                rta = PROCESO_INEXISTENTE;
                //send(fd_kernel,&rta,sizeof(int),0);
            }
            else{
                log_info(logger_obligatorio,"Destrucción de Tabla de Páginas: \"PID: %d - Tamaño: %d\"",pid,cant_pag_eliminadas);
                rta = PROCESO_ELIMINADO;
                //send(fd_kernel,&rta,sizeof(int),0);
            }
            break;
        }
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