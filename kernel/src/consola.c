#include "../include/consola.h"
#include "../include/main.h"

extern t_log* logger;
char* opciones[] = {
    "EJECUTAR_SCRIPT",
    "INICIAR_PROCESO",
    "FINALIZAR_PROCESO",
    "DETENER_PLANIFICACION",
    "INICIAR_PLANIFICACION",
    "MULTIPROGRAMACION",
    "PROCESO_ESTADO",
    NULL
};
//Por parte de la documentación compartida en UTNSO
char** custom_completion(const char* text, int start, int end){
    char** matches = NULL;

    if (start == 0) {
        matches = rl_completion_matches(text, &custom_completion_generator);
    } else {
        rl_attempted_completion_over = 1;
    }

    return matches;
}

char* custom_completion_generator(const char* text, int state){
    static int list_index, len;
    const char* name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = opciones[list_index])) {
        list_index++;

        if (strncasecmp(name, text, len) == 0) {
            return strdup(name);
        }
    }
    return NULL;
}

void iniciar_consola(){
    //t_log* logger;
    //logger = iniciar_logger("kernel.log","Kernel",1,LOG_LEVEL_INFO);

    rl_attempted_completion_function = custom_completion;

    char* leido;
    printf("EJECUTAR_SCRIPT [PATH] \n");
    printf("INICIAR_PROCESO [PATH] \n");
    printf("FINALIZAR_PROCESO [PID] \n");
    printf("DETENER_PLANIFICACION \n");
    printf("INICIAR_PLANIFICACION \n");
    printf("MULTIPROGRAMACION [VALOR]\n");
    printf("PROCESO_ESTADO \n");

    leido = readline("> ");
    bool validarComando;
    while(!string_is_empty(leido)){
        validarComando = validar_instrucciones_leidas(leido);
        if(!validarComando){
            log_error(logger, "Comando no reconocido");
            free(leido);
            leido = readline("> ");
            continue;
        }
        add_history(leido);
        instrucciones_consola(leido);
        free(leido);
        leido = readline("> ");
    }
    free(leido);
}

bool validar_instrucciones_leidas(char* leido){
    char** instruccion_leida = string_split(leido, " ");
    bool valido;

    if(strcmp(instruccion_leida[0], "EJECUTAR_SCRIPT") == 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "INICIAR_PROCESO") == 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "FINALIZAR_PROCESO") == 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "DETENER_PLANIFICACION") == 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "INICIAR_PLANIFICACION") == 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "MULTIPROGRAMACION") == 0)
        valido = true;
    else if (strcmp(instruccion_leida[0], "PROCESO_ESTADO") == 0)
        valido = true;
    else 
        valido = false;

    string_array_destroy(instruccion_leida);

    return valido;
}

void instrucciones_consola(char* leido){
    char** instruccion_leida = string_split(leido, " ");

    if(strcmp(instruccion_leida[0], "EJECUTAR_SCRIPT") == 0)
        ejecutar_script(instruccion_leida[1]);
    else if (strcmp(instruccion_leida[0], "INICIAR_PROCESO") == 0)
        iniciar_proceso(instruccion_leida[1]);
    else if (strcmp(instruccion_leida[0], "FINALIZAR_PROCESO") == 0)
        finalizar_proceso(atoi(instruccion_leida[1]));
    else if (strcmp(instruccion_leida[0], "DETENER_PLANIFICACION") == 0)
        detener_planificacion();
    else if (strcmp(instruccion_leida[0], "INICIAR_PLANIFICACION") == 0)
        iniciar_planificacion();
    else if (strcmp(instruccion_leida[0], "MULTIPROGRAMACION") == 0)
        multiprogramacion(atoi(instruccion_leida[1]));
    else if (strcmp(instruccion_leida[0], "PROCESO_ESTADO") == 0)
        proceso_estado();

    string_array_destroy(instruccion_leida);
}

void ejecutar_script(char* path){
    printf("ejecutar_script \n");
}

void iniciar_proceso(char* path){
    printf("iniciar_proceso \n");
}

void finalizar_proceso(int pid){
    printf("finalizar_proceso \n");
}

void detener_planificacion(){
    printf("detener_planificador \n");
}

void iniciar_planificacion(){
    printf("iniciar_planificacion \n");
}

void multiprogramacion(int valor){
    printf("multiprogramacion \n");
}

void proceso_estado(){
    printf("proceso_estado \n");
}