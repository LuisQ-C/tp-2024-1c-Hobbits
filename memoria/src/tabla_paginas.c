#include "../include/tabla_paginas.h"

extern t_log* logger;


t_tabla_pag* crear_tabla_paginas(int tam_memoria,int tam_pagina)
{
    int cant_entradas = tam_memoria / tam_pagina;
    t_tabla_pag* tabla_pagina = malloc(sizeof(t_tabla_pag) * cant_entradas);
    for(int i=0;i<cant_entradas;i++)
    {
        tabla_pagina[i].marco= -1;
        tabla_pagina[i].validez_marco = 0;
        
    }
    for(int i=0;i<cant_entradas;i++)
    {
        printf("\nPAGINA: %d -> MARCO: %d,VALDEZ: %d\n" ,i,tabla_pagina[i].marco,tabla_pagina[i].validez_marco);
        
    }
    return tabla_pagina;
}

int consultar_marco(int num_pagina,t_tabla_pag* tabla_pag)
{
    // tendria q buscar segun PID
    if(tabla_pag[num_pagina].validez_marco == 1)
    {
        int marco_buscado = tabla_pag[num_pagina].marco;
        return marco_buscado;
    }
    else{
        log_error(logger,"NO TE PERTENECE FLACO");
        return MARCO_INVALIDO;
    }
    
}