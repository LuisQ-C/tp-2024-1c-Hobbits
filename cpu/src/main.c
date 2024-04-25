#include "../include/main.h"

t_log* logger;
t_config* config;
t_registro_cpu registro = {0,0,0,0,0,0,0,0,0,0,0};



int main(int argc, char* argv[])
{
    /*
    int AX = 12;
    set_8(&registro.ax,0);
    //set_8(&registro.bx,4);
    //set_32(&registro.eax,32);
    printf("%d\n",AX);
    printf("%d\n",registro.ax);
    printf("%d\n",registro.pc);
    //printf("%d\n",registro.bx);
    //printf("%d\n",registro.eax);

    jnz(registro.ax,1);
    printf("%d",registro.pc);
    //sub_8(&registro.ax,registro.bx);
    //sub_32(&registro.eax,registro.bx);

    //printf("%d\n",registro.ax);
    //printf("%d\n",registro.eax);

    */



    //char* instruccion= string_new();

    //NO TOCAR ANDA PERFECTO
    int fd_conexion_memoria = 0;
    int server_fd_escucha_dispatch = 0;
    int server_fd_escucha_interrupt = 0;
    int cliente_fd_conexion_interrupt = 0;
    int cliente_fd_conexion_dispatch = 0;
    logger = iniciar_logger("cpu.log", "MODULO CPU", 1, LOG_LEVEL_DEBUG); //QUEDA PARA SIEMPRE 
    config = iniciar_config("cpu.config",logger); //DESTRUIRLO CUANDO TERMINEMOS DE LEER LOS DATOS
   
    if(!iniciar_conexiones(&fd_conexion_memoria,&server_fd_escucha_dispatch,&server_fd_escucha_interrupt,&cliente_fd_conexion_dispatch,&cliente_fd_conexion_interrupt))
    {
        log_error(logger,"Error al crear conexiones iniciales");
        exit(1);
    }
    mandarHandshake(logger,fd_conexion_memoria,"MODULO MEMORIA","CPU");
    //
    
    //log_info(logger,"%u",registro.BX);
    //log_info(logger,"%u",registro.AX);
    //log_info(logger,"%u",registro.BX);
    //CONEXION DISPATCH - SIN HILO POR SER SECUENCIAL
    
    //INICIALIZAR HILO INTERRUPT
    inicializar_hilo_interrupt(cliente_fd_conexion_interrupt);

    manejarConexionDispatch(cliente_fd_conexion_dispatch);
    
    
    //deshabilitar esto para tomas y luis-roger

    realizarCicloInstruccion(fd_conexion_memoria);
    log_info(logger,"VALOR REGISTRO AX: %u",registro.AX);
    log_info(logger,"VALOR REGISTRO BX: %u",registro.BX);
    log_info(logger,"VALOR REGISTRO CX: %u",registro.CX);
    log_info(logger,"VALOR PC: %u",registro.PC);
    ///// 
    
    //liberar los mensajes
    //manejarConexionKernel(logger,&cliente_fd_conexion_dispatch,&cliente_fd_conexion_interrupt);
    int cod_op;
    recv(cliente_fd_conexion_dispatch, &cod_op, sizeof(cod_op), MSG_WAITALL);
    terminar_programa(&fd_conexion_memoria,&cliente_fd_conexion_dispatch,&cliente_fd_conexion_interrupt);
    //hola
    return 0;
}