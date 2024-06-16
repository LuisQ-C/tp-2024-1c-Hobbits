#include "../include/main.h"

t_log* logger;
t_log* logger_obligatorio;
t_config* config;
t_list* instrucciones_procesos;
pthread_mutex_t mutex_lista_procesos;
void* user_space;
t_bitarray* bitmap;
pthread_mutex_t mutex_bitmap;
pthread_mutex_t mutex_user_space;
t_datos_memoria datos_memoria;



int main(int argc, char* argv[]) {
    
    //ESTO ANDA NO BORRARLO
    int server_fd = 0;
    int fd_cpu = 0;
    int fd_kernel = 0;


    logger = iniciar_logger("memoria.log", "MODULO MEMORIA", 1, LOG_LEVEL_DEBUG);
    logger_obligatorio = iniciar_logger("memoria_oblig.log","MODULO MEMORIA-OBL",1,LOG_LEVEL_DEBUG);
    config = iniciar_config("memoria.config",logger);
    tomar_datos_config_y_destruir();
    ////////////////////////////////////////////////////////////////
    user_space = inicializar_user_space();
    
    bitmap = crear_bitmap();


    /////////////////////////////////////////////////////////////////
    instrucciones_procesos = list_create();
    pthread_mutex_init(&mutex_lista_procesos,NULL);
    pthread_mutex_init(&mutex_bitmap,NULL);
    pthread_mutex_init(&mutex_user_space,NULL);
    //
    
    //
    if(!iniciar_conexiones(&server_fd,&fd_cpu,&fd_kernel))
    {
        log_error(logger,"Error al crear conexiones iniciales");
        exit(1);
    }

    realizar_handshakes_memoria(fd_cpu,fd_kernel);
    enviar_datos_config_memoria_a_cpu(datos_memoria.tam_memoria,datos_memoria.tam_pagina,datos_memoria.retardo_respuesta,fd_cpu);

    //PAQUETE PRUEBA
    /*
    t_list* nueva_lista = list_create();

    t_prueba new;
    
    new.a = 2;
    new.b = 4;
    new.c = 6;
    

    t_paquete* paquete = crear_paquete(25); //codigo op random
    agregar_a_paquete(paquete,&new,sizeof(t_prueba));
    new.a = 8;
    new.b =10;
    new.c = 12;
    agregar_a_paquete(paquete,&new,sizeof(t_prueba));
    enviar_paquete(paquete,fd_cpu);
    eliminar_paquete(paquete);*/

    inicializar_hilos(fd_cpu,fd_kernel);

    //////
    //
    while(escucharConexionesIO(server_fd));
    terminar_programa(&fd_cpu,&fd_kernel);
    return 0; //puede ir exit_sucess
}

/*
int main(int argc, char* argv[]) {
    
    //ESTO ANDA NO BORRARLO
    //ESTO ANDA NO BORRARLO
    int server_fd = 0;
    int fd_cpu = 0;
    int fd_kernel = 0;


    logger = iniciar_logger("memoria.log", "MODULO MEMORIA", 1, LOG_LEVEL_DEBUG);
    logger_obligatorio = iniciar_logger("memoria_oblig.log","MODULO MEMORIA-OBL",1,LOG_LEVEL_DEBUG);
    config = iniciar_config("memoria.config",logger);
    tomar_datos_config_y_destruir();
    ////////////////////////////////////////////////////////////////
    user_space = inicializar_user_space();
    bitmap = crear_bitmap();


    /////////////////////////////////////////////////////////////////
    instrucciones_procesos = list_create();
    pthread_mutex_init(&mutex_lista_procesos,NULL);
    
    FILE* archivo_1 = fopen("./pseudos/c2.txt","r");
    agregar_proceso_lista(0,archivo_1);
    agregar_proceso_lista(1,archivo_1);
    agregar_proceso_lista(2,archivo_1);

    modificar_espacio_proceso(0,10,4);
    //modificar_espacio_proceso(1,8,4);
    //modificar_espacio_proceso(2,3,4);

    t_proceso* num1 = list_get(instrucciones_procesos,0);
    t_proceso* num2 = list_get(instrucciones_procesos,1);
    t_proceso* num3 = list_get(instrucciones_procesos,2);

    int tam1 = list_size(num1->tabla_paginas);
    int tam2 = list_size(num2->tabla_paginas);
    int tam3 = list_size(num3->tabla_paginas);

    int espaciolibre = espacio_libre_bitmap();

    for(int i=0;i<128;i++)
    {
        int bit = bitarray_test_bit(bitmap,i);
        printf("%d",bit);
    }

    printf("\nESPACIO LIBRE BITMAP: %d\n",espaciolibre);
    printf("\ntamanio lista: %d\n",tam1);
    //printf("\ntamanio lista: %d\n",tam2);
    //printf("\ntamanio lista: %d\n",tam3);

    modificar_espacio_proceso(0,11,4);
    //modificar_espacio_proceso(1,9,4);
    //modificar_espacio_proceso(1,12,4);
    //modificar_espacio_proceso(2,9,4);
    //modificar_espacio_proceso(0,4,4);
    tam1 = list_size(num1->tabla_paginas);

    printf("\ntamanio lista: %d\n",tam1);
    for(int i=0;i<128;i++)
    {
        int bit = bitarray_test_bit(bitmap,i);
        printf("%d",bit);
    }

    
    void _list_elements(t_entrada_pagina *p) {
                    int marco = p->marco;
                    printf("\nMARCO: %d\n",marco);
                }

    list_iterate(num2->tabla_paginas, (void*) _list_elements);
    

   // int paginasNecesarias = calcular_pag_necesarias(-11,4);
    
    int pag = calcular_pag_necesarias(-3,4);
    printf("\npaginas: %d\n", pag);

    int resultado = ceil(1.15);
    int resultado2 = ceil(1.8);
    int resultado3 = ceil(1.0);
    int resultado4 = ceil(0.2);
    int resultado5 = ceil(0.8);
    
    printf("\nNumero: %d\n", resultado);
    printf("\nNumero: %d\n", resultado2);
    printf("\nNumero: %d\n", resultado3);
    printf("\nNumero: %d\n", resultado4);
    printf("\nNumero: %d\n", resultado5);
    
    

   //PRUEBA USER SPACE ESCRITURA

    uint32_t num_eax = 45487965;
    void* puntero_eax = &num_eax;

    char* prueba = "hola";
    

    t_list* lista_porciones = list_create();
    t_porcion_dato* dato_escribir = malloc(sizeof(t_porcion_dato));
    dato_escribir->direccion_fisica=2;
    dato_escribir->base=0;
    dato_escribir->tamanio=2;
    list_add(lista_porciones,dato_escribir);
    t_porcion_dato* dato_escribir2 = malloc(sizeof(t_porcion_dato));
    dato_escribir2->direccion_fisica=10; 
    dato_escribir2->base=2;
    dato_escribir2->tamanio=2;
    list_add(lista_porciones,dato_escribir2);

    //printf("\nTAMANIO LISTA: %d\n",)

    escribir_dato_user_space(lista_porciones,prueba);

    uint32_t num_reconstruido = 0;
    void* ptr_num_reconstruido = &num_reconstruido;

    char* reconstruido = malloc(5);


    memcpy(reconstruido,user_space+2,2);
    memcpy(reconstruido+2,user_space+10,2);

    reconstruido[4] = '\0';

    printf("\nCADENA RECONSTRUIDA: %s\n",reconstruido);

    //ponele q las paginas sean de 4bytes, y yo quiero escribir SISTEMAS OPERATIVOS
    // son strlen(SISTEMAS OPERATIVOS) son 19b, entonces 19/4 , necesito 5 paginas 

    
    char* s = "SISTEMAS OPERATIVOS";
    int offset = 0;
    char* ss = string_substring(s,offset,4);
    offset += 4;
    printf("\ncad 1: %s\n", ss);
    memcpy(user_space,ss,strlen(ss));
    ss = string_substring(s,offset,4);
    offset += 4;
    printf("\ncad 1: %s\n", ss);
    memcpy(user_space+4,ss,strlen(ss));
    ss = string_substring(s,offset,4);
    offset += 4;
    
    memcpy(user_space+8,ss,strlen(ss));



    printf("\nc : %c\n", *((char*)user_space));
    printf("\nc : %c\n", *(((char*)user_space)+1));
    printf("\nc : %c\n", *(((char*)user_space)+2));
    printf("\nc : %c\n", *(((char*)user_space)+3));
    printf("\nc : %c\n", *(((char*)user_space)+4));
    
    void* hola = malloc(3);

    // Copia los caracteres 'h' y 'o' en el bloque de memoria
    char* ho = "ho";
    //memcpy(hola, ho, 2);
    memcpy(hola,ho,3);

    // Lee los caracteres del bloque de memoria
    char caracter1 = *((char*)hola); // Convierte el puntero void* a char*
    char caracter2 = *(((char*)hola) + 1); 

    
    // Imprime los caracteres
    printf("\nchar 1: %c\n", caracter1);
    printf("\nchar 2: %c\n", caracter2);
    
    ////////////////////////////////////////////////////////////////

    
    
    
    t_tabla_pag* tablita = crear_tabla_paginas(4096,32);
    //int marco = consultar_marco(15,tablita);
    if(marco == MARCO_INVALIDO)
    {
        log_error(logger,"CONSULTASTE UN MARCO INVALIDO PAPAH");
    }
    
    /////////////////////////////////////////////////////////////////
    
    return 0; //puede ir exit_sucess
}*/
/*
void* imprimir_marco(void* element)
{
    printf("MARCO: %d",element->marco);
}*/











