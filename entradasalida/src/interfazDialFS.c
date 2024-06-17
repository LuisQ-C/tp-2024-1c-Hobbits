#include "../include/interfazDialFS.h"

void dialFS(t_config* config,int fd_kernel,int fd_memoria)
{
    int tam_bloque = config_get_int_value(config,"BLOCK_SIZE");
    int cant_bloques = config_get_int_value(config,"BLOCK_COUNT");
    int tiempo_trabajo = config_get_int_value(config,"TIEMPO_UNIDAD_TRABAJO");
    int retraso_compactacion = config_get_int_value(config,"RETRASO_COMPACTACION");
    char* path_base = config_get_string_value(config,"PATH_BASE_DIALFS");

    FILE* arch_bitmap;
    FILE* arch_bloques;
    t_bitarray* bitmap_fs;

    if(access("bitmap.dat",F_OK) == -1)
    {
        arch_bitmap = fopen("file_system/bitmap.dat","wb+"); //NO EXISTE ENTONCES LO CREO
        fflush(arch_bitmap);
        ftruncate(fileno(arch_bitmap),ceil(cant_bloques/8.00));
        bitmap_fs = cargar_bitmap_nuevo(arch_bitmap,cant_bloques);
       // modificar_bitmap()
    }
    else
    {
        arch_bitmap = fopen("file_system/bitmap.dat","rb+");
        bitmap_fs = cargar_bitmap_existente(arch_bitmap,cant_bloques);
    }

    /*
    if(access("bloques.dat",F_OK) == -1)
    {
        arch_bloques = fopen("file_system/bloques.dat","wb+"); //NO EXISTE ENTONCES LO CREO
    }
    else
    {
        arch_bloques = fopen("file_system/bloques.dat","rb+");
    }*/


}

t_bitarray* cargar_bitmap_nuevo(FILE* arch_bitmap,int cant_bloques)
{
    int tam_bitmap = ceil(cant_bloques/8.00);
    void* data_bitmap = malloc(tam_bitmap);
    data_bitmap = mmap(NULL,tam_bitmap,PROT_READ|PROT_WRITE,MAP_SHARED,fileno(arch_bitmap),0);
   // memset(data_bitmap,0,tam_bitmap);
    t_bitarray* bitmap_fs = bitarray_create_with_mode(data_bitmap,tam_bitmap,LSB_FIRST);
    msync(arch_bitmap,tam_bitmap,MS_SYNC);
    
    fclose(arch_bitmap);
    return bitmap_fs;
}

t_bitarray* cargar_bitmap_existente(FILE* arch_bitmap,int cant_bloques)
{
    int cant_bloques_redondeados = ceil(cant_bloques/8.00);
    void* data_bitmap = mmap(NULL,cant_bloques_redondeados,PROT_READ|PROT_WRITE,MAP_SHARED,fileno(arch_bitmap),0);
    t_bitarray* bitmap_fs = bitarray_create_with_mode(data_bitmap,cant_bloques_redondeados,LSB_FIRST);
    
    fclose(arch_bitmap);
    return bitmap_fs;
}

void modificar_bitmap(int cant_bloques)
{
    FILE* arch_bitmap = fopen("file_system/bitmap.dat","rb+");
    int cant_bloques_redondeados = ceil(cant_bloques/8.00);
    void* data_bitmap = mmap(NULL,cant_bloques_redondeados,PROT_READ|PROT_WRITE,MAP_SHARED,fileno(arch_bitmap),0);
    t_bitarray* bitmap_fs = bitarray_create_with_mode(data_bitmap,cant_bloques_redondeados,LSB_FIRST);
    bitarray_set_bit(bitmap_fs,0);
    bitarray_set_bit(bitmap_fs,1);
    msync(arch_bitmap,cant_bloques_redondeados,MS_SYNC);
    fclose(arch_bitmap);

}