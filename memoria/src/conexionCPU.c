#include "../include/conexionCPU.h"

extern t_log* logger;
extern t_log* logger_obligatorio;
extern t_list* instrucciones_procesos;
extern t_datos_memoria datos_memoria;
extern t_bitarray* bitmap;
extern void* user_space;//se podria sacar

void realizar_handshake_cpu(int fd_cpu)
{
    if(recibir_operacion(fd_cpu)==HANDSHAKE)
    {
        char* moduloConectado = recibir_mensaje(fd_cpu,logger);
		enviar_handshake_ok(logger,fd_cpu, moduloConectado);
        free(moduloConectado);
    }else{
        log_error(logger,"Error al realizar el handshake con cpu");
        abort();
    }
}

void iniciar_hilo_cpu(int fd_cpu)
{
    pthread_t hiloCPU;
    info_fd_cpu* info_fd = malloc(sizeof(info_fd_cpu));
    info_fd->fd = fd_cpu;
    pthread_create(&hiloCPU,NULL,(void*) conexionCPU,(void*) info_fd);
    pthread_detach(hiloCPU);
}

/* Funcion main del hilo CPU-MEMORIA */
void conexionCPU(void* info_fd)
{
    info_fd_cpu* fd_recibido = info_fd;
    int fd_cpu = fd_recibido->fd;
    free(fd_recibido);

    int retardo = datos_memoria.retardo_respuesta;
    //TODO LO QUE SIGUE SE DEBE HACER EN EL KERNEL
    /*char* pathKernel = "archivos_pseudocodigo/codigoPrueba.txt";
    char* pathConfig = config_get_string_value(config,"PATH_INSTRUCCIONES");
    string_append(&pathConfig,pathKernel); //ya esta liberado en conexion_kernel
    agregar_proceso_lista(14,pathConfig);*/
    ////////////////////////

    int codigoOperacion;
    while(1)
    {
        codigoOperacion = recibir_operacion(fd_cpu);

        if(codigoOperacion == DESCONEXION)
        {
            log_error(logger,"TE DESCONECTASTE FLACO (CPU)");
            return;
        }
        else if(codigoOperacion == ERROR)
        {
            log_warning(logger,"ERROR EN EL RECIBIR_OPERACION (CPU)");
            return;
        }

		switch (codigoOperacion) {
        case INSTRUCCION:
            t_list* lista_auxiliar = recibir_paquete(fd_cpu);
            int* pid = list_get(lista_auxiliar,0);
            uint32_t* pc = list_get(lista_auxiliar,1);
            t_proceso* proceso = s_buscar_proceso_pid(*pid);
            usleep(retardo*1000);
            enviar_mensaje((proceso->instrucciones)[*pc],fd_cpu,INSTRUCCION);
            list_destroy_and_destroy_elements(lista_auxiliar,(void*) liberar_elemento);
            break;
        case ACCESO_TABLA_PAGINA:
        {
            t_list* paquete_recibido = recibir_paquete(fd_cpu);
            int* pid = list_get(paquete_recibido,0);
            int* pagina_a_buscar = list_get(paquete_recibido,1);
            t_list* tabla_pag = sbuscar_tabla_pid(*pid);
            int marco_buscado = consultar_marco(*pagina_a_buscar,tabla_pag);
            log_info(logger_obligatorio,"Acceso a Tabla de Páginas: \"PID: %d - Pagina: %d - Marco: %d\"",*pid,*pagina_a_buscar,marco_buscado);
            list_destroy_and_destroy_elements(paquete_recibido,(void*) liberar_elemento);
            usleep(retardo*1000); 
            send(fd_cpu,&marco_buscado,sizeof(int),0);

            break;
        }
        case RESIZE:
        {
            t_list* lista_auxiliar = recibir_paquete(fd_cpu); // RECIBO LOS DATOS QUE ME MANDA CPU
            int* pid = list_get(lista_auxiliar,0);
            int* new_size = list_get(lista_auxiliar,1);
            int resultado;

            resultado = modificar_espacio_proceso(*pid,*new_size,datos_memoria.tam_pagina); // HAGO EL RESIZE
            
            //imprimir_bitmap(); // SOLO PARA PROBAR 
            //fflush(stdout);
            list_destroy_and_destroy_elements(lista_auxiliar,(void*) liberar_elemento);
            usleep(retardo*1000); 
            send(fd_cpu,&resultado,sizeof(int),0); // LE ENVIO A CPU LA RESPUESTA
            
            break;
        }
        case ESCRITURA:
        {
            int resultado = 1;
            int pid,tamanio,base,direccion_fisica;
            void* dato_escribir = recibir_peticion_escritura(&pid,&base,&tamanio,&direccion_fisica,fd_cpu);
            
            memcpy_pagina_frame(direccion_fisica,base,dato_escribir,tamanio);
            log_info(logger_obligatorio,"Acceso a espacio de usuario: \"PID: %d - Accion: ESCRIBIR - Direccion fisica: %d - Tamaño %d\"",pid,direccion_fisica,tamanio);
        
            free(dato_escribir);
            usleep(retardo*1000);
            send(fd_cpu,&resultado,sizeof(int),0); // ENVIA OK SI ESTA OK
            break;
            
        }
        case LECTURA:
        {
            int pid,tamanio_fragmento,direccion_fisica;
            void* dato_leido = recibir_peticion_lectura(&pid,&tamanio_fragmento,&direccion_fisica,fd_cpu);
        
            memcpy(dato_leido,user_space + direccion_fisica,tamanio_fragmento);
            log_info(logger_obligatorio,"Acceso a espacio de usuario: \"PID: %d - Accion: LEER - Direccion fisica: %d - Tamaño %d\"",pid,direccion_fisica,tamanio_fragmento);

            usleep(retardo*1000); 
            send(fd_cpu,dato_leido,tamanio_fragmento,0);
            free(dato_leido);
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



void imprimir_bitmap()
{
    int tam = bitarray_get_max_bit(bitmap);
    int valor;
    char* ola = string_new();
    for(int i=0;i<tam;i++)
    {
        valor = bitarray_test_bit(bitmap,i);
        char* aux = string_itoa(valor);
        string_append(&ola,aux);
        free(aux);
    }
    log_info(logger,"%s",ola);
    free(ola);
    return;
}