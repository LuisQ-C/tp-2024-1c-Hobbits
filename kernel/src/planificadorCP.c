#include "../include/planificadorCP.h"

extern t_config* config;
extern t_log* logger;

extern t_squeue *lista_procesos_new;
extern t_squeue *lista_procesos_ready;
extern t_squeue *lista_procesos_exec;
extern t_squeue *lista_procesos_exit;
extern t_slist *lista_procesos_blocked;
extern t_slist *lista_recursos_blocked;
extern t_sdictionary *instancias_utilizadas;

extern sem_t grado_de_multiprogramacion;
extern sem_t proceso_en_cola_new;
extern sem_t proceso_en_cola_ready;
extern sem_t ejecutar_proceso;
extern sem_t pasar_a_ejecucion;

extern sem_t planificacion_new_iniciada;
extern sem_t planificacion_ready_iniciada;
extern sem_t planificacion_exec_iniciada;
extern bool planificacion_iniciada;

extern int fd_dispatch;
extern int fd_interrupt;
extern int fd_mem;

extern bool interrupcion_usuario;

int quantum;
char* algoritmo;

void iniciar_PCP(){
    algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");


    pthread_t hilo_CP;
    if(strcmp(algoritmo, "FIFO")==0){
        pthread_create(&hilo_CP, NULL, (void*) planificacion_fifo, NULL);

        pthread_t hilo_ejecucion;
        pthread_create(&hilo_ejecucion, NULL, (void *) ejecutar_procesos_exec, NULL);
        pthread_detach(hilo_ejecucion);
    }
    if(strcmp(algoritmo, "RR")==0){
        quantum = config_get_int_value(config, "QUANTUM");
        pthread_create(&hilo_CP, NULL, (void*) planificacion_rr, NULL);
    }
    if(strcmp(algoritmo, "VRR") == 0){
        quantum = config_get_int_value(config, "QUANTUM");
        pthread_create(&hilo_CP, NULL, (void*) planificacion_vrr, NULL);
    }
    
    pthread_detach(hilo_CP);

    //log_debug(logger, "Planificacion %s iniciada", algoritmo); // ---- POR ALGUNA RAZON NO LO IMPRE ---> NO IMPORTA

    //CREAR OTRO HILO PARA MANDAR PROCESOS A BLOCKED

}

//HILO ENCARGADO DE MANEJAR LOS PROCESOS DE BLOCKED, CON QUE CRITERIO? NI IDEA
//SEMAFORO QUE TIENE CADA IO TE PUEDE AVISAR CUANDO LE PODES MANDAR UNA SOLICITUD DE VUELTA

void recibir_contexto_actualizado(int fd_dispatch)
{
    recibir_operacion(fd_dispatch);
    t_list* pcb_con_motivo = recibir_paquete(fd_dispatch);
    sem_wait(&planificacion_exec_iniciada);

    if(interrupcion_usuario){
        int* aux = malloc(sizeof(int));
        *aux = USER_INTERRUPT;
        int* valor_reemplazado = list_replace(pcb_con_motivo, 5, aux);
        free(valor_reemplazado);
        interrupcion_usuario = false;
        
    }
    
    t_pcb* pcb_a_actualizar = squeue_pop(lista_procesos_exec);
    actualizar_pcb_ejecutado(pcb_a_actualizar,pcb_con_motivo);
    bool hay_que_replanificar = manejar_motivo_interrupcion(pcb_a_actualizar,pcb_con_motivo);
    sem_post(&planificacion_exec_iniciada);
    list_destroy_and_destroy_elements(pcb_con_motivo,(void*) liberar_elemento);
    if(hay_que_replanificar){
        recibir_contexto_actualizado(fd_dispatch);
    }
}

void actualizar_pcb_ejecutado(t_pcb* pcb_a_actualizar,t_list* pcb_con_motivo)
{
    int* pid = list_get(pcb_con_motivo,0);
    uint32_t* pc = list_get(pcb_con_motivo,1);
    uint32_t* estado = list_get(pcb_con_motivo,2);
    int* quantum = list_get(pcb_con_motivo,3);
    log_trace(logger, "%d ESTOY ACTUALIZANDO EL PCB", *quantum);
    t_registros_generales* registros_generales = list_get(pcb_con_motivo,4);
    pcb_a_actualizar->pid = *pid;
    pcb_a_actualizar->pc = *pc;
    pcb_a_actualizar->quantum = *quantum;
    pcb_a_actualizar->estado = *estado;
    pcb_a_actualizar->registros_CPU = *registros_generales;
}
bool manejar_motivo_interrupcion(t_pcb* pcb_a_actualizar,t_list* pcb_con_motivo)
{
    int* motivo_interrupcion = list_get(pcb_con_motivo,5);
    switch(*motivo_interrupcion)
    {
        case EXIT:
            manejar_fin_con_motivo(SUCCESS, pcb_a_actualizar);
            sem_post(&grado_de_multiprogramacion);
            return false;
            break;
        case IO_GEN_SLEEP:
        {
            char* nombre_interfaz = list_get(pcb_con_motivo,6);
            int* tiempo_dormicion = list_get(pcb_con_motivo,7);
            //t_list_io* prueba = list_get(lista_procesos_blocked->lista,0);
            //printf("\nAccediendo desde otro hilo, primer elem lista %s\n",prueba->nombre_interfaz);
            bool validez = slist_comprobate_io(nombre_interfaz,IO_GEN_SLEEP);
            if(validez)
            {
                t_list_io* interfaz_buscada = slist_buscar_interfaz(nombre_interfaz); //habria que juntarlo con validez
                pcb_a_actualizar->estado = BLOCKED;
                t_elemento_iogenerica* nueva_solicitud_gen = malloc(sizeof(t_elemento_iogenerica));
                //nueva_solicitud_gen->cola_destino = READY; //ACA DEFINIRIAS LO DE VRR
                if(strcmp(algoritmo, "VRR") == 0 && pcb_a_actualizar->quantum <= 0){
                    pcb_a_actualizar->quantum = quantum;
                    nueva_solicitud_gen->cola_destino = READY;
                }
                else if(strcmp(algoritmo, "VRR") == 0 && pcb_a_actualizar->quantum>0){
                    nueva_solicitud_gen->cola_destino = READYPLUS;
                }
                else 
                    nueva_solicitud_gen->cola_destino = READY;

                nueva_solicitud_gen->pcb= pcb_a_actualizar;
                nueva_solicitud_gen->tiempo = *tiempo_dormicion;
                push_elemento_cola_io(interfaz_buscada,nueva_solicitud_gen);
                /*
                if(NO EJECUTO TODO SU QUANTUM){
                    int numero de instrucciones ejecutadas =list_get(pcb_con_motivo, 8)
                    pcb_a_actualizar->quantum = pcb_a_actualizar->quantum - (instrucciones_ejecutadas * retardo_memoria);

                }

                */
                log_info(logger, "PID: %d - Estado Anterior: EXECUTE - Estado Actual: BLOCKED", pcb_a_actualizar->pid);
                sem_post(interfaz_buscada->hay_proceso_cola);
                return false;
            }
            else
            {
                //pcb_a_actualizar->estado = EXIT;
                //squeue_push(lista_procesos_exit,pcb_a_actualizar);
                manejar_fin_con_motivo(INVALID_INTERFACE, pcb_a_actualizar);

                //log_info(logger, "PID: %d - Estado Anterior: EXECUTE - Estado Actual: EXIT", pcb_a_actualizar->pid);
                /*
                int tamanio= strlen(nombre_interfaz);
                printf("\ntamanio string: %d, contenido: %s\n",tamanio,nombre_interfaz);
                printf("\ninterfaz invalida\n");*/

                sem_post(&grado_de_multiprogramacion);
                return false;
            }
            break;
        }
        case IO_STDIN_READ:
        {
            char* nombre_interfaz = list_get(pcb_con_motivo,6);
            int tam_lista = list_size(pcb_con_motivo);
            int cant_direcciones = tam_lista - 7; //los primeros 8 son necesarios para todos los ios
            
            bool validez = slist_comprobate_io(nombre_interfaz,IO_STDIN_READ);
            if(validez)
            {
                t_list* direcciones = list_slice_and_remove(pcb_con_motivo, 7, cant_direcciones);
                t_list_io* interfaz_buscada = slist_buscar_interfaz(nombre_interfaz); //habria que juntarlo con validez
                pcb_a_actualizar->estado = BLOCKED;
                t_elemento_io_in_out* nueva_solicitud_stdin= malloc(sizeof(t_elemento_io_in_out));
               // nueva_solicitud_stdin->cola_destino=READY;
                if(strcmp(algoritmo, "VRR") == 0 && pcb_a_actualizar->quantum <= 0){
                    pcb_a_actualizar->quantum = quantum;
                    nueva_solicitud_stdin->cola_destino = READY;
                }
                else if(strcmp(algoritmo, "VRR") == 0 && pcb_a_actualizar->quantum>0){
                    nueva_solicitud_stdin->cola_destino = READYPLUS;
                }
                else
                    nueva_solicitud_stdin->cola_destino = READY;

                nueva_solicitud_stdin->pcb = pcb_a_actualizar;
                nueva_solicitud_stdin->direcciones_fisicas = direcciones;
                push_elemento_cola_io(interfaz_buscada,nueva_solicitud_stdin);

                //log_warning(logger, "PID: %d Fue desalojado por IO_STDIN_READ ", pcb_a_actualizar->pid);

                sem_post(interfaz_buscada->hay_proceso_cola);

                log_info(logger, "PID: %d - Estado Anterior: EXECUTE - Estado Actual: BLOCKED", pcb_a_actualizar->pid);
                /*
                if(pcb_a_actualizar->quantum>0){
                    squeue_push(lista_procesos_ready_plus, pcb_a_actualizar);
                    log_info(logger, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY PLUS", solicitud_io->pcb->pid);
                    sem_post(&proceso_en_cola_ready_plus)
                }
                else{
                    squeue_push(lista_procesos_ready, pcb_a_actualizar);
                    log_info(logger, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY", solicitud_io->pcb->pid);
                    sem_post(&proceso_en_cola_ready);
                }*/
            }
            else
            {
                manejar_fin_con_motivo(INVALID_INTERFACE, pcb_a_actualizar);
                /*
                int tamanio= strlen(nombre_interfaz);
                printf("\ntamanio string: %d, contenido: %s\n",tamanio,nombre_interfaz);
                printf("\ninterfaz invalida\n");*/

                sem_post(&grado_de_multiprogramacion);
            }
            return false;
            break;
        }
        case IO_STDOUT_WRITE:
        {
            char* nombre_interfaz = list_get(pcb_con_motivo,6);
            int tam_lista = list_size(pcb_con_motivo);
            int cant_direcciones = tam_lista - 7; //los primeros 8 son necesarios para todos los ios
            
            bool validez = slist_comprobate_io(nombre_interfaz,IO_STDOUT_WRITE);
            if(validez)
            {
                t_list* direcciones = list_slice_and_remove(pcb_con_motivo, 7, cant_direcciones);
                t_list_io* interfaz_buscada = slist_buscar_interfaz(nombre_interfaz); //habria que juntarlo con validez
                pcb_a_actualizar->estado = BLOCKED;
                t_elemento_io_in_out* nueva_solicitud_stdin= malloc(sizeof(t_elemento_io_in_out));
                //nueva_solicitud_stdin->cola_destino=READY;
                if(strcmp(algoritmo, "VRR") == 0 && pcb_a_actualizar->quantum <= 0){
                    pcb_a_actualizar->quantum = quantum;
                    nueva_solicitud_stdin->cola_destino = READY;
                }
                else if(strcmp(algoritmo, "VRR") == 0 && pcb_a_actualizar->quantum>0){
                    nueva_solicitud_stdin->cola_destino = READYPLUS;
                }
                else 
                    nueva_solicitud_stdin->cola_destino = READY;


                nueva_solicitud_stdin->pcb = pcb_a_actualizar;
                nueva_solicitud_stdin->direcciones_fisicas = direcciones;
                push_elemento_cola_io(interfaz_buscada,nueva_solicitud_stdin);

                //log_warning(logger, "PID: %d Fue desalojado por IO_STDOUT_WRITE ", pcb_a_actualizar->pid);

                sem_post(interfaz_buscada->hay_proceso_cola);

                /*
                if(pcb_a_actualizar->quantum>0){
                    squeue_push(lista_procesos_ready_plus, pcb_a_actualizar);
                    log_info(logger, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY PLUS", solicitud_io->pcb->pid);
                    sem_post(&proceso_en_cola_ready_plus)
                }
                else{
                    squeue_push(lista_procesos_ready, pcb_a_actualizar);
                    log_info(logger, "PID: %d - Estado Anterior: BLOCKED - Estado Actual: READY", solicitud_io->pcb->pid);
                    sem_post(&proceso_en_cola_ready);
                }*/
            }
            else
            {
                manejar_fin_con_motivo(INVALID_INTERFACE, pcb_a_actualizar);
                /*
                int tamanio= strlen(nombre_interfaz);
                printf("\ntamanio string: %d, contenido: %s\n",tamanio,nombre_interfaz);
                printf("\ninterfaz invalida\n");*/

                sem_post(&grado_de_multiprogramacion);
            }
            return false;
            break;
        }
        case IO_FS_CREATE:
                return false;
            break;
        case IO_FS_DELETE:
                return false;
            break;
        case IO_FS_TRUNCATE:
                return false;
            break;
        case IO_FS_WRITE:
                return false;
            break;
        case IO_FS_READ:
                return false;
            break;
        case INTERRUPCION_QUANTUM:
            log_warning(logger, "PID: %d Fue desalojado por fin de Q", pcb_a_actualizar->pid);
            pcb_a_actualizar->estado = READY;
            pcb_a_actualizar->quantum = quantum; //PARA VRR
            squeue_push(lista_procesos_ready, pcb_a_actualizar);
            log_info(logger, "PID: %d - Estado Anterior: EXECUTE - Estado Actual: READY", pcb_a_actualizar->pid);
            mostrar_cola_ready();
            sem_post(&proceso_en_cola_ready);
            return false;

            break;
        case USER_INTERRUPT:
            //log_warning(logger, "PID: %d Fue desalojado por interrupcion de usuario", pcb_a_actualizar->pid);
            manejar_fin_con_motivo(INTERRUPTED_BY_USER_EXEC, pcb_a_actualizar);
            return false;

            break;
        case WAIT:
        {
            
            //log_info(logger, "LLEGUE AL WAIT");
            char* nombre_recurso = list_get(pcb_con_motivo, 6);
            pthread_mutex_lock(lista_recursos_blocked->mutex);
            bool esValido = existe_recurso(nombre_recurso);
            pthread_mutex_unlock(lista_recursos_blocked->mutex);
            //log_trace(logger, "PASE LA FUNCION DE EXISTE RECURSO");
            //t_instancias_usadas* auxiliar_asdasd = malloc(sizeof(t_instancias_usadas));
            if(esValido){
                //log_info(logger, "ES VALIDO EL RECURSO");
                /*
                int _es_el_recurso(t_recurso* r){
                    int recurso_encontrado = strcmp(r->nombre, nombre_recurso);
                    return recurso_encontrado;
                }
                t_recurso* recurso_usado = list_find(lista_recursos_blocked, (void*) _es_el_recurso);
                */
                pthread_mutex_lock(lista_recursos_blocked->mutex);
                t_recurso* recurso_usado = buscar_recurso(nombre_recurso); //Utiliza lista_recursos_blocked->lista
                recurso_usado->instancias_recurso -= 1;
                pthread_mutex_unlock(lista_recursos_blocked->mutex);
                char* pid = string_itoa(pcb_a_actualizar->pid);

                if(sdictionary_has_key(instancias_utilizadas, pid)){
                    t_list* aux_lista = sdictionary_get(instancias_utilizadas, pid);
                    
                    bool _el_recurso_es_usado(t_instancias_usadas* iu){
                        bool esUsado = strcmp(iu->nombre, nombre_recurso) == 0;
                        return esUsado;
                    }
                    
                    if(list_any_satisfy(aux_lista, (void*) _el_recurso_es_usado)){
                        t_instancias_usadas* aux_inst = list_find(aux_lista, (void*) _el_recurso_es_usado);
                        aux_inst->cantInstanciasUtil++;

                    }
                    else{
                        t_instancias_usadas* nueva_instancia_usada = malloc(sizeof(t_instancias_usadas));
                        nueva_instancia_usada->nombre = string_duplicate(nombre_recurso);
                        nueva_instancia_usada->cantInstanciasUtil = 1;
                        list_add(aux_lista, nueva_instancia_usada);
                    } 

                    sdictionary_put(instancias_utilizadas, pid, aux_lista);


                    //usar list_any_satisfy
                    //SI EXISTE LA KEY ES PORQUE YA USO UN RECURSO ALGUNA VEZ
                    //DEBO EXTRAER LA LISTA ASOCIADA A ESA KEY CON UN T_LIST AUX?
                    //ACCEDO AL INDEX DONDE ESTA EL NOMBRE DE RECURSO
                    //SI NO EXISTE ESE RECURSO EN LA LISTA CREO UN NUEVO T_INSTANCIAS_USADAS
                    //Y A LA CANTIDAD ASOCIADA A ESE NOMBRE DE RECURSO LE SUMO UNO
                    //AL TERMINAR DE MODIFICARLO UTILIZO SD_PUT Y SOBREESCRIBO LA POSICION DE LA KEY
                }else{
                    t_list* nueva_lista_para_dicc = list_create();
                    t_instancias_usadas* nueva_instancia_usada = malloc(sizeof(t_instancias_usadas));
                    nueva_instancia_usada->nombre = string_duplicate(nombre_recurso);
                    nueva_instancia_usada->cantInstanciasUtil = 1;
                    list_add(nueva_lista_para_dicc, nueva_instancia_usada);
                    sdictionary_put(instancias_utilizadas, pid, nueva_lista_para_dicc);
                    //SI NUNCA ESTUVO EN EL DICC ENTONCES LE TENGO QUE CREAR UNA LISTA
                    //Y LUEGO VUELVO A CREAR UN 
                    //T_INSTANCIAS_USADAS* AUXILIAR Y LE HAGO MALLOC
                    //LUEGO HAGO LIST_ADD AUXILIAR Y ESA LISTA LA METO EN EL DICCIONARIO
                    //CON SD_PUT Y LA KEY EN ESTE CASO PID
                }
                free(pid);
                
                if(recurso_usado->instancias_recurso >= 0){
                    /*
                    if(pcb_a_actualizar->quantum > 0 || (strcmp(algoritmo, "FIFO") == 0)){
                        pthread_mutex_lock(lista_procesos_ready->mutex);
                        list_add_in_index(lista_procesos_ready->cola->elements, 0, pcb_a_actualizar);
                        pthread_mutex_unlock(lista_procesos_ready->mutex);
                    }
                    else{
                        pcb_a_actualizar->quantum = quantum;
                        squeue_push(lista_procesos_ready, pcb_a_actualizar);
                        //ACA BASICAMENTE SE REPITE LA LOGICA DE INTERRUPCION_QUANTUM
                    } 
                        sem_post(&proceso_en_cola_ready);
                    */
                    squeue_push(lista_procesos_exec, pcb_a_actualizar);
        
                    int ok = MISMO_PID;
                    send(fd_dispatch, &ok, sizeof(int), 0);        
                    return true;
                }
                else if(recurso_usado->instancias_recurso < 0){
                    pcb_a_actualizar->quantum = quantum;
                    //log_debug(logger, "ME FUI A LA COLA DE BLOCKED POR EL RECURSO");
                    squeue_push(recurso_usado->cola_blocked, pcb_a_actualizar);
                    int ok = NUEVO_PID;
                    send(fd_dispatch, &ok, sizeof(int), 0);
                    return false;
                }

            }
            else{
                //log_info(logger, "NO ES VALIDO EL NOMBRE DEL RECURSO");
                manejar_fin_con_motivo(INVALID_RESOURCE, pcb_a_actualizar);
                int ok = NUEVO_PID;
                send(fd_dispatch, &ok, sizeof(int), 0);
                sem_post(&grado_de_multiprogramacion);
                return false;
            }
        
            break;}
        case SIGNAL:
            //log_info(logger, "LLEGUE AL SIGNAL");
            char* nombre_recurso = list_get(pcb_con_motivo, 6);
            pthread_mutex_lock(lista_recursos_blocked->mutex);
            bool esValido = existe_recurso(nombre_recurso);
            pthread_mutex_unlock(lista_recursos_blocked->mutex);
            //log_trace(logger, "PASE LA FUNCION DE EXISTE RECURSO");
            if(esValido){
                pthread_mutex_lock(lista_recursos_blocked->mutex);
                t_recurso* recurso_usado = buscar_recurso(nombre_recurso); //Utiliza lista_recursos_blocked->lista
                recurso_usado->instancias_recurso += 1;
                pthread_mutex_unlock(lista_recursos_blocked->mutex);
                char* pid = string_itoa(pcb_a_actualizar->pid);

             
                

                //ESTA PARTE ES POR SI SE LIBERA EL RECURSO
            
                if(!squeue_is_empty(recurso_usado->cola_blocked)){
                    t_pcb* pcb_en_cola_blocked = squeue_pop(recurso_usado->cola_blocked);
                    pcb_en_cola_blocked->estado = READY;
                    squeue_push(lista_procesos_ready, pcb_en_cola_blocked);
                    sem_post(&proceso_en_cola_ready);
                }

                if(sdictionary_has_key(instancias_utilizadas, pid)){
                    t_list* aux_lista = sdictionary_get(instancias_utilizadas, pid);

                    bool _el_recurso_es_usado(t_instancias_usadas* iu){
                        bool esUsado = strcmp(iu->nombre, nombre_recurso) == 0;
                        return esUsado;
                    }
                    
                    if(list_any_satisfy(aux_lista, (void*) _el_recurso_es_usado)){
                        t_instancias_usadas* aux_inst = list_find(aux_lista, (void*) _el_recurso_es_usado);
                        aux_inst->cantInstanciasUtil--;
                        //log_trace(logger, "HOLA DESDE LA LISTA DE RECURSOS USADA");
                    }

                }

                /*
                if(pcb_a_actualizar->quantum > 0 || (strcmp(algoritmo, "FIFO") == 0)){
                    pthread_mutex_lock(lista_procesos_ready->mutex);
                    list_add_in_index(lista_procesos_ready->cola->elements, 0, pcb_a_actualizar);
                    pthread_mutex_unlock(lista_procesos_ready->mutex);
                }
                else {
                    pcb_a_actualizar->quantum = quantum;
                    squeue_push(lista_procesos_ready, pcb_a_actualizar);
                }
                sem_post(&proceso_en_cola_ready);
                */
                squeue_push(lista_procesos_exec, pcb_a_actualizar);
                int ok = MISMO_PID;
                send(fd_dispatch, &ok, sizeof(int), 0);
                return true;

            }
            else {
                //log_info(logger, "NO ES VALIDO EL NOMBRE DEL RECURSO");
                manejar_fin_con_motivo(INVALID_RESOURCE, pcb_a_actualizar);
                int ok = NUEVO_PID;
                send(fd_dispatch, &ok, sizeof(int), 0);
                sem_post(&grado_de_multiprogramacion);
                return false;
            }


            break;
        case OUT_OF_MEMORY:
        {
            manejar_fin_con_motivo(OUT_OF_MEMORY_FIN, pcb_a_actualizar);
            sem_post(&grado_de_multiprogramacion);
            return false;
            break;
        }
        default:
            log_warning(logger,"MOTIVO DE DESALOJO DESCONOCIDO");
            return false;
            break;
    }
    return false;
}





/*
void fin_fin_ready(){
    sem_wait(&proceso_en_cola_ready);
    int hola;
    sem_getvalue(&proceso_en_cola_ready, &hola);
    printf("\n %d \n", hola);
    if(hola <= 0)    
        sem_post(&proceso_en_cola_ready);
}

void fin_fin(){
    sem_wait(&proceso_en_cola_new);
    int hola;
    sem_getvalue(&proceso_en_cola_new, &hola);
    printf("\n %d \n", hola);
    if(hola <= 0)    
        sem_post(&proceso_en_cola_new);
}
*/

void hilo_quantum(void* arg)
{
    data* quantum_recibido = arg;
    int quantum_r = quantum_recibido->quantum;
    int pid_r = quantum_recibido->pid;
    free(quantum_recibido);
    usleep(quantum_r*1000);
    enviar_interrupcion(INTERRUPCION_QUANTUM, pid_r,fd_interrupt);
}

/*
hilo que manda a ejecutar a las interfaces

le manda la operacion, recibe el ok de la interfaz, saca el proceso de blocked y lo manda a ready

//con criterio va ir vaciando

//el hilo que cada interfaz

//semaforo de cada interfaz
//cuando yo le mando algo a la interfaz lo seteo en 0

//cuando me da el ok hago signal, entonces el hilo va a hacer pop del proceso y lo manda a ready
//y procede a sacar el 2do proceso de la cola

*/
