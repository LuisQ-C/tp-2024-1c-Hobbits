#include "../include/conexionKernel.h"

extern t_log* logger;
extern t_config* config;

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

void conexionKernel(void* info_fd)
{
    info_fd_kernel* fd_recibido = info_fd;
    int fd_kernel = fd_recibido->fd;
    free(fd_recibido);

    int codigoOperacion;
    char* path_config = config_get_string_value(config,"PATH_INSTRUCCIONES");
    
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
            int archivo_existe = ARCHIVO_EXISTE;
            int archivo_invalido = ARCHIVO_INVALIDO;
            t_list* lista_aux = recibir_paquete(fd_kernel);
            int* pid = list_get(lista_aux,0);
            char* path_kernel = list_get(lista_aux,1);
            log_debug(logger,"ruta archivo: %s",path_kernel);
            char* path_archivo = string_new();
            string_append(&path_archivo,path_config);
            string_append(&path_archivo,path_kernel);

            //ver
            log_debug(logger,"ruta archivo: %s",path_archivo);
            FILE* archivo_pseudocodigo = fopen(path_archivo,"r");
            if(archivo_pseudocodigo != NULL)
            {
                agregar_proceso_lista(*pid,archivo_pseudocodigo); //cambiar agregar_proceso_lista y todas las funciones que lo usan
                send(fd_kernel,&archivo_existe,sizeof(int),0);
                fclose(archivo_pseudocodigo);
            }
            else
            {
                send(fd_kernel,&archivo_invalido,sizeof(int),0);
            }
            list_destroy_and_destroy_elements(lista_aux,(void*) liberar_elemento);
            free(path_archivo);
             //por ahi solo es necesario si el archivo es valido, dentro del if
            break;
        case FINALIZAR_PROCESO:
            /*HARIA FALTA MANDARL CONFIRMACION O ERROR A KERNEL??*/
            char* pid_recibido = recibir_mensaje(fd_kernel,logger);
            int pid_XD = atoi(pid_recibido);
            quitar_proceso_lista(pid_XD);
            /*MARCAR FRAMES COMO LIBRES SIN SOBREESCRIBIR*/
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