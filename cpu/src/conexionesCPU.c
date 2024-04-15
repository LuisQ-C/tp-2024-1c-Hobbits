#include "../include/conexionesCPU.h"

typedef struct
{
    int fd_conexion;
}datos;

int iniciar_conexiones(t_log* logger,t_config* config,int* fd_conexion_memoria,int* server_fd_escucha_dispatch, int* server_fd_escucha_interrupt, int* cliente_fd_conexion_dispatch, int* cliente_fd_conexion_interrupt)
{
    char* ip;
    char* puerto;
    //PRIMERO SE CONECTA A MEMORIA
    ip=config_get_string_value(config,"IP_MEMORIA");
    puerto=config_get_string_value(config,"PUERTO_MEMORIA");
    *fd_conexion_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    //LUEGO EMPIEZA A ESCUCHAR CONEXIONES ENTRANTES
    puerto=config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
    *server_fd_escucha_dispatch = iniciar_servidor(logger,"0.0.0.0",puerto);
    log_info(logger,"Servidor escuchando en puerto dispatch");
    puerto=config_get_string_value(config,"PUERTO_ESCUCHA_INTERRUPT");
    *server_fd_escucha_interrupt = iniciar_servidor(logger,"0.0.0.0",puerto);
    log_info(logger,"Servidor escuchando en puerto interrupt");
    //ESPERA A QUE SE LE CONECTE EL KERNEL
    *cliente_fd_conexion_dispatch = esperar_cliente(*server_fd_escucha_dispatch,logger,"KERNEL_DISPATCH");
    *cliente_fd_conexion_interrupt = esperar_cliente(*server_fd_escucha_interrupt,logger,"KERNEL_INTERRUPT");
    
    return *fd_conexion_memoria != 0 && *server_fd_escucha_dispatch != 0 && *server_fd_escucha_interrupt != 0 && *cliente_fd_conexion_dispatch != 0 && *cliente_fd_conexion_interrupt != 0;
}


void manejarConexionKernel(t_log* logger,int* cliente_fd_conexion_dispatch,int* cliente_fd_conexion_interrupt)
{
    int cod_op, bytes;
    while(1)
    {
        bytes = recv(*cliente_fd_conexion_dispatch, &cod_op, sizeof(cod_op), MSG_WAITALL);
        //SI recv devuelve 0 significa que la conexion se cerro del otro lado, del lado del kernel
        if(bytes==0)
        {
            log_error(logger, "el cliente se desconecto. Terminando servidor");
            return;
        }
        //SI recv retorna -1 significa que hubo un error mas grave
        else if(bytes==-1)
        {
            log_error(logger,"error de fd_conexion_dispatch en CPU");
            return;
        }
		
        switch (cod_op) {
        case PROCESO:
            log_info(logger,"proceso recibido");
		case HANDSHAKE:
			recibir_handshake(logger,*cliente_fd_conexion_dispatch,"MODULO KERNEL-DISPATCH");
            recibir_handshake(logger,*cliente_fd_conexion_interrupt,"MODULO KERNEL-INTERRUPT");
            break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            break;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
        
    }
}

void terminar_programa(t_log* logger, t_config* config, int* fd_conexion_memoria,int* cliente_fd_conexion_dispatch,int*cliente_fd_conexion_interrupt)
{
    destruir_log_config(logger,config);
    close(*fd_conexion_memoria);
    close(*cliente_fd_conexion_dispatch);
    close(*cliente_fd_conexion_interrupt);
}