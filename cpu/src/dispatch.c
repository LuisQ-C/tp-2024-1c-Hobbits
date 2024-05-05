#include "../include/dispatch.h"
extern t_log* logger;

void realizar_handshake_dispatch(int cliente_fd_conexion_dispatch)
{
    //HAY Q MANEJARLE LOS ERRORES, HACERLO EN RECIBIR_OPERACION
    if(recibir_operacion(cliente_fd_conexion_dispatch)==HANDSHAKE)
    {
        char* moduloConectado = recibir_mensaje(cliente_fd_conexion_dispatch,logger);
        enviar_handshake_ok(logger,cliente_fd_conexion_dispatch,moduloConectado);
        free(moduloConectado);
    }else
    {
        log_error(logger,"Error al realizar el handshake cpu_dispatch");
        abort();
    }
    
}
void manejarConexionDispatch(int cliente_fd_conexion_dispatch,int fd_conexion_memoria)
{
    int bytes;
    
    
    while(1)
    {
    bytes = recibir_operacion(cliente_fd_conexion_dispatch);

    if(bytes == DESCONEXION)
    {
        log_error(logger,"TE DESCONECTASTE FLACO (KERNEL-DISPATCH)");
        return;
    }
    else if(bytes == ERROR)
    {
        log_warning(logger,"ERROR EN EL RECIBIR_OPERACION (KERNEL-DISPATCH)");
        return;
    }

    t_pcb* pcb_recibido = recibir_pcb(cliente_fd_conexion_dispatch);
    
    realizarCicloInstruccion(fd_conexion_memoria,pcb_recibido,cliente_fd_conexion_dispatch);
    
    free(pcb_recibido->estado);
    free(pcb_recibido);
    }
}













/*
antes
log_debug(logger,"PC RECIBIDO: %u",pcb_recibido->pc);
    log_debug(logger,"ESTADO RECIBIDO: %s",pcb_recibido->estado);
    log_debug(logger,"AX: %u",pcb_recibido->registros_CPU.AX);
    log_debug(logger,"BX: %u",pcb_recibido->registros_CPU.BX);
    log_debug(logger,"CX: %u",pcb_recibido->registros_CPU.CX);
despues
    log_debug(logger,"AX DESPUES DEL CICLO: %u",pcb_recibido->registros_CPU.AX);
    log_debug(logger,"BX DC: %u",pcb_recibido->registros_CPU.BX);
    log_debug(logger,"CX DC: %u",pcb_recibido->registros_CPU.CX);
*/