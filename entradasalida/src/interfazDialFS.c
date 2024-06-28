#include "../include/interfazDialFS.h"

extern t_log* logger;
t_bitarray* bitmap_fs;
void* block_fs;
void* data_bitmap;

void dialFS(t_config* config,int fd_kernel,int fd_memoria)
{
    int tam_bloque = config_get_int_value(config,"BLOCK_SIZE");
    int cant_bloques = config_get_int_value(config,"BLOCK_COUNT");
    int tiempo_trabajo = config_get_int_value(config,"TIEMPO_UNIDAD_TRABAJO");
    int retraso_compactacion = config_get_int_value(config,"RETRASO_COMPACTACION");
    char* path_base = config_get_string_value(config,"PATH_BASE_DIALFS");
    int tam_block_fs = cant_bloques * tam_bloque;
    
    FILE* arch_bitmap = NULL;
    FILE* arch_bloques = NULL;

    char* ruta_bitmap = string_new();
    char* ruta_bloques = string_new();
    char* ruta_archivo_madre = string_new();//falta liberarlo, no se donde
    string_append(&ruta_bitmap,path_base);
    string_append(&ruta_bloques,path_base);
    string_append(&ruta_archivo_madre,path_base);
    string_append(&ruta_bitmap,"/bitmap.dat");
    string_append(&ruta_bloques,"/bloques.dat");
    string_append(&ruta_archivo_madre,"/archivo_madr3.txt");

    bitmap_fs = (access(ruta_bitmap,F_OK) == -1)
        ? cargar_bitmap_nuevo(arch_bitmap,cant_bloques)
        : cargar_bitmap_existente(arch_bitmap,cant_bloques);

    //imprimir_bitmap(bitmap_fs);
    
    block_fs = (access(ruta_bloques,F_OK) == -1)
        ? cargar_block_fs_nuevo(arch_bloques,tam_block_fs)
        : cargar_block_fs_existente(arch_bloques,tam_block_fs);

    if(access(ruta_archivo_madre,F_OK) == -1)
        crear_archivo_metadatas(ruta_archivo_madre);


    free(ruta_bitmap);
    free(ruta_bloques);

    while(1)
    {
        int opcode = recibir_operacion(fd_kernel);
        t_list* paquete_recibido;
        /*char* codop = readline("ingresa opcode 19(c) 20(d) 21(t) 22(w) 23(r) >");
        int opcode = atoi(codop);
        int pid = 1; */
        //char* nombre = "arch1.txt"; // TXT??
        switch (opcode)
        {
            case IO_FS_CREATE:
            {
                paquete_recibido = recibir_paquete(fd_kernel);
                int* pid_recibido = list_get(paquete_recibido,0);
                char* nombre_recibido = list_get(paquete_recibido,1);
                //GUARDO LO RECIBIDO EN VARIABLES LOCALES
                int pid = *pid_recibido;
                char* nombre = string_duplicate(nombre_recibido);
                //LOGEO OPERACION Y CREACION ARCHIVO
                logear_operacion_dialFS(logger,pid,IO_FS_CREATE);
                logear_creacion_archivo(logger,pid,nombre);
                //ARMO RUTA ARCHIVO
                char* ruta_archivo = string_new();
                string_append(&ruta_archivo,path_base);
                string_append(&ruta_archivo,"/");
                string_append(&ruta_archivo,nombre);
                
                
                int bloque_inicial = bloque_libre(cant_bloques);

                if(bloque_inicial != -1 && access(ruta_archivo,F_OK) == -1)
                {
                    FILE* archivo_creado = fopen(ruta_archivo,"w+");
                    t_config* config_archivo = config_create(ruta_archivo);
                    char* primer_bloque = string_itoa(bloque_inicial);
                    asignar_bloque(bloque_inicial);
                    agregar_nombre_metadatas(nombre,ruta_archivo_madre);
                    config_set_value(config_archivo,"BLOQUE_INICIAL",primer_bloque);
                    config_set_value(config_archivo,"TAMANIO","0");
                    config_save_in_file(config_archivo,ruta_archivo);

                    config_destroy(config_archivo);
                    fclose(archivo_creado);

                    free(primer_bloque);
                }
                free(ruta_archivo);
                free(nombre);
                list_destroy_and_destroy_elements(paquete_recibido,(void*) liberar_elemento);

                usleep(tiempo_trabajo*1000);
                int respuesta = INTERFAZ_LISTA;
                send(fd_kernel,&respuesta,sizeof(int),0);
                
                break;
            }
            case IO_FS_DELETE:
            {
                paquete_recibido = recibir_paquete(fd_kernel);
                int* pid_recibido = list_get(paquete_recibido,0);
                char* nombre_recibido = list_get(paquete_recibido,1);
                //GUARDO LO RECIBIDO EN VARIABLES LOCALES
                int pid = *pid_recibido;
                char* nombre = string_duplicate(nombre_recibido);
                //LOGEO OPERACION Y ELIMINACION ARCHIVO
                logear_operacion_dialFS(logger,pid,IO_FS_DELETE);
                logear_eliminacion_archivo(logger,pid,nombre);
                //ARMO LA RUTA DEL ARCHIVO
                char* ruta_archivo = string_new();
                string_append(&ruta_archivo,path_base);
                string_append(&ruta_archivo,"/");
                string_append(&ruta_archivo,nombre);

                if(access(ruta_archivo,F_OK) != -1)
                {
                    t_config* config_archivo = config_create(ruta_archivo);
                    int bloque_inicial = config_get_int_value(config_archivo,"BLOQUE_INICIAL"); 
                    int tamanio = config_get_int_value(config_archivo,"TAMANIO"); 
                
                    int puntero_inicial = bloque_inicial*tam_bloque;

                    int tamanio_bloques_asignados = calcular_bloques(tamanio,tam_bloque) * tam_bloque;

                    liberar_bloques(bloque_inicial,tamanio_bloques_asignados,tam_bloque);
                
                    //SE SETEA TODO EN 0 EL ESPACIO LIBERADO DE BLOQUES
                    memset(block_fs+puntero_inicial,0,tamanio_bloques_asignados);
                    msync(block_fs,tam_block_fs,MS_SYNC);

                    eliminar_nombre_metadatas(nombre,ruta_archivo_madre,path_base);
                    remove(ruta_archivo);
                    config_destroy(config_archivo);
                }
                

                free(ruta_archivo);
                free(nombre);
                list_destroy_and_destroy_elements(paquete_recibido,(void*) liberar_elemento);

                usleep(tiempo_trabajo*1000);
                int respuesta = INTERFAZ_LISTA;
                send(fd_kernel,&respuesta,sizeof(int),0);

                break;
            }
            case IO_FS_TRUNCATE:
            {
                //COMPROBAR SI EL ARCHIVO EXISTE, SI NO EXISTE NO HACER NADA - NO ESTA HECHO, NO QUIERO AGREGAR OTRO IF XD
                paquete_recibido = recibir_paquete(fd_kernel);
                int* pid_recibido = list_get(paquete_recibido,0);
                char* nombre_recibido = list_get(paquete_recibido,1);
                int* tamanio_recibido = list_get(paquete_recibido,2);
                //GUARDO LO RECIBIDO EN VARIABLES LOCALES
                int pid = *pid_recibido;
                char* nombre = string_duplicate(nombre_recibido);
                int tamanio_truncate = *tamanio_recibido;
                //LOGEO OPERACION A REALIZAR
                logear_operacion_dialFS(logger,pid,IO_FS_TRUNCATE);
                //ARMO RUTA ARCHIVO
                char* ruta_archivo = string_new();
                string_append(&ruta_archivo,path_base);
                string_append(&ruta_archivo,"/");
                string_append(&ruta_archivo,nombre);

                t_config* config_archivo = config_create(ruta_archivo);
                int bloque_inicial = config_get_int_value(config_archivo,"BLOQUE_INICIAL"); 
                int tamanio = config_get_int_value(config_archivo,"TAMANIO");
                config_destroy(config_archivo);
                //CALCULO EL TAMANIO TOTAL CONTANDO TODOS LOS BLOQUES QUE TENGO ASIGNADO
                int tamanio_total_asignado = calcular_bloques(tamanio,tam_bloque) * tam_bloque;
            
                //SI DESEAN ACHICAR EL TAMANIO ENTRA AL IF, SE ANALIZA SI SE QUITAN BLOQUES O NO PARTIENDO DESDE EL ULTIMO
                if(tamanio_truncate<=tamanio_total_asignado)
                {
                    int bloques_totales = calcular_bloques(tamanio_total_asignado,tam_bloque);
                    int bloques_quitar = bloques_totales - calcular_bloques(tamanio_truncate,tam_bloque);
                    int bloque_inicial_a_quitar = bloques_totales - bloques_quitar + bloque_inicial;
                    int tamanio_quitar = bloques_quitar*tam_bloque;
                    
                    liberar_bloques(bloque_inicial_a_quitar,tamanio_quitar,tam_bloque);
                    //printf("\nALCANZA CON LO QUE YA TENES\n");
                    modificar_metadata(nombre,path_base,tamanio_truncate,bloque_inicial);
                    logear_truncate(logger,pid,nombre,tamanio_truncate); //VA EL TAMANIO QUE ME MANDAN O LA RESTA?

                }
                //SI NECESITO MAS TAMANIO Y NO ES SUFICIENTE CON LOS BLOQUES QUE CUENTO ENTRO AL ELSE, SE ANALIZAN 2 CASOS
                //1- SI HAY ESPACIO CONTIGUO SOLO MODIFICO EL BITMAP (IF)
                //2- SI NO HAY ESPACIO CONTIGUO, HAGO LA COMPACTACION Y COLOCO EL ARCHIVO AL FINAL CON SUS NUEVOS BLOQUES (ELSE)
                else
                {
                    int tamanio_necesario = tamanio_truncate - tamanio_total_asignado;
                    int tam_contiguo_en_bytes = espacio_contiguo_bitmap(bloque_inicial,tamanio_total_asignado,tam_bloque,cant_bloques) * tam_bloque;
                    //printf("\nTAMANIO NECESARIO: %d\n",tamanio_necesario);
                    //printf("\nTAMANIO CONTIGUO: %d\n",tam_contiguo_en_bytes);
                    if(tam_contiguo_en_bytes>=tamanio_necesario)
                    {
                        //printf("\nHAY ESPACIO CONTIGUO\n");
                        int primer_bloque_libre_asignar = bloque_inicial + calcular_bloques(tamanio,tam_bloque);
                        asignar_bloques_adicionales(primer_bloque_libre_asignar,calcular_bloques(tamanio_necesario,tam_bloque)); //FALTA HACER FUNCION
                        modificar_metadata(nombre,path_base,tamanio_truncate,bloque_inicial);
                        logear_truncate(logger,pid,nombre,tamanio_truncate);
                    }
                    else
                    {
                        //printf("\nSE NECESITA COMPACTAR\n");
                        //GUARDO EN UN AUXILIAR LOS DATOS DEL ARCHIVO A TRUNCAR
                        int puntero_inicial = calcular_puntero_inicial(bloque_inicial,tam_bloque);
                        void* copia_datos_archivo = malloc(tamanio);
                        memcpy(copia_datos_archivo,block_fs+puntero_inicial,tamanio);
                        //INICIO COMPACTACION
                        logear_inicio_compactacion(logger,pid);
                        iniciar_compactacion(ruta_archivo_madre,path_base,tam_bloque,tam_block_fs,nombre);
                        //FINALIZADA LA COMPACTACION, BUSCO EL PRIMERO BLOQUE LIBRE, LO QUE ME HABIA GUARDADO COMO
                        //AUXILIAR LO COPIO EN LA NUEVA UBICACION Y LE ASIGNO NUEVOS BLOQUES AL PROCESO EN EL BITMAP
                        int nuevo_bloque_inicial = bloque_libre(cant_bloques);
                        int nuevo_puntero_inicial = calcular_puntero_inicial(nuevo_bloque_inicial,tam_bloque);

                        memcpy(block_fs+nuevo_puntero_inicial,copia_datos_archivo,tamanio);
                        free(copia_datos_archivo);
                        msync(block_fs,tam_block_fs,MS_SYNC);
                        modificar_metadata(nombre,path_base,tamanio_truncate,nuevo_bloque_inicial);
                        asignar_bloques_adicionales(nuevo_bloque_inicial,calcular_bloques(tamanio_truncate,tam_bloque));
                        //ESPERO DELAY
                        usleep(retraso_compactacion*1000);
                        //LOGEO LA FINALIZACION DE COMPACTACION
                        logear_fin_compactacion(logger,pid);
                        //LOGEO EL TRUNCATE
                        logear_truncate(logger,pid,nombre,tamanio_truncate);
                    }
                }

                free(nombre);
                free(ruta_archivo);
                list_destroy_and_destroy_elements(paquete_recibido,(void*) liberar_elemento);

                usleep(tiempo_trabajo*1000);
                int respuesta = INTERFAZ_LISTA;
                send(fd_kernel,&respuesta,sizeof(int),0);

                break;
            }
            case IO_FS_WRITE:
            {
                paquete_recibido = recibir_paquete(fd_kernel);
                int* pid_recibido = list_get(paquete_recibido,0);
                char* nombre_recibido = list_get(paquete_recibido,1);
                int* tamanio_dato_recibido = list_get(paquete_recibido,2);
                int* puntero_recibido = list_get(paquete_recibido,3);
                //GUARDO LO RECIBIDO EN VARIABLES LOCALES MENOS LAS DIRECCIONES FISICAS
                int pid = *pid_recibido;
                char* nombre = string_duplicate(nombre_recibido);
                int tamanio_dato = *tamanio_dato_recibido;
                int puntero = *puntero_recibido;
                //LOGEAR OPERACION
                logear_operacion_dialFS(logger,pid,IO_FS_WRITE);
                //ARMO RUTA ARCHIVO
                char* ruta_archivo = string_new();
                string_append(&ruta_archivo,path_base);
                string_append(&ruta_archivo,"/");
                string_append(&ruta_archivo,nombre);
                //OBTENGO DE MEMORIA LOS FRAGMENTOS Y REARMO EL DATO
                void* dato_entero = malloc(tamanio_dato);
                int base, direccionFisica, tamanio;

                for(int i=4; i<list_size(paquete_recibido); i++)
                {
                    t_porcion_dato* datoAenviar = list_get(paquete_recibido,i);
                    t_paquete* infoAenviar = crear_paquete(LECTURA);
                    base= datoAenviar->base;
                    direccionFisica= datoAenviar->direccion_fisica;
                    tamanio= datoAenviar-> tamanio;
                
                    agregar_a_paquete(infoAenviar,&pid,sizeof(int));
                    agregar_a_paquete(infoAenviar,&tamanio,sizeof(int));
                    agregar_a_paquete(infoAenviar,&direccionFisica,sizeof(int));
                    enviar_paquete(infoAenviar,fd_memoria);
                    eliminar_paquete(infoAenviar);

                    void* fragmento_recibido = malloc(tamanio);
                
                    recv(fd_memoria, fragmento_recibido, tamanio,MSG_WAITALL);

                    memcpy(dato_entero+base,fragmento_recibido,tamanio);

                    free(fragmento_recibido);
                }
                logear_escritura_archivo(logger,pid,nombre,tamanio_dato,puntero);
                //OBTENGO LOS DATOS DEL CONFIG DEL ARCHIVO
                t_config* config_archivo = config_create(ruta_archivo);
                int bloque_inicial = config_get_int_value(config_archivo,"BLOQUE_INICIAL");
                //int tamanio = config_get_int_value(config_archivo,"TAMANIO"); NO HACE FALTA LEERLO
                config_destroy(config_archivo);
                //CALCULO EL PUNTERO FISICO CON EL BLOQUES INICIAL
                int puntero_fisico = bloque_inicial*tam_bloque+puntero;
                //COPIO EL DATO AL ARCHIVO
                memcpy(block_fs+puntero_fisico,dato_entero,tamanio_dato);
                msync(block_fs,tam_block_fs,MS_SYNC);

                free(nombre);
                free(ruta_archivo);
                free(dato_entero);
                list_destroy_and_destroy_elements(paquete_recibido,(void*) liberar_elemento);

                usleep(tiempo_trabajo*1000);
                int respuesta = INTERFAZ_LISTA;
                send(fd_kernel,&respuesta,sizeof(int),0);

                break;
            }
            case IO_FS_READ:
            {
                paquete_recibido = recibir_paquete(fd_kernel);
                int* pid_recibido = list_get(paquete_recibido,0);
                char* nombre_recibido = list_get(paquete_recibido,1);
                int* tamanio_dato_recibido = list_get(paquete_recibido,2);
                int* puntero_recibido = list_get(paquete_recibido,3);
                //GUARDO LO RECIBIDO EN VARIABLES LOCALES MENOS LAS DIRECCIONES FISICAS
                int pid = *pid_recibido;
                char* nombre = string_duplicate(nombre_recibido);
                int tamanio_dato = *tamanio_dato_recibido;
                int puntero = *puntero_recibido;
                //LOGEAR OPERACION
                logear_operacion_dialFS(logger,pid,IO_FS_READ);
                //ARMO RUTA ARCHIVO
                char* ruta_archivo = string_new();
                string_append(&ruta_archivo,path_base);
                string_append(&ruta_archivo,"/");
                string_append(&ruta_archivo,nombre);
                //BUSCO EL DATO CALCULANDO EL PUNTERO FISICO Y LO GUARDO EN UN VOID*
                void* dato_escribir_mem = malloc(tamanio_dato);
                t_config* config_archivo = config_create(ruta_archivo);
                int bloque_inicial = config_get_int_value(config_archivo,"BLOQUE_INICIAL");
                //int tamanio = config_get_int_value(config_archivo,"TAMANIO"); NO HACE FALTA LEERLO
                config_destroy(config_archivo);

                int puntero_fisico = bloque_inicial*tam_bloque+puntero;
                int resultado, base, direccionFisica, tamanio;
               
                memcpy(dato_escribir_mem,block_fs+puntero_fisico,tamanio_dato);

                for(int i=4; i<list_size(paquete_recibido); i++){
                    t_porcion_dato* datoAenviar = list_get(paquete_recibido,i);
                    //cadenaDeCaracteres+base+tamanio+direccionfisica
                    t_paquete* infoAenviar = crear_paquete(ESCRITURA);
                    base= datoAenviar->base;
                    direccionFisica= datoAenviar->direccion_fisica;
                    tamanio= datoAenviar-> tamanio;
                    agregar_a_paquete(infoAenviar,&pid,sizeof(int));
                    agregar_a_paquete(infoAenviar,dato_escribir_mem,tamanio_dato);
                    agregar_a_paquete(infoAenviar,&base,sizeof(int));
                    agregar_a_paquete(infoAenviar,&tamanio,sizeof(int));
                    agregar_a_paquete(infoAenviar,&direccionFisica,sizeof(int));
                    enviar_paquete(infoAenviar, fd_memoria);
                    eliminar_paquete(infoAenviar);
                    recv(fd_memoria,&resultado,sizeof(int),MSG_WAITALL);
                }
                
                logear_lectura_archivo(logger,pid,nombre,tamanio_dato,puntero);

                free(nombre);
                free(ruta_archivo);
                free(dato_escribir_mem);
                list_destroy_and_destroy_elements(paquete_recibido,(void*) liberar_elemento);

                usleep(tiempo_trabajo*1000);
                int respuesta = INTERFAZ_LISTA;
                send(fd_kernel,&respuesta,sizeof(int),0);

                break;
            }
            default:
            {
                break;
            }
        }
    }
    
}

t_bitarray* cargar_bitmap_nuevo(FILE* arch_bitmap,int cant_bloques)
{
    log_info(logger,"ENTRE AL IF");
    int tam_bitmap = ceil(cant_bloques/8.00);
    arch_bitmap = fopen("file_system/bitmap.dat","wb+"); //NO EXISTE ENTONCES LO CREO
    fflush(arch_bitmap);
    ftruncate(fileno(arch_bitmap),tam_bitmap);

    data_bitmap = mmap(NULL,tam_bitmap,PROT_READ|PROT_WRITE,MAP_SHARED,fileno(arch_bitmap),0);
    memset(data_bitmap,0,tam_bitmap);
    t_bitarray* bitmap_fs_aux = bitarray_create_with_mode(data_bitmap,tam_bitmap,LSB_FIRST);
    msync(data_bitmap,tam_bitmap,MS_SYNC);
    
    //munmap(data_bitmap,tam_bitmap);

    fclose(arch_bitmap);
    return bitmap_fs_aux;
}

t_bitarray* cargar_bitmap_existente(FILE* arch_bitmap,int cant_bloques)
{
    log_info(logger,"ENTRE AL ELSE");
    arch_bitmap = fopen("file_system/bitmap.dat","rb+"); 
    int tam_bitmap = ceil(cant_bloques/8.00);

    data_bitmap = mmap(NULL,tam_bitmap,PROT_READ|PROT_WRITE,MAP_SHARED,fileno(arch_bitmap),0);
    t_bitarray* bitmap_fs_aux = bitarray_create_with_mode(data_bitmap,tam_bitmap,LSB_FIRST);
    
    //munmap(data_bitmap,tam_bitmap);

    fclose(arch_bitmap);
    return bitmap_fs_aux;
}

void imprimir_bitmap()
{
    int tam = bitarray_get_max_bit(bitmap_fs);
    int valor;
    char* ola = string_new();
    for(int i=0;i<tam;i++)
    {
        valor = bitarray_test_bit(bitmap_fs,i);
        char* aux = string_itoa(valor);
        string_append(&ola,aux);
        free(aux);
    }
    log_info(logger,"%s",ola);
    free(ola);
    return;
}

void* cargar_block_fs_nuevo(FILE* arch_bloques,int tam_block_fs)
{
    log_info(logger,"ENTRE AL IF");
    arch_bloques = fopen("file_system/bloques.dat","wb+"); //NO EXISTE ENTONCES LO CREO
    fflush(arch_bloques);
    ftruncate(fileno(arch_bloques),tam_block_fs);


    void* data_bloques = mmap(NULL,tam_block_fs,PROT_READ|PROT_WRITE,MAP_SHARED,fileno(arch_bloques),0);
    
    memset(data_bloques,0,tam_block_fs);

    msync(data_bloques,tam_block_fs,MS_SYNC);
    
    //munmap(data_bloques,tam_block_fs);

    fclose(arch_bloques);

    return data_bloques;
}

void* cargar_block_fs_existente(FILE* arch_bloques,int tam_block_fs)
{
    log_info(logger,"ENTRE AL ELSE");
    arch_bloques = fopen("file_system/bloques.dat","rb+"); 

    void* data_bloques = mmap(NULL,tam_block_fs,PROT_READ|PROT_WRITE,MAP_SHARED,fileno(arch_bloques),0);
    
    //munmap(data_bloques,tam_block_fs); //NOSE SI VA

    fclose(arch_bloques);

    return data_bloques;
}

int espacio_libre_bitmap(int cant_bloques) // CANT BLOQUES ES DEL CONFIG
{
    int bloques_libres = 0;
    int valor;
    //pthread_mutex_lock(&mutex_bitmap);
    for(int i=0;i<cant_bloques;i++)
    {
        valor = bitarray_test_bit(bitmap_fs,i);
        if(valor==0)
            bloques_libres++;
    }
    //pthread_mutex_unlock(&mutex_bitmap);
    return bloques_libres;
}
//BLOQUE INICIAL = 2 TAMANIO = 1024 TAM BLOQUE = 512
int espacio_contiguo_bitmap(int bloque_inicial,int tamanio, int tam_bloque, int cant_bloques) // TAM BLOQUE ES DEL CONFIG
{
    int bloques_libres = 0;
    int valor;
    int inicio = bloque_inicial + tamanio / tam_bloque; //2+1=3
    //pthread_mutex_lock(&mutex_bitmap);
    for(;inicio<cant_bloques;inicio++)
    {
        valor = bitarray_test_bit(bitmap_fs,inicio);
        if(valor==0)
            bloques_libres++;
        else
            break;
    }
    //pthread_mutex_unlock(&mutex_bitmap);
    return bloques_libres;
}

/******************
MARCO LIBRE

>DEVUELVE EL PRIMER MARCO LIBRE QUE ENCUENTRA

**********************/
int bloque_libre(int cant_bloques)
{
    int valor;
    int bloque_libre = -1;
    //pthread_mutex_lock(&mutex_bitmap);
    for(int i=0;i<cant_bloques;i++)
    {
        valor = bitarray_test_bit(bitmap_fs,i);
        if(valor==0)
        {
            bloque_libre = i;
            break;
        }
    } 
    //pthread_mutex_unlock(&mutex_bitmap);
    return bloque_libre;
}

void asignar_bloque(int bloque_a_asignar)
{
   // pthread_mutex_lock(&mutex_bitmap);
    int tam_bitmap = bitarray_get_max_bit(bitmap_fs);
    bitarray_set_bit(bitmap_fs,bloque_a_asignar);
    
    msync(data_bitmap,tam_bitmap,MS_SYNC);

    //pthread_mutex_unlock(&mutex_bitmap);
}

//EL TAMANIO DE LOS METADATAS SIEMPRE SERA MULTIPLO DEL TAMANIO DEL BLOQUE?? PREGUNTAR FORO
void liberar_bloques(int bloque_inicial,int tamanio,int tam_bloque)
{
    int bloque_final = bloque_inicial + tamanio / tam_bloque;
    int tam_bitmap = bitarray_get_max_bit(bitmap_fs);
    for(;bloque_inicial<bloque_final;bloque_inicial++)
    {
        bitarray_clean_bit(bitmap_fs,bloque_inicial);
        msync(data_bitmap,tam_bitmap,MS_SYNC);
    }
}

int calcular_bloques (int tamanio_config, int tam_bloque)
{
    double tamanio_bloque = (double) tam_bloque;

    int bloques_asignados = ceil(tamanio_config/tamanio_bloque);

    return (tamanio_config==0) ? 1 : bloques_asignados;
}

int calcular_puntero_inicial(int bloque_inicial, int tam_bloque)
{
    return bloque_inicial * tam_bloque;
}

void iniciar_compactacion(char* ruta_archivo_madre, char* ruta_base, int tam_bloque, int tam_block_fs, char* archivo_agrandar)
{
    t_list* metadatas = obtener_nombres_metadatas_con_atributos(ruta_archivo_madre,ruta_base);
    remover_archivo_truncar(metadatas,archivo_agrandar);
    ordenar_lista_por_bloque_inicial(metadatas);
    //PRIMER ELEMENTO LIBERO SU BITMAP, LO MUEVO AL INICIO, LE ASIGNO EL NUEVO TAMANIO, 
    t_list_iterator* iterar_metadatas = list_iterator_create(metadatas);
    int offset = 0;
    while(list_iterator_has_next(iterar_metadatas))
    {
        t_metadata* archivo_metadata = list_iterator_next(iterar_metadatas);
        int tamanio = archivo_metadata->tamanio;
        int bloque_inicial = archivo_metadata->bloque_inicial;
        if(offset != archivo_metadata->bloque_inicial)
        {
            int puntero_destino = calcular_puntero_inicial(offset,tam_bloque);//128
            int puntero_origen = calcular_puntero_inicial(bloque_inicial,tam_bloque);//160
            memcpy_datos_origen_destino(puntero_destino,puntero_origen,tamanio,tam_block_fs);
            reasignar_bitmap(offset,bloque_inicial,tam_bloque,tamanio);
            modificar_metadata(archivo_metadata->nombre_metadata,ruta_base,tamanio,offset);
        }
        offset += calcular_bloques(tamanio,tam_bloque);
    }
    list_iterator_destroy(iterar_metadatas);
    list_destroy_and_destroy_elements(metadatas,(void*)liberar_t_metadata);
}

void remover_archivo_truncar(t_list* lista, char* nombre_archivo)
{
    bool _es_el_archivo(t_metadata* self)
    {
        return strcmp(self->nombre_metadata,nombre_archivo) == 0;
    }
    list_remove_and_destroy_by_condition(lista,(void*)_es_el_archivo,(void*)liberar_t_metadata);
}

void modificar_metadata(char* nombre_archivo,char* path_base,int nuevo_tamanio, int nuevo_bloque_inicial)
{
    char* ruta_completa = string_new();
    string_append(&ruta_completa,path_base);
    string_append(&ruta_completa,"/");
    string_append(&ruta_completa,nombre_archivo);
    char* tamanio_nuevo = string_itoa(nuevo_tamanio);
    char* bloque_inicial_nuevo = string_itoa(nuevo_bloque_inicial);

    t_config* config_metadata = config_create(ruta_completa);
    config_set_value(config_metadata,"TAMANIO",tamanio_nuevo);
    config_set_value(config_metadata,"BLOQUE_INICIAL",bloque_inicial_nuevo);
    config_save_in_file(config_metadata,ruta_completa);
    config_destroy(config_metadata);
    free(ruta_completa);
    free(tamanio_nuevo);
    free(bloque_inicial_nuevo);
}

void memcpy_datos_origen_destino(int puntero_destino, int puntero_origen, int tamanio, int tamanio_block_fs)
{
    memcpy(block_fs+puntero_destino,block_fs+puntero_origen,tamanio);
    msync(block_fs,tamanio_block_fs,MS_SYNC);
}

void reasignar_bitmap(int bloque_inicial_nuevo, int bloque_inicial_anterior, int tam_bloque, int tamanio_config)
{
    int bloques_totales = calcular_bloques(tamanio_config,tam_bloque)+bloque_inicial_nuevo;
    int tamanio_total_bytes = bloques_totales * tam_bloque;
    liberar_bloques(bloque_inicial_anterior,tamanio_total_bytes,tam_bloque);
    for(;bloque_inicial_nuevo<bloques_totales;bloque_inicial_nuevo++)
    {
        asignar_bloque(bloque_inicial_nuevo);
    }
}

void asignar_bloques_adicionales(int primer_bloque, int cant_bloques_asignar)
{
    cant_bloques_asignar += primer_bloque;
    for(;primer_bloque<cant_bloques_asignar;primer_bloque++)
    {
        asignar_bloque(primer_bloque);
    }
}


/*

*/