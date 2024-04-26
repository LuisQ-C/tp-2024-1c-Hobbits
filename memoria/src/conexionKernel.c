#include "../include/conexionKernel.h"

extern t_log* logger;

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
    while(1)
    {
        codigoOperacion = recibir_operacion(fd_kernel);

        if(codigoOperacion == -1)
        {
            log_error(logger,"Error al recibirOperacion");
            return;
        }

		switch (codigoOperacion) {
        case INICIAR_PROCESO:
            /* RECIBE EL PID Y EL PATH AL PID (SUMARLO A PATH DEL CONFIG PARA HALLAR LA RUTA ABSOLUTA)
            int pid = recibe
            char* pathPseudocodigo = recibir_mensaje(fd_kernel,logger);
            //se recibe el path del kernel
            char* pathPseudocodigo = "codigoPrueba.txt";
            FILE* archivoPseudocodigo = fopen(pathPseudocodigo,"r+");

            instrucciones = pasarArchivoEstructura(archivoPseudocodigo);
            fclose(archivoPseudocodigo);
            string_array_destroy(instrucciones);
            */
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