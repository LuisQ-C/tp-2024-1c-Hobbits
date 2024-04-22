#include "../include/main.h"

t_log* logger;
t_config* config;
t_registro_cpu registro = {0,0,0,0,0,0,0,0,0};

typedef struct{
    int fd;
    t_log* logger;
}info_fd_conexion;

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
   
    if(!iniciar_conexiones(logger,config,&fd_conexion_memoria,&server_fd_escucha_dispatch,&server_fd_escucha_interrupt,&cliente_fd_conexion_dispatch,&cliente_fd_conexion_interrupt))
    {
        log_error(logger,"Error al crear conexiones iniciales");
        exit(1);
    }
    mandarHandshake(logger,fd_conexion_memoria,"MODULO MEMORIA","CPU");
    //
    //realizarCicloInstruccion(logger,fd_conexion_memoria);
    //log_info(logger,"%u",registro.BX);
    log_info(logger,"%u",registro.AX);
    log_info(logger,"%u",registro.BX);
    //CONEXION DISPATCH - SIN HILO POR SER SECUENCIAL
    manejarConexionDispatch(logger,cliente_fd_conexion_dispatch);
    //CONEXION INTERRUPT CON HILO
    pthread_t hiloInterrupt;
    info_fd_conexion* fd_interrupt = malloc(sizeof(info_fd_conexion));
    fd_interrupt->fd = cliente_fd_conexion_interrupt;
    fd_interrupt->logger = logger;
    pthread_create(&hiloInterrupt,NULL,(void*) manejarConexionInterrupt,(void*) fd_interrupt);
    pthread_detach(hiloInterrupt);
    /////  
    
    //liberar los mensajes
    //manejarConexionKernel(logger,&cliente_fd_conexion_dispatch,&cliente_fd_conexion_interrupt);
    int cod_op;
    recv(cliente_fd_conexion_dispatch, &cod_op, sizeof(cod_op), MSG_WAITALL);
    terminar_programa(logger,config,&fd_conexion_memoria,&cliente_fd_conexion_dispatch,&cliente_fd_conexion_interrupt);
    //hola
    return 0;
}

void manejarConexionInterrupt(void* fd_interrupt)
{
    info_fd_conexion* fd_recibido = fd_interrupt;
    int fd_kernel_interrupt = fd_recibido->fd;
    t_log* logger = fd_recibido->logger;
    free(fd_recibido);

    recibir_operacion(fd_kernel_interrupt);
    char* moduloConectado = recibir_mensaje(fd_kernel_interrupt,logger);
    recibir_handshake(logger,fd_kernel_interrupt,moduloConectado);
    free(moduloConectado);

}

void manejarConexionDispatch(t_log* logger,int cliente_fd_conexion_dispatch)
{
    recibir_operacion(cliente_fd_conexion_dispatch);
    char* moduloConectado = recibir_mensaje(cliente_fd_conexion_dispatch,logger);
    recibir_handshake(logger,cliente_fd_conexion_dispatch,moduloConectado);
    free(moduloConectado);

}














/*
void mandar_persona()
{
    t_paquete* paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = PERSONA; // Podemos usar una constante por operación
    paquete->buffer = buffer; // Nuestro buffer de antes.

    // Armamos el stream a enviar
    void* a_enviar = malloc(buffer->size + sizeof(uint8_t) + sizeof(uint32_t));
    int offset = 0;

    memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));

    offset += sizeof(uint8_t);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

    // Por último enviamos
    send(unSocket, a_enviar, buffer->size + sizeof(uint8_t) + sizeof(uint32_t), 0);

    // No nos olvidamos de liberar la memoria que ya no usaremos
    free(a_enviar);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}

t_buffer *persona_serializar(t_persona *persona) {
    t_buffer *buffer = buffer_create(
      sizeof(uint32_t) * 2 +                    // DNI y Pasaporte
      sizeof(uint8_t) +                         // Edad
      sizeof(uint32_t) + persona->nombre_length // Longitud del nombre, y el propio nombre
    );

    buffer_add_uint32(buffer, persona->dni);
    buffer_add_uint8(buffer, persona->edad);
    buffer_add_uint32(buffer, persona->pasaporte);
    buffer_add_string(buffer, persona->nombre_length, persona->nombre);

    free(persona);

    return buffer;
}*/
