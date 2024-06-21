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

    //IO FS TRUNCATE ARCHIVO2.TXT 4069
    //ARCHIVOS_CREADOS = ARCHIVO1.TXT -  ARCHIVO2.TXT - A3 - A4;
    FILE* arch_bitmap = NULL;
    FILE* arch_bloques = NULL;
    //t_bitarray* bitmap_fs;

    char* ruta_bitmap = string_new();
    char* ruta_bloques = string_new();
    char* ruta_archivo_madre = string_new();//falta liberarlo
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
        //int opcode = recibir_operacion(fd_kernel);
        char* codop = readline("ingresa opcode 19(c) 20(d) 21(t) 22(w) 23(r) >");
        int opcode = atoi(codop);
        //char* nombre = "arch1.txt"; // TXT??
        switch (opcode)
        {
            case IO_FS_CREATE:
            {
                char* nombre = readline("INGRESA NOMBRE AL ARCHIVO :");
                char* ruta_archivo = string_new();
                string_append(&ruta_archivo,path_base);
                string_append(&ruta_archivo,"/");
                string_append(&ruta_archivo,nombre); // SERA TXT??
                

                FILE* archivo_creado = fopen(ruta_archivo,"w+"); //MANEJAR SI SE ABRE UN ARCHVO YA CREADO
                t_config* config_archivo = config_create(ruta_archivo);
                int bloque_inicial = bloque_libre(cant_bloques); //MANEJAR SI NO HAY ESPACIO PARA UN NUEVO ARCHIVO
                char* primer_bloque = string_itoa(bloque_inicial);
                asignar_bloque(bloque_inicial);
                agregar_nombre_metadatas(nombre,ruta_archivo_madre);
                config_set_value(config_archivo,"BLOQUE_INICIAL",primer_bloque);
                config_set_value(config_archivo,"TAMANIO","0");
                config_save_in_file(config_archivo,ruta_archivo);

                config_destroy(config_archivo);
                fclose(archivo_creado);

                free(ruta_archivo);
                free(nombre);
                free(primer_bloque);
                break;
            }
            case IO_FS_DELETE:
            {
                char* nombre = readline("INGRESA NOMBRE DEL ARHCIVO A ELIMINAR :");
                char* ruta_archivo = string_new();
                string_append(&ruta_archivo,path_base);
                string_append(&ruta_archivo,"/");
                string_append(&ruta_archivo,nombre);

                t_config* config_archivo = config_create(ruta_archivo);
                int bloque_inicial = config_get_int_value(config_archivo,"BLOQUE_INICIAL"); 
                int tamanio = config_get_int_value(config_archivo,"TAMANIO"); 

                liberar_bloques(bloque_inicial,tamanio,tam_bloque);
                eliminar_nombre_metadatas(nombre,ruta_archivo_madre,path_base);
                remove(ruta_archivo);
                config_destroy(config_archivo);


                free(ruta_archivo);
                free(nombre);
                break;
            }
            case IO_FS_TRUNCATE:
            {
                //L-1-1-L-L-2-2-L-3
                //TAM_BLOQUE 64 DEL CONFIG
                //TAM ACTUAL DEL ARCHIVO -> ARCHIVO METADATA -> 128
                //TRUNCATE 2 190 -> NECESITAS 2 BLOQUES
                //TOTAL ESPACIO CONTIGUO: 1-> NO ALCANZA PARA EVITAR COMPACTACION
                //TOTAL ESPACIO TOTAL: 4 -> NECESITAS COMPACTAR

                //VOID* DATOS_2 = ACA GUARDARIAS TODOS LOS DATOS ACTUALES DEL ARCHIVO 2
                //MEMCPY DESDE EL INICIO AL FINAL

                //ARCHIVO BLOQUE_INICIAL TAMANIO
                // 1-1-2 3-8-1

                // ACCEDER AL 1
                // CAMBIAR SU METADATA -> BLOQUE INICIAL = 0 TAMANIO = 2
                // MEMCPY PARA MOVER LOS DATOS DEL 1

                //1-1-3-2-2-2-2-L-L
                //4 PODES METER EL ARCHIVO 2

                break;
            }
            case IO_FS_WRITE:
            {
                break;
            }
            case IO_FS_READ:
            {
                break;
            }
            default:
            {

                break;
            }
        }
        free(codop);
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
int espacio_contiguo_bitmap(int bloque_inicial,int tamanio, int tam_bloque) // TAM BLOQUE ES DEL CONFIG
{
    int bloques_libres = 0;
    int valor;
    int inicio = bloque_inicial + tamanio / tam_bloque;
    //pthread_mutex_lock(&mutex_bitmap);
    while(1)
    {
        valor = bitarray_test_bit(bitmap_fs,inicio);
        if(valor==0)
            bloques_libres++;
        else
            break;
        inicio++;
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
    int bloque_libre;
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
    int tam_total = bloque_inicial + tamanio / tam_bloque;
    int tam_bitmap = bitarray_get_max_bit(bitmap_fs);
    for(;bloque_inicial<tam_total;bloque_inicial++)
    {
        bitarray_clean_bit(bitmap_fs,bloque_inicial);
        msync(data_bitmap,tam_bitmap,MS_SYNC);
    }
}

/*

*/