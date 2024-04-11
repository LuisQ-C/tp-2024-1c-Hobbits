#include "../include/protocolo.h"

void mandarHandshake(t_log* logger,int fd_destinatario, char* nombreDestinatario,int32_t valorHandshake)
{
    int32_t result = 0;
    while(1)
    {   
        send(fd_destinatario,&valorHandshake,sizeof(int32_t),0);
        recv(fd_destinatario,&result,sizeof(int32_t),MSG_WAITALL);
        if(result==0)
        {
            log_info(logger,"HANDSHAKE ACEPTADO DE: %s!",nombreDestinatario);
        }else{
            log_error(logger,"HANDSHAKE DENEGADO DE: %s!",nombreDestinatario);
        }
        break;
    }
    close(fd_destinatario);
}

void recibirHandshake(t_log* logger,int fd_origen, char* nombreOrigen,int32_t valorHandshake)
{
    int32_t resultOk = 0;
    int32_t resultError = -1;
    while(1)
    {
        recv(fd_origen, &valorHandshake, sizeof(int32_t), MSG_WAITALL);
        if (valorHandshake == 1) {
            log_info(logger,"HANDSHAKE ACEPTADO A: %s!", nombreOrigen);
            send(fd_origen, &resultOk, sizeof(int32_t), 0);
        } else {
            log_error(logger,"HANDSHAKE DENEGADO A: %s!", nombreOrigen);
            send(fd_origen, &resultError, sizeof(int32_t), 0);
        }
        break;
    }
    close(fd_origen);
}