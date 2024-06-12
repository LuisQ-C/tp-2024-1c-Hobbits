#include "../include/cicloInstruccion.h"

extern t_registro_cpu registro;
extern t_log* logger;
extern int MOTIVO_INTERRUPCION;
extern int PID_ACTUAL;
extern int MOTIVO_DESALOJO;
extern pthread_mutex_t mutex_interrupcion;
extern pthread_mutex_t mutex_pid;
extern sem_t semaforo_pcb_recibido;
extern config_memoria config_mem;

void realizarCicloInstruccion(int fd_conexion_memoria, t_pcb* pcb_recibido,int cliente_fd_conexion_dispatch)
{
    //resetear_var_globales();
    resetear_interrupcion();
    establecer_contexto(pcb_recibido);                                                //CAMBIA EL VALOR DE TODOS LOS REGISTROS GENERALES
    //sem_post(&semaforo_pcb_recibido);
    int i = 1;
    while(1){
    //imprimir_registros(pcb_recibido); //PARA PRUEBAS

    t_instruccion instruccion = fetch(registro.PC,fd_conexion_memoria,pcb_recibido->pid); //FETCH (SOLICITA Y RECIBE LA INSTRUCCION)
    
    decode_and_execute(instruccion,pcb_recibido,cliente_fd_conexion_dispatch,fd_conexion_memoria);         //DECODIFICA LA INSTRUCCION Y LA EJECUTA 

    actualizar_pcb(pcb_recibido);                                                      //ACTUALIZAR PCB
    
    //RESETEAR LAS VAR GLOBALES EN CADA CASO DONDE SE DEVUELVA EL PCB ANTES DE DEVOLVERLO.
    if(fue_desalojado())                                                               //CHEQUEA SI HUBO DESALOJO POR IO, EXIT, ETC
    {
        log_debug(logger,"fue desalojado");
        //resetear_var_globales();
        break;
    }

    if(check_interrupt(pcb_recibido,cliente_fd_conexion_dispatch))                     //CHEQUEA SI EN EL HILO DE INTERRUPCION LE LLEGO UNA INTERRUPCION
    {
        //PARA DEBUG Y PRUEBAS
        if(i==3)
        {
            log_debug(logger,"TODO OKKK");
        }
        else
        {
            log_error(logger,"TODO MAAAAAAL");
        }

        //
        log_warning(logger,"fue interrumpido");
        //resetear_var_globales();
        break;
    }
    i++;
    }
}

/* Resetea las variable globales a sus valores iniciales */
/*void resetear_var_globales()
{
    MOTIVO_DESALOJO = -1;
    pthread_mutex_lock(&mutex_pid);
    PID_ACTUAL = -1;
    pthread_mutex_unlock(&mutex_pid);
    log_debug(logger,"TODO RESETEADO CAPO");
}*/

/*resetea la variable global PID_ACTUAL*/
void resetear_interrupcion()
{
    pthread_mutex_lock(&mutex_interrupcion);
    PID_ACTUAL = -1;
    MOTIVO_INTERRUPCION = -1;
    pthread_mutex_unlock(&mutex_interrupcion);
}

/*void resetear_motivo_interrupcion()
{
    pthread_mutex_lock(&mutex_interrupcion);
    MOTIVO_INTERRUPCION = -1;
    pthread_mutex_unlock(&mutex_interrupcion);
}*/


/* Actualiza los registros de la CPU segun el PCB recibido (tambien actualiza PID_ACTUAL)*/
void establecer_contexto(t_pcb* pcb_recibido)
{
    registro.PC = pcb_recibido->pc;
    registro.AX = pcb_recibido->registros_CPU.AX;
    registro.BX = pcb_recibido->registros_CPU.BX;
    registro.CX = pcb_recibido->registros_CPU.CX;
    registro.DX = pcb_recibido->registros_CPU.DX;
    registro.EAX = pcb_recibido->registros_CPU.EAX;
    registro.EBX = pcb_recibido->registros_CPU.EBX;
    registro.ECX = pcb_recibido->registros_CPU.ECX;
    registro.EDX = pcb_recibido->registros_CPU.EDX;
    /*pthread_mutex_lock(&mutex_pid);
    PID_ACTUAL = pcb_recibido->pid;
    pthread_mutex_unlock(&mutex_pid);*/
}

/* Actualiza el PCB con los datos de los registros de la CPU*/
void actualizar_pcb(t_pcb* pcb_a_actualizar)
{
    pcb_a_actualizar->pc = registro.PC;
    pcb_a_actualizar->registros_CPU.AX = registro.AX;
    pcb_a_actualizar->registros_CPU.BX = registro.BX;
    pcb_a_actualizar->registros_CPU.CX = registro.CX;
    pcb_a_actualizar->registros_CPU.DX = registro.DX;
    pcb_a_actualizar->registros_CPU.EAX = registro.EAX;
    pcb_a_actualizar->registros_CPU.EBX = registro.EBX;
    pcb_a_actualizar->registros_CPU.ECX = registro.ECX;
    pcb_a_actualizar->registros_CPU.EDX = registro.EDX;
    
}

/*Solicita la instruccion a memoria de acuerdo al pc indicado*/
t_instruccion fetch(uint32_t pc, int fd_conexion_memoria,int pid)
{
    pedir_instruccion(pc,pid,fd_conexion_memoria);
    t_instruccion instruccion = recibirInstruccion(fd_conexion_memoria);
    log_info(logger,"Fetch Instruccion: \"PID: %d - FETCH - Program Counter: %u\".",pid,pc);
    return instruccion;
}

/*Crea un paquete con pc y pid para mandarselo a memoria*/
void pedir_instruccion(uint32_t pc, int pid,int fd_conexion_memoria)
{
    t_paquete* paquete = crear_paquete(INSTRUCCION);
    agregar_a_paquete(paquete,&pid,sizeof(int));
    agregar_a_paquete(paquete,&pc,sizeof(uint32_t));
    enviar_paquete(paquete,fd_conexion_memoria);
    eliminar_paquete(paquete);
}

/*Recibe la instruccion de la memoria, necesita free luego de utilizarse*/
t_instruccion recibirInstruccion(int fd_conexion_memoria)
{
    int cod_op;
    t_instruccion instruccion = string_new();
    cod_op = recibir_operacion(fd_conexion_memoria);
    if(cod_op == -1)
    {
        log_error(logger,"Error al recibir la instruccion");
    }
    //log_info(logger,"Valor de CODOP: %d",cod_op);
    char* instruccionRecibida = recibir_mensaje(fd_conexion_memoria,logger);
    string_append(&instruccion,instruccionRecibida);
    free(instruccionRecibida);
    return instruccion;
}

/* Decodifica y ejecuta la instruccion pasada por parametro*/
void decode_and_execute(t_instruccion instruccion,t_pcb* pcb_a_enviar,int fd_dispatch,int fd_memoria)
{
    char** instruccionDesarmada = string_split(instruccion," ");
    int op_code = string_to_opcode(instruccionDesarmada[0]);

    switch(op_code)
    {
        case SET:
        {
            int numero_setear = atoi(instruccionDesarmada[2]);
            int resultado = set(instruccionDesarmada[1],numero_setear);
            if(resultado == PC_SIN_MODIFICAR)
            {
                registro.PC = registro.PC + 1;
            }
            
            break;
        }
        case SUM:
        {
            
            sum(instruccionDesarmada);
            registro.PC = registro.PC + 1;
            break;
        }
        case SUB:
        {
            sub(instruccionDesarmada);
            registro.PC = registro.PC + 1;
            break;
        }
        case JNZ:
        {
            jnz(instruccionDesarmada);
            break;
        }
        case IO_GEN_SLEEP:
        {
            MOTIVO_DESALOJO = IO_GEN_SLEEP;
            pcb_a_enviar->pc = registro.PC+1;
            io_gen_sleep(pcb_a_enviar,instruccionDesarmada,fd_dispatch);
            break;
        }
        case MOV_OUT:
        {   
            //CALCULO NRO PAGINA Y DESPLAZAMIENTO CON PRIMER PARAMETRO MOV_OUT
            int direccion_logica = obtener_valor_registro(instruccionDesarmada[1]);
            //CALCULO TAMANIO REGISTRO ESCRIBIR Y SU CONTENIDO
            int tam_dato_escribir = tam_registro(instruccionDesarmada[2]);
            //int contenido_escribir = obtener_valor_registro(instruccionDesarmada[2]); //PUEDO MANEJARLO TODO CON INT O HACER DIFERENCIA ENTRE 32 Y 8
            int contenido_escribir = obtener_valor_registro(instruccionDesarmada[2]);
            void* ptro_dato = &contenido_escribir;
            
            //CALCULO NRO PAGINA Y DESPLAZAMIENTO A PARTIR DE DIR. LOGICA
            int pagina = traducir_direccion_pagina(direccion_logica); //MANEJAR ERROR DE DIRECCION ERRONEO
            int offset = traducir_direccion_desplazamiento(direccion_logica,pagina);
            //CALCULO PAGINA NECESARIAS
            int pag_necesarias = paginas_necesarias(offset,tam_dato_escribir);
            //ACA VERIA SI CADA UNA ESTA EN LA TLB, SI ESTA TLB HIT SINO MISS
            t_list* marcos = solicitar_macros(pagina,pag_necesarias,pcb_a_enviar->pid,fd_memoria);
            int* primer_marco = list_get(marcos,0);
            int dir_fisica_principal = calcular_direccion_fisica(*primer_marco,offset);

            solicitar_escritura(pcb_a_enviar->pid,ptro_dato,tam_dato_escribir,fd_memoria,offset,marcos);

            logear_escritura_int(pcb_a_enviar->pid,dir_fisica_principal,contenido_escribir);

            list_destroy_and_destroy_elements(marcos,(void*)liberar_elemento);
           
            registro.PC++;
            
            break;
        }
        case MOV_IN:
        {
            //CALCULO NRO PAGINA Y DESPLAZAMIENTO CON PRIMER PARAMETRO MOV_OUT
            int direccion_logica = obtener_valor_registro(instruccionDesarmada[2]);
            //CALCULO TAMANIO REGISTRO ESCRIBIR Y SU CONTENIDO
            int tam_dato_leer = tam_registro(instruccionDesarmada[1]);
            ///////////
            int contenido_leer = 0; //PUEDO MANEJARLO TODO CON INT O HACER DIFERENCIA ENTRE 32 Y 8?
            void* ptro_dato = &contenido_leer;
            //CALCULO NRO PAGINA Y DESPLAZAMIENTO A PARTIR DE DIR. LOGICA
            int pagina = traducir_direccion_pagina(direccion_logica); //MANEJAR ERROR DE DIRECCION ERRONEO
            int offset = traducir_direccion_desplazamiento(direccion_logica,pagina);
            //CALCULO PAGINA NECESARIAS
            int pag_necesarias = paginas_necesarias(offset,tam_dato_leer);
            //ACA VERIA SI CADA UNA ESTA EN LA TLB, SI ESTA TLB HIT SINO MISS
            t_list* marcos = solicitar_macros(pagina,pag_necesarias,pcb_a_enviar->pid,fd_memoria);
            int* primer_marco = list_get(marcos,0);
            int dir_fisica_principal = calcular_direccion_fisica(*primer_marco,offset);


            solicitar_lectura(pcb_a_enviar->pid,&ptro_dato,tam_dato_leer,fd_memoria,offset,marcos);

            logear_lectura_int(pcb_a_enviar->pid,dir_fisica_principal,contenido_leer);

            set(instruccionDesarmada[1],contenido_leer);

            list_destroy_and_destroy_elements(marcos,(void*)liberar_elemento);

            registro.PC++;
            break;
            
        }
        case RESIZE:
        {
            int respuesta;
            int new_size = atoi(instruccionDesarmada[1]); 
            respuesta = resize(pcb_a_enviar->pid,new_size,fd_memoria);
            
            if(respuesta == OUT_OF_MEMORY)
            {
                desalojar_proceso(pcb_a_enviar,OUT_OF_MEMORY,fd_dispatch);
                break;
            }
            
            registro.PC++;
            break;
        }
        case COPY_STRING:
        {
            //copia lo apuntado por si a lo apuntado por di

            //CALCULO NRO PAGINA Y DESPLAZAMIENTO CON PRIMER PARAMETRO MOV_OUT
            int direccion_logica_si = obtener_valor_registro("SI");
            int direccion_logica_di = obtener_valor_registro("DI");
            //OBTENGO TAMANIO STRING A COPIAR Y RESERVO ESPACIO
            int tamanio = atoi(instruccionDesarmada[1]);
            //char* string_copiar = malloc(tamanio);
            void* string_copiar = malloc(tamanio);
            //CALCULO NRO PAGINA, DESPLAZAMIENTO, PAG.NECESARIAS Y MARCOS PARA SI
            int pagina_si = traducir_direccion_pagina(direccion_logica_si); //MANEJAR ERROR DE DIRECCION ERRONEO
            int offset_si = traducir_direccion_desplazamiento(direccion_logica_si,pagina_si);
            int pag_necesarias_si = paginas_necesarias(offset_si,tamanio);
            t_list* marcos_si = solicitar_macros(pagina_si,pag_necesarias_si,pcb_a_enviar->pid,fd_memoria);
            int* primer_marco_si = list_get(marcos_si,0);
            int dir_fisica_principal_si = calcular_direccion_fisica(*primer_marco_si,offset_si);
            //CALCULO NRO PAGINA, DESPLAZAMIENTO, PAG.NECESARIAS Y MARCOS PARA DI
            int pagina_di = traducir_direccion_pagina(direccion_logica_di); //MANEJAR ERROR DE DIRECCION ERRONEO
            int offset_di = traducir_direccion_desplazamiento(direccion_logica_di,pagina_di);
            int pag_necesarias_di = paginas_necesarias(offset_di,tamanio);
            t_list* marcos_di = solicitar_macros(pagina_di,pag_necesarias_di,pcb_a_enviar->pid,fd_memoria);
            int* primer_marco_di = list_get(marcos_di,0);
            int dir_fisica_principal_di = calcular_direccion_fisica(*primer_marco_di,offset_di);
            //ACA VERIA SI CADA UNA ESTA EN LA TLB, SI ESTA TLB HIT SINO MISS, SUPONGAMOS TODOS MISS
            solicitar_lectura(pcb_a_enviar->pid,&string_copiar,tamanio,fd_memoria,offset_si,marcos_si);
            //AGREGO EL \0 al string leido
            char* copia_string_logear = malloc(tamanio+1); //+1 por el caracter centinela
            memcpy(copia_string_logear,string_copiar,tamanio);
            copia_string_logear[tamanio] = '\0';

            logear_lectura_string(pcb_a_enviar->pid,dir_fisica_principal_si,copia_string_logear);

            solicitar_escritura(pcb_a_enviar->pid,string_copiar,tamanio,fd_memoria,offset_di,marcos_di);

            logear_escritura_string(pcb_a_enviar->pid,dir_fisica_principal_di,copia_string_logear);

            list_destroy_and_destroy_elements(marcos_si,(void*)liberar_elemento);
            list_destroy_and_destroy_elements(marcos_di,(void*)liberar_elemento);
            free(string_copiar);
            
            registro.PC++;
            break;
        }
        case WAIT:
        {
            MOTIVO_DESALOJO = WAIT;
            pcb_a_enviar->pc = registro.PC+1;
            instruccion_wait(pcb_a_enviar,instruccionDesarmada[1],fd_dispatch);
            break;
        }
        case SIGNAL:
        {
            MOTIVO_DESALOJO = SIGNAL;
            pcb_a_enviar->pc = registro.PC+1;
            instruccion_signal(pcb_a_enviar,instruccionDesarmada[1],fd_dispatch);
            break;
        }
        case IO_STDIN_READ:
        {
            MOTIVO_DESALOJO = IO_STDIN_READ;
            //INTERFAZ, REGISTRO DIRECCION, REGISTRO TAMANIO
            char* nombre_interfaz = string_duplicate(instruccionDesarmada[1]);
            int direccion_logica = obtener_valor_registro(instruccionDesarmada[2]);
            int tamanio_dato = obtener_valor_registro(instruccionDesarmada[3]);
            
            pcb_a_enviar->pc = registro.PC+1;
            io_stdin_stdout(pcb_a_enviar,nombre_interfaz,direccion_logica,tamanio_dato,fd_dispatch,fd_memoria,IO_STDIN_READ);
            free(nombre_interfaz);
            break;
        }
        case IO_STDOUT_WRITE:
        {
            MOTIVO_DESALOJO = IO_STDOUT_WRITE;
            //INTERFAZ, REGISTRO DIRECCION, REGISTRO TAMANIO
            char* nombre_interfaz = string_duplicate(instruccionDesarmada[1]);
            int direccion_logica = obtener_valor_registro(instruccionDesarmada[2]);
            int tamanio_dato = obtener_valor_registro(instruccionDesarmada[3]);

            pcb_a_enviar->pc = registro.PC+1;
            io_stdin_stdout(pcb_a_enviar,nombre_interfaz,direccion_logica,tamanio_dato,fd_dispatch,fd_memoria,IO_STDOUT_WRITE);
            free(nombre_interfaz);
            break;
        }
        case IO_FS_CREATE:
        {
            MOTIVO_DESALOJO = IO_FS_CREATE;
            char* nombre_interfaz = string_duplicate(instruccionDesarmada[1]);
            char* nombre_archivo = string_duplicate(instruccionDesarmada[2]);

            pcb_a_enviar->pc = registro.PC+1;

            io_fs_create(pcb_a_enviar,nombre_interfaz,nombre_archivo,fd_dispatch);
            free(nombre_archivo);
            free(nombre_interfaz);
            break;
        }
        case IO_FS_DELETE:
        {
            MOTIVO_DESALOJO = IO_FS_DELETE;
            char* nombre_interfaz = string_duplicate(instruccionDesarmada[1]);
            char* nombre_archivo = string_duplicate(instruccionDesarmada[2]);

            pcb_a_enviar->pc = registro.PC+1;

            io_fs_delete(pcb_a_enviar,nombre_interfaz,nombre_archivo,fd_dispatch);
            free(nombre_archivo);
            free(nombre_interfaz);
            break;
        }
        case IO_FS_TRUNCATE:
        {
            MOTIVO_DESALOJO = IO_FS_TRUNCATE;
            char* nombre_interfaz = string_duplicate(instruccionDesarmada[1]);
            char* nombre_archivo = string_duplicate(instruccionDesarmada[2]);
            int tamanio_archivo = obtener_valor_registro(instruccionDesarmada[3]);

            pcb_a_enviar->pc = registro.PC+1;

            io_fs_truncate(pcb_a_enviar,nombre_interfaz,nombre_archivo,tamanio_archivo,fd_dispatch);
            free(nombre_archivo);
            free(nombre_interfaz);
            break;
        }
        case EXIT:
        {
            desalojar_proceso(pcb_a_enviar,EXIT,fd_dispatch);
            break;
        }
        case -1:
            log_warning(logger,"Instruccion invalida");
        default:
            log_error(logger,"Error al decodificar instruccion");
    }
    
    logear_instruccion_ejecutada(pcb_a_enviar->pid,instruccion);
    string_array_destroy(instruccionDesarmada);
    free(instruccion);
    
}

/* Loggea la instruccion ejecutada */
void logear_instruccion_ejecutada(int pid,char* instruccion)
{
    char** instruccionLogear = string_n_split(instruccion,1," ");
    if(instruccionLogear[1]!=NULL)
    {
        log_info(logger,"Instruccion Ejecutada: \"PID: %d - Ejecutando: %s - %s\".",pid,instruccionLogear[0],instruccionLogear[1]);
    }
    else{
        log_info(logger,"Instruccion Ejecutada: \"PID: %d - Ejecutando: %s\".",pid,instruccionLogear[0]);
    }
    string_array_destroy(instruccionLogear);
}

/* Chequea si el PID_ACTUAL fue desalojado, de ser asi devuelve 1, de lo contrario 0 */
int fue_desalojado()
{
    int desalojado = 0;
    if(MOTIVO_DESALOJO != -1)
    {
        MOTIVO_DESALOJO = -1;
        desalojado = 1;
    }
    return desalojado;
}

void desalojar_proceso(t_pcb* pcb_a_desalojar,int motivo_desalojo,int fd_dispatch)
{
    MOTIVO_DESALOJO = motivo_desalojo;

    t_paquete* paquete = armar_paquete_pcb(pcb_a_desalojar);

    agregar_a_paquete(paquete,&motivo_desalojo,sizeof(int));

    enviar_paquete(paquete,fd_dispatch);
    eliminar_paquete(paquete);
}

/* Chequea si llego una interrupcion al PID_ACTUAL, de ser asi devuelve el PCB a kernel y retorna 1, de lo contrario devuelve 0*/
int check_interrupt(t_pcb* pcb_a_chequear,int fd_dispatch)
{
    int ocurrio_interrupcion = 0;
    int motivo_interrupcion = -1;
    int coincide_pid = 0;
    int pid_interrupcion = -1;
    pthread_mutex_lock(&mutex_interrupcion);
    coincide_pid = pcb_a_chequear->pid ==  PID_ACTUAL;
    pid_interrupcion = PID_ACTUAL;
    motivo_interrupcion = MOTIVO_INTERRUPCION;
    PID_ACTUAL = -1;
    MOTIVO_INTERRUPCION = -1;
    pthread_mutex_unlock(&mutex_interrupcion);
    if(coincide_pid)
    {
        log_debug(logger,"INTERRUPCION ACEPTADA A PID: %d",pcb_a_chequear->pid);
        t_paquete* paquete = armar_paquete_pcb(pcb_a_chequear);
        agregar_a_paquete(paquete, &motivo_interrupcion, sizeof(int));
        enviar_paquete(paquete, fd_dispatch);
        eliminar_paquete(paquete);
        ocurrio_interrupcion = 1;
    }
    else
    {
        log_error(logger,"INTERRUPCION RECHAZADA PID PCB: %d Y PID INTERRUPCION: %d",pcb_a_chequear->pid,pid_interrupcion);
    }



    

    //pthread_mutex_lock(&mutex_interrupcion);

    //if(HAY_INTERRUPCION)
    //{
     //   int motivo_interrupcion = INTERRUPCION;
     //   //resetear_pid_actual();
     //   t_paquete* paquete = armar_paquete_pcb(pcb_a_chequear);
     //   agregar_a_paquete(paquete, &motivo_interrupcion, sizeof(int));
     //   enviar_paquete(paquete, fd_dispatch);
     //   eliminar_paquete(paquete);
      //  //enviar_pcb(pcb_a_chequear,fd_dispatch); //ACA SE DEBERIA DEVOLVER CONTEXTO CON MOTIVO "INTERRUPCION"
     //   HAY_INTERRUPCION = 0;
     //   ocurrio_interrupcion = 1;
    //}

    //pthread_mutex_unlock(&mutex_interrupcion);

    

    return ocurrio_interrupcion;
}

void imprimir_registros(t_pcb* pcb)
{
    printf("\nREGISTRO AX: %d\n",pcb->registros_CPU.AX);
    printf("\nREGISTRO BX: %d\n",pcb->registros_CPU.BX);
    printf("\nREGISTRO CX: %d\n",pcb->registros_CPU.CX);
    printf("\nREGISTRO DX: %d\n",pcb->registros_CPU.DX);
    printf("\nREGISTRO EAX: %d\n",pcb->registros_CPU.EAX);
    printf("\nREGISTRO EBX: %d\n",pcb->registros_CPU.EBX);
    printf("\nREGISTRO ECX: %d\n",pcb->registros_CPU.ECX);
    printf("\nREGISTRO EDX: %d\n",pcb->registros_CPU.EDX);
}

void solicitar_lectura(int pid,void** ptro_dato,int tam_dato_leer, int fd_destino, int offset, t_list* marcos)
{
    //DATOS ADMINISTRATIVOS
    int tam_pagina = config_mem.tam_pagina;
    int base = 0;
    int restante = tam_dato_leer;
    int espacio_restante = tam_pagina - offset;
    int pag_necesarias = paginas_necesarias(offset,tam_dato_leer);
    //RTA MEMORIA
    int rta;
    //CALCULO LA DIR. FISICA EN BASE AL PRIMER FRAME
    int* ptr_nro_frame = list_get(marcos,0);
    int dir_fis = calcular_direccion_fisica(*ptr_nro_frame,offset);

    void* fragmento;

    int tamanio_fragmento = (espacio_restante>tam_dato_leer) ? tam_dato_leer : espacio_restante;

    enviar_paquete_lectura(pid,tamanio_fragmento,dir_fis,fd_destino);
    fragmento = recibir_dato_leido(fd_destino,tamanio_fragmento);
    memcpy_fragmento_void(*ptro_dato,base,fragmento,tamanio_fragmento);
    
    free(fragmento);
    //AVANZO LA BASE Y LO RESTANTE
    avanzar_base_restante(&base,&restante,espacio_restante);
    
    for(int i=1;i<pag_necesarias;i++)
    {   
        ptr_nro_frame = list_get(marcos,i);
        dir_fis = calcular_direccion_fisica(*ptr_nro_frame,0);//EL DESPLAZAMIENTO NO ESTA HARDCODED, A PARTIR DE LAS PAGINAS CONTIGUA EL DESPL ES 
        tamanio_fragmento = (restante<tam_pagina) ? restante : tam_pagina;
        
        enviar_paquete_lectura(pid,tamanio_fragmento,dir_fis,fd_destino);
        fragmento = recibir_dato_leido(fd_destino,tamanio_fragmento);
        memcpy_fragmento_void(*ptro_dato,base,fragmento,tamanio_fragmento);

        free(fragmento);

        avanzar_base_restante(&base,&restante,tam_pagina);
    }
}

void solicitar_escritura(int pid,void* ptro_dato,int tam_dato_escribir, int fd_destino, int offset, t_list* marcos)
{
    //DATOS ADMINISTRATIVOS
    int tam_pagina = config_mem.tam_pagina;
    int base = 0;
    int restante = tam_dato_escribir;
    int espacio_restante = tam_pagina - offset;
    int pag_necesarias = paginas_necesarias(offset,tam_dato_escribir);
    //RTA DE MEMORIA
    int rta;
    //CALCULO EN BASE AL PRIMER MARCO LA DIRECCION FISICA
    int* ptr_nro_frame = list_get(marcos,0);
    int dir_fis = calcular_direccion_fisica(*ptr_nro_frame,offset);
    
    int tamanio_enviar;
    
    tamanio_enviar = (espacio_restante > tam_dato_escribir) ? tam_dato_escribir : espacio_restante;
    
    enviar_paquete_escritura(pid,ptro_dato,tam_dato_escribir,base,tamanio_enviar,dir_fis,fd_destino);
    
    avanzar_base_restante(&base,&restante,espacio_restante);

    recv(fd_destino,&rta,sizeof(int),MSG_WAITALL); //MEMORIA DEVUELVE OK, QUE HACEMOS CON ESO?

    for(int i=1;i<pag_necesarias;i++)
    {   
        ptr_nro_frame = list_get(marcos,i);
        dir_fis = calcular_direccion_fisica(*ptr_nro_frame,0);//EL DESPLAZAMIENTO NO ESTA HARDCODED, A PARTIR DE LAS PAGINAS CONTIGUA EL DESPL ES 0

        tamanio_enviar = (restante<tam_pagina) ? restante : tam_pagina;
        
        enviar_paquete_escritura(pid,ptro_dato,tam_dato_escribir,base,tamanio_enviar,dir_fis,fd_destino);
        
        avanzar_base_restante(&base,&restante,tam_pagina);

        recv(fd_destino,&rta,sizeof(int),MSG_WAITALL);
    }

}

//memcpy(ptro_dato+base,fragmento,tamanio_fragmento);

void memcpy_fragmento_void(void* ptro_dato,int base,void* fragmento,int tamanio_fragmento)
{
    memcpy(ptro_dato+base,fragmento,tamanio_fragmento);
}

/*
if(espacio_restante>tam_dato_leer)
{
    enviar_paquete_lectura(pcb_a_enviar->pid,tam_dato_leer,dir_fis,fd_memoria);
    fragmento = recibir_dato_leido(fd_memoria,tam_dato_leer);
    memcpy(ptro_dato+base,fragmento,tam_dato_leer);
}
else{
    enviar_paquete_lectura(pcb_a_enviar->pid,espacio_restante,dir_fis,fd_memoria);
    fragmento = recibir_dato_leido(fd_memoria,tam_dato_leer);
    memcpy(ptro_dato+base,fragmento,espacio_restante);
}*/

/*if(tam_dato_escribir == 1)
{
    uint8_t contenido_escribir = obtener_valor_registro(instruccionDesarmada[2]);
    ptro_dato = &contenido_escribir;
}
else
{
    uint32_t contenido_escribir = obtener_valor_registro(instruccionDesarmada[2]);
    ptro_dato = &contenido_escribir;
}*/


