#include "../include/main.h"

t_log* logger;
t_config* config;

int main(int argc, char* argv[]) 
{
    int fd_conexion_memoria = 0;
    int server_fd_escucha_dispatch = 0;
    int server_fd_escucha_interrupt = 0;
    int cliente_fd_conexion_interrupt = 0;
    int cliente_fd_conexion_dispatch = 0;
    logger = iniciar_logger("cpu.log", "MODULO CPU", 1, LOG_LEVEL_DEBUG);
    config = iniciar_config("cpu.config",logger);
    //
    /*t_persona* persona=malloc(sizeof(t_persona)); //hacerle free
    persona->dni=45689548;
    persona->pasaporte=45454654;
    persona->edad=25;
    persona->nombre_length=4;
    persona->nombre="pepe";*/
    //
    iniciar_conexiones(logger,config,&fd_conexion_memoria,&server_fd_escucha_dispatch,&server_fd_escucha_interrupt,&cliente_fd_conexion_dispatch,&cliente_fd_conexion_interrupt);
    mandarHandshake(logger,fd_conexion_memoria,"MODULO MEMORIA");
    recibir_handshake(logger,cliente_fd_conexion_dispatch,"MODULO KERNEL-DISPATCH");
    recibir_handshake(logger,cliente_fd_conexion_interrupt,"MODULO KERNEL-INTERRUPT");
    manejarConexionKernel(logger,&cliente_fd_conexion_dispatch);
    //persona_serializar(persona);
    terminar_programa(logger,config,&fd_conexion_memoria,&cliente_fd_conexion_dispatch,&cliente_fd_conexion_interrupt);
    //close(server_fd_escucha_dispatch);
    //close(server_fd_escucha_interrupt);

    return 0;
}

void manejarConexionKernel(t_log* logger,int* cliente_fd_conexion_dispatch)
{
    int a;
    while(1){
        scanf("%d",&a);
    } // SOLAMENTE PARA QUE SE QUEDE EJECUTANDO Y NO SE CAIGA, REPRESENTARIA EL CICLO DE INSTRUCCION
    // QUE IMPLEMENTAREMOS EN EL SIGUIENTE CHECKPOINT
    /*int cod_op;
    recv(*cliente_fd_conexion_dispatch, &cod_op, sizeof(cod_op), MSG_WAITALL);
    while(1)
    {
		switch (cod_op) {
        case PROCESO:
            log_info(logger,"proceso recibido");
		case HANDSHAKE:
			recibir_handshake(logger,*fd_cpu,"MODULO CPU");
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            break;
			//return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
        break;
    }*/
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
