#include "../include/conexionesIO.h"

int iniciar_conexiones(t_log* logger, t_config* config,int* fd_conexion_kernel,int* fd_conexion_memoria){
    char* ip;
    char* puerto;
    //CONEXION A KERNEL
    ip = config_get_string_value(config,"IP_KERNEL");
    puerto = config_get_string_value(config,"PUERTO_KERNEL");
    *fd_conexion_kernel = crear_conexion(ip,puerto,logger,"KERNEL");
//AGREGADO conviene que este debajo de conexion a memoria
//////////Aca leo todo junto???
char* tipoDeInterfaz;
char* tiempoDeUnidadDeTrabajo;

tipoDeInterfaz = config_get_string_value(config, "TIPO_INTERFAZ");
tiempoDeUnidadDeTrabajo = config_get_string_value(config, "TIEMPO_UNIDAD_TRABAJO");

if (!(strcmp(tipoDeInterfaz, GENERICO)))
{
    log_info(logger,"La interfaz recibida es: GENERICA");
    realizarDemora(tiempoDeUnidadDeTrabajo)   //IO_GEN_SLEEP???? creo la funcion?

}


//////////
//////////

    //CONEXION A MEMORIA
    ip = config_get_string_value(config,"IP_MEMORIA");
    puerto = config_get_string_value(config,"PUERTO_MEMORIA");
    *fd_conexion_memoria = crear_conexion(ip,puerto,logger,"MEMORIA");
    
    return *fd_conexion_kernel != 0 && *fd_conexion_memoria != 0;
}

void manejarConexion(t_log* logger,int* fd_conexion_kernel,int* fd_conexion_memoria)
{
    int cod_op, bytes;
    while(1)
    {
        bytes = recv(*fd_conexion_kernel, &cod_op, sizeof(cod_op), MSG_WAITALL);
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
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            break;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
        
    }
}

void terminar_programa(t_log* logger, t_config* config,int* fd_conexion_kernel,int* fd_conexion_memoria)
{
    destruir_log_config(logger,config);
    close(*fd_conexion_kernel);
    close(*fd_conexion_memoria);
}
