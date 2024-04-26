#ifndef CPU_DISPATCH_H
#define CPU_DISPATCH_H

#include <stdio.h>
#include <stdlib.h>
#include "../../utils/include/protocolo.h"

void realizar_handshake_dispatch(int cliente_fd_conexion_dispatch);
void manejarConexionDispatch(int cliente_fd_conexion_dispatch);

#endif