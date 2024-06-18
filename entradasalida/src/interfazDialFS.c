#include "../include/interfazDialFS.h"

extern t_log* logger;
t_bitarray* bitmap_fs;
void* block_fs;

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
    //t_bitarray* bitmap_fs;

    char* ruta_bitmap = string_new();
    char* ruta_bloques = string_new();
    string_append(&ruta_bitmap,path_base);
    string_append(&ruta_bloques,path_base);
    string_append(&ruta_bitmap,"/bitmap.dat");
    string_append(&ruta_bloques,"/bloques.dat");

    bitmap_fs = (access(ruta_bitmap,F_OK) == -1)
        ? cargar_bitmap_nuevo(arch_bitmap,cant_bloques)
        : cargar_bitmap_existente(arch_bitmap,cant_bloques);

    //imprimir_bitmap(bitmap_fs);
    
    block_fs = (access(ruta_bloques,F_OK) == -1)
        ? cargar_block_fs_nuevo(arch_bloques,tam_block_fs)
        : cargar_block_fs_existente(arch_bloques,tam_block_fs);

    free(ruta_bitmap);
    free(ruta_bloques);

    while(1)
    {
        //int opcode = recibir_operacion(fd_kernel);
        char* codop = readline("ingresa opcode 19(c) 20(d) 21(t) 22(w) 23(r) >");
        int opcode = atoi(codop);
        char* nombre = "arch1.txt"; // TXT??
        switch (opcode)
        {
            case IO_FS_CREATE:
            {
                char* ruta_archivo = string_new();
                string_append(&ruta_archivo,path_base);
                string_append(&ruta_archivo,"/");
                string_append(&ruta_archivo,nombre);

                FILE* archivo_creado = fopen(ruta_archivo,"w+");// MANEJAR SI SE ABRE UN ARCHVO YA CREADO
                t_config* config_archivo = config_create(ruta_archivo);
                config_set_value(config_archivo,"BLOQUE_INICIAL","0");
                config_set_value(config_archivo,"TAMANIO","0");
                config_save_in_file(config_archivo,ruta_archivo);
                config_destroy(config_archivo);
                break;
            }
            case IO_FS_DELETE:
            {
                break;
            }
            case IO_FS_TRUNCATE:
            {
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
                break;
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

    void* data_bitmap = mmap(NULL,tam_bitmap,PROT_READ|PROT_WRITE,MAP_SHARED,fileno(arch_bitmap),0);
    memset(data_bitmap,0,tam_bitmap);
    t_bitarray* bitmap_fs_aux = bitarray_create_with_mode(data_bitmap,tam_bitmap,LSB_FIRST);
    msync(data_bitmap,tam_bitmap,MS_SYNC);
    
    munmap(data_bitmap,tam_bitmap);

    fclose(arch_bitmap);
    return bitmap_fs_aux;
}

t_bitarray* cargar_bitmap_existente(FILE* arch_bitmap,int cant_bloques)
{
    log_info(logger,"ENTRE AL ELSE");
    arch_bitmap = fopen("file_system/bitmap.dat","rb+"); 
    int tam_bitmap = ceil(cant_bloques/8.00);

    void* data_bitmap = mmap(NULL,tam_bitmap,PROT_READ|PROT_WRITE,MAP_SHARED,fileno(arch_bitmap),0);
    t_bitarray* bitmap_fs_aux = bitarray_create_with_mode(data_bitmap,tam_bitmap,LSB_FIRST);
    
    munmap(data_bitmap,tam_bitmap);

    fclose(arch_bitmap);
    return bitmap_fs_aux;
}

void modificar_bitmap(int cant_bloques)
{
    FILE* arch_bitmap = fopen("file_system/bitmap.dat","rb+");
    int cant_bloques_redondeados = ceil(cant_bloques/8.00);
    void* data_bitmap = mmap(NULL,cant_bloques_redondeados,PROT_READ|PROT_WRITE,MAP_SHARED,fileno(arch_bitmap),0);
    t_bitarray* bitmap_fs_aux = bitarray_create_with_mode(data_bitmap,cant_bloques_redondeados,LSB_FIRST);
    bitarray_set_bit(bitmap_fs_aux,0);
    bitarray_set_bit(bitmap_fs_aux,1);
    
    msync(arch_bitmap,cant_bloques_redondeados,MS_SYNC);
    fclose(arch_bitmap);

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