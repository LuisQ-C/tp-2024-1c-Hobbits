#include "../include/mmu.h"

extern t_log* logger;
extern t_log* logger_obligatorio;
extern config_memoria config_mem;
extern t_tlb tlb_scheduler;

/* Traduce una direccion logica y devuelve el numero de pagina, o DIRECCION_ERRONEA si la direccion es mala*/
int traducir_direccion_pagina(int direccion_logica) // LEERLO DEL CONFIG
{
    float aux_tam_pag = config_mem.tam_pagina;
    int numero_pagina = floor(direccion_logica/aux_tam_pag);
    if(numero_pagina>config_mem.cant_marcos || numero_pagina<0) 
    {
        printf("DIRECCION ERRONEA");
        return DIRECCION_ERRONEA; // manejar con enum DIRECCION_ERRONEA
    }
    return numero_pagina;
}

/* Calcula el desplazamiento de una direccion logica y lo retorna*/
int traducir_direccion_desplazamiento(int direccion_logica,int numero_pagina)
{
    int desplazamiento = direccion_logica - numero_pagina * config_mem.tam_pagina;
    return desplazamiento;
}

/*
PROCESO

-DETERMINO SI MI DATO ENTRA EN UNA PAGINA
-SI LO HACE, SOLICITO SU FRAME
-SI NO LO HACE, CUENTO CUANTAS PAGINAS EXTRAS NECESITO Y PIDO EL FRAME DE TODAS
-AL RECIBIR EL FRAME O LOS FRAMES
    >DIVIDO EL DATO
    >SUMO EL DESPLAZAMIENTO A CADA FRAME*tamframe PARA ENVIAR EXACTAMENTE A DONDE DEBE COPIARLO, JUNTO CON EL DATO


*/

//PARA HACER LA TRADUCCION NECESITO LA DIRECCION LOGICA Y EL TAMANIO DE LO QUE QUIERO MANDAR 
//SABER SI ENTRA EN LA PAGINA Y SINO SABER CUANTAS NECEISTO Y PEDIRLE A LA MEMORIA EL TAMANIO DE TODAS ELLAS

//DIRECCION LOGICA: TAMANIO PAGINA || DESPLAZAMIENTO


//primero calculo pagina inicial con funcion, paginas totales con funcion
/* Arma y envia el paquete a memoria solicitando los frames de las paginas correspondientes */
void armar_solicitud_frames(int pagina_inicial, int paginas_totales, int fd_memoria, int pid)
{
    t_paquete* paquete = crear_paquete(ACCESO_TABLA_PAGINA);
    agregar_a_paquete(paquete,&pid,sizeof(int));
    int pagina_agregar = pagina_inicial;
    for(int i = 0;i<paginas_totales;i++) 
    {
        agregar_a_paquete(paquete,&pagina_agregar,sizeof(int));
        pagina_agregar++;
    }
    enviar_paquete(paquete,fd_memoria);//ver de donde sacar el fd de memoria
    eliminar_paquete(paquete);
    
    //PARA PRUEBAS
    /*
    t_list* lista_pruebas = list_create();
    int pagina_agregar = pagina_inicial;
    int* nueva_pagina;
    for(int i = 0;i<paginas_totales;i++) 
    {
        nueva_pagina = malloc(sizeof(int));
        *nueva_pagina = pagina_agregar;
        list_add(lista_pruebas,nueva_pagina);
        pagina_agregar++;
    }
    return lista_pruebas;*/
}








//PRIMERO EN LA LISTA ESTA EL DATO
//Y DESPUES LE SIGUEN TODOS LOS T_PORCION con dir fisica, base y tamanio
t_list* crear_paquete_escritura(int direccion_logica, int tam_dato, t_list* lista_frames, int pag_necesarias)
{
    int base = 0;
    int restante = tam_dato;
    int direccion_fisica = -1;
    int* nro_frame;

    int nro_pagina = traducir_direccion_pagina(direccion_logica); //manejo error direccion erronea
    int desplazamiento = traducir_direccion_desplazamiento(direccion_logica,nro_pagina);
    int espacio_disponible = config_mem.tam_pagina - desplazamiento;

    t_list_iterator* iterar_frames = list_iterator_create(lista_frames);
    //int pag_necesarias = paginas_necesarias(desplazamiento,tam_dato);
    //
    printf("\nPAGINAS NECESARIAS: %d\n",pag_necesarias);

    t_list* lista_pruebas = list_create();
    //int espacio_restante = tam_pagina - (direccion_logica%tam_pagina);
    t_porcion_dato* nueva_porcion;
    nueva_porcion = malloc(sizeof(t_porcion_dato));
    
    nro_frame = list_iterator_next(iterar_frames);
    printf("\nNUMERO FRAME: %d\n",*nro_frame);

    direccion_fisica = calcular_direccion_fisica(*nro_frame,desplazamiento);

    asignar(nueva_porcion,base,espacio_disponible,direccion_fisica);

    list_add(lista_pruebas,nueva_porcion);

    avanzar_base_restante(&base,&restante,espacio_disponible);

    for(int i=1;i<pag_necesarias;i++)
    {
        nro_frame = list_iterator_next(iterar_frames);
        printf("\nNUMERO FRAME: %d\n",*nro_frame);
        direccion_fisica = calcular_direccion_fisica(*nro_frame,0); //DESPLAZAMIENTO 0 PORQUE CONTINUA CON LA PAGINA CONTIGUA
        nueva_porcion = malloc(sizeof(t_porcion_dato));
        if(restante<config_mem.tam_pagina)
        {
            asignar(nueva_porcion,base,restante,direccion_fisica);
        }else{
            asignar(nueva_porcion,base,config_mem.tam_pagina,direccion_fisica);
        }
        list_add(lista_pruebas,nueva_porcion);
        avanzar_base_restante(&base,&restante,config_mem.tam_pagina);
    }
    list_iterator_destroy(iterar_frames);
    

    return lista_pruebas;
}

void asignar(t_porcion_dato* porcion_asignar,int base,int tamanio,int direccion_fisica)
{
    porcion_asignar->base = base;
    porcion_asignar->tamanio = tamanio;
    porcion_asignar->direccion_fisica = direccion_fisica;
}

void avanzar_base_restante(int* base, int* restante, int tamanio)
{
    *base += tamanio;
    *restante -= tamanio;
}

int calcular_direccion_fisica(int nro_frame, int desplazamiento)
{
    int direccion_fisica = (nro_frame*config_mem.tam_pagina)+desplazamiento;
    return direccion_fisica;
}


/*
t_list* bases_arranque(int num_pagina,int desplazamiento, int tam_dato)
{
    int pag_necesarias = paginas_necesarias(num_pagina,desplazamiento,tam_dato);
    int tamanio_pagina = 4; //viene del config
    int base = 0;
    int restante = tam_dato;
    //11 bytes
    int espacio_primer_pagina = tamanio_pagina - desplazamiento; //ESTA MAL
    list_add(lista,primeraBase);
    base += espacio_primer_pagina;
    restante -= espacio_primer_pagina; //9
    //11 bytes y arranco en 2 y las paginas son de 4, 2+4+4+4
    for(int i=1;i<pag_necesarias,i++) //1 aniade el 2->2 aniade el 3->3 aniade el 4->4
    {
        list_add(lista,xBase);
        if(restante<tamanio_pagina)
        {
            base += restante;
        }else
        {
            base += tamanio_pagina;
            restante -= tamanio_pagina;

        }
    }
    
    
    

    

    t_list* bases = list_create();


}*/




//DEBO DIVIDIR EL DATO
int paginas_necesarias(int desplazamiento, int tamanio_dato)
{
    int paginas_necesarias = 0;
    int tamanio_pagina = 4; //HARCODEADO, VIENE DEL CONFIG
    int espacio_disponible_pagina = tamanio_pagina - desplazamiento;
    //TAMANIO PAGINA - DESPLAZAMIENTO, LO SOBRANTE ME CUBRE EL TAMANIO DE MI DATO? 0 o positivo
    int alcanza_pagina = alcanza_una_pagina(espacio_disponible_pagina,tamanio_dato);
    //SI, PERFECTO PIDO EL FRAME DE UNA UNICA PAGINA, INFORMO QUE SOLO NECESITO UNA PAGINA, LA DE LA DIR LOGICA
    if(alcanza_pagina == ALCANZA_UNA_PAGINA)
    {
        paginas_necesarias = 1;
    }
    else
    {
        int espacio_faltante = tamanio_dato - espacio_disponible_pagina; 
        int cant_paginas_adicionales = cant_paginas_necesarias(espacio_faltante);
        paginas_necesarias = cant_paginas_adicionales + 1;
    }
    //NO, DEBO DETERMINAR CUANTAS PAGINAS MAS NECESITO PARA PEDIR EL FRAME DE TODAS ELLAS, SUMO A LA PAGINA DE MI DIR LOGICA
    //PASO LO NEGATIVO PARA VER CUANTAS PAGINAS MAS NECESITO
    return paginas_necesarias;
}

int alcanza_una_pagina(int espacio_disponible, int tamanio_dato)
{
    if(espacio_disponible<tamanio_dato)
    {
        return NO_ALCANZA_UNA_PAGINA;
    }
    else
    {
        return ALCANZA_UNA_PAGINA;
    }
}

int cant_paginas_necesarias(int tamanio)
{

    double tam_pagina = config_mem.tam_pagina;//hardcoadeado viene de config
    int paginas_necesarias = ceil(tamanio/tam_pagina);
    
    return paginas_necesarias;
}

//TAMANIOS REGISTRO

int tam_registro(char* registro_chequear)
{
    int tam_dato;
    if(registro_chequear[0] == 'E')
    {
        tam_dato = 4;
    }
    else
    {
        tam_dato = 1;
    }
    return tam_dato;
}

t_list* solicitar_macros(int pagina_inicial, int paginas_totales, int pid, int fd_destinatario)
{
    int cant_entradas = tlb_scheduler.cant_entradas;

    t_list* marcos = (cant_entradas==0)
        ?   solicitar_marcos_sin_tlb(pagina_inicial,paginas_totales,pid,fd_destinatario)
        :   solicitar_marcos_con_tlb(pagina_inicial,paginas_totales,pid,fd_destinatario);

    return marcos;
}

t_list* solicitar_marcos_sin_tlb(int pagina_inicial, int paginas_totales, int pid, int fd_destinatario)
{
    t_list* lista_marcos = list_create();
    int * marco;

    for(int i=0;i<paginas_totales;i++)
    {
        marco = malloc(sizeof(int));
        enviar_solicitud_macro(pid,pagina_inicial,fd_destinatario);
        pagina_inicial++;
        recv(fd_destinatario,marco,sizeof(int),MSG_WAITALL);
        logear_consulta_marco(pid,pagina_inicial,*marco);
        list_add(lista_marcos,marco);
    }

    return lista_marcos;
}

t_list* solicitar_marcos_con_tlb(int pagina_inicial, int paginas_totales,int pid, int fd_destinatario)
{
    t_list* lista_marcos = list_create();
    int* marco;
    for(int i=0;i<paginas_totales;i++)
    {
        //BUSCAR MARCO EN TLB
        marco = malloc(sizeof(int));
        *marco = buscar_entrada_tlb(pid,pagina_inicial);
        if(*marco == -1)
        {
            //TLB MISS
            log_info(logger_obligatorio,"TLB Miss: \"PID: %d- TLB MISS - Pagina: %d\"",pid,pagina_inicial);
            enviar_solicitud_macro(pid,pagina_inicial,fd_destinatario);
            recv(fd_destinatario,marco,sizeof(int),MSG_WAITALL);
            logear_consulta_marco(pid,pagina_inicial,*marco);
            list_add(lista_marcos,marco);
            agregar_entrada_tlb(pid,pagina_inicial,*marco);
        }
        else
        {
            //TLB HIT
            log_info(logger_obligatorio,"TLB Hit: \"PID: %d- TLB HIT - Pagina: %d\"",pid,pagina_inicial);
            list_add(lista_marcos,marco);
        }

        pagina_inicial++;
        /*
        marco = malloc(sizeof(int));
        enviar_solicitud_macro(pid,pagina_inicial,fd_destinatario);
        pagina_inicial++;
        recv(fd_destinatario,marco,sizeof(int),MSG_WAITALL);
        list_add(lista_marcos,marco);
        */
    }
    return lista_marcos;
}

void logear_consulta_marco(int pid, int pagina, int marco)
{
    //falta log obligatorio
    log_info(logger_obligatorio,"Obtener Marco: \"PID: %d - OBTENER MARCO - Página: %d - Marco: %d\".", pid, pagina, marco);
}

void logear_lectura_string(int pid, int dir_fis,char* valor_leido)
{
    log_info(logger_obligatorio,"Lectura Memoria: \"PID: %d - Acción: LEER - Dirección Física: %d - Valor: %s\".",pid,dir_fis,valor_leido);
}

void logear_lectura_int(int pid, int dir_fis,int valor_leido)
{
    log_info(logger_obligatorio,"Lectura Memoria: \"PID: %d - Acción: LEER - Dirección Física: %d - Valor: %d\".",pid,dir_fis,valor_leido);
}

void logear_escritura_string(int pid, int dir_fis,char* valor_escrito)
{
    log_info(logger_obligatorio,"Escritura Memoria: \"PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %s\".",pid,dir_fis,valor_escrito);
}

void logear_escritura_int(int pid, int dir_fis,int valor_escrito)
{
    log_info(logger_obligatorio,"Escritura Memoria: \"PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %d\".",pid,dir_fis,valor_escrito);
}