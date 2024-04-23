#include "../include/main.h"
t_log* logger;

int main(int argc, char* argv[]) {
    int fd_conexion_kernel = 0;
    int fd_conexion_memoria = 0;
   // t_log* logger;
    t_config* config;
    logger = iniciar_logger("entrada_salida.log","MODULO INTERFAZ",1,LOG_LEVEL_DEBUG);
    config = iniciar_config("entrada_salida.config",logger);
    if(!iniciar_conexiones(logger,config,&fd_conexion_kernel,&fd_conexion_memoria))
    {
        log_error(logger,"Error al crear conexiones iniciales");
        exit(1);
    }
    mandarHandshake(logger,fd_conexion_kernel,"MODULO KERNEL");
    mandarHandshake(logger,fd_conexion_memoria,"MODULO MEMORIA");
    manejarConexion(logger,&fd_conexion_kernel,&fd_conexion_memoria);
    terminar_programa(logger,config,&fd_conexion_kernel,&fd_conexion_memoria);
      //pedir nombre
      //scanf
    //agregar iniciar interfaz
  

   return 0;

//***interfaz generica***//

//recibo peticion de kernel??? LA PETICION ESTA EN EL CONFIG O EN EL KERNEL, O KERNEL ESCRIBE EL CONFIG?
//de kernel recibo cantidad de unidades de trabajo
//la interfaz generica solo recibe IO_GEN_SLEEP
//leo config
//miro el tipo de interfaz, este caso, generica
//

}




void iniciarInterfaz(char nombre,  t_config* config, int fd_conexion_kernel )// o hacerlo global? deberiamos agregar el fd de kernel, despues tambien habria que agregar el de la memoria
{
    
char* tipoDeInterfaz;
//int* tiempoDeUnidadDeTrabajo;

tipoDeInterfaz = config_get_string_value(config, "TIPO_INTERFAZ");
//tiempoDeUnidadDeTrabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");

if (!(strcmp(tipoDeInterfaz, "GENERICO")))
{
    //log_info(logger,"La interfaz recibida es: GENERICA");
    interfazGenerica(config, fd_conexion_kernel);
    

}
}

void interfazGenerica( t_config* config, int fd_conexion_kernel,t_log* logger)//pasa el config para no leer cada vez que inicia una interfaz todo completo
{
    int tiempoDeUnidadDeTrabajo;
    int cantidad_unidad_trabajo;
    tiempoDeUnidadDeTrabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
    
    recv(fd_conexion_kernel,&cantidad_unidad_trabajo,sizeof(int),MSG_WAITALL);
        int resultado = cantidad_unidad_trabajo * tiempoDeUnidadDeTrabajo; //cantidad viene de kernel
        usleep(resultado);
        int finalizado = 4; // aca deberia ir el valor del ENUM INTERFAZLISTA
        log_info(logger,"Finalizo el tiempo en espera de la interfaz generica");
        send(fd_conexion_kernel,&finalizado,sizeof(int),0);
        


}
