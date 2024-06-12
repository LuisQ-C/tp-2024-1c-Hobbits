#include "../include/conversores.h"

extern t_registro_cpu registro;

/*Funcion que convierte char* a opcode del enum, en caso de error retorna -1*/
int string_to_opcode(char* instruccion)
{
    if(string_equals_ignore_case("SET",instruccion))
    {
        return SET;
    }
    else if(string_equals_ignore_case("SUM",instruccion))
    {
        return SUM;
    }
    else if(string_equals_ignore_case("SUB",instruccion))
    {
        return SUB;
    }
    else if(string_equals_ignore_case("JNZ",instruccion))
    {
        return JNZ;
    }
    else if(string_equals_ignore_case("IO_GEN_SLEEP",instruccion))
    {
        return IO_GEN_SLEEP;
    }
    else if(string_equals_ignore_case("RESIZE",instruccion))
    {
        return RESIZE;
    }
    else if(string_equals_ignore_case("MOV_IN",instruccion))
    {
        return MOV_IN;
    }
    else if(string_equals_ignore_case("MOV_OUT",instruccion))
    {
        return MOV_OUT;
    }
    
    else if(string_equals_ignore_case("EXIT",instruccion))
    {
        return EXIT;
    }
    else if(string_equals_ignore_case("COPY_STRING",instruccion))
    {
        return COPY_STRING;
    }
    else if(string_equals_ignore_case("SIGNAL",instruccion))
    {
        return SIGNAL;
    }
    else if(string_equals_ignore_case("WAIT",instruccion))
    {
        return WAIT;
    }
    else if(string_equals_ignore_case("IO_STDIN_READ",instruccion))
    {
        return IO_STDIN_READ;
    }
    else if(string_equals_ignore_case("IO_STDOUT_WRITE",instruccion))
    {
        return IO_STDOUT_WRITE;
    }
    else
    {
        return -1;
    }
}

/*Funcion que convierte char* a uint8_t*, en caso de error retorna NULL*/
uint8_t* string_to_register8(char* registroConvertir)
{
    if(string_equals_ignore_case("AX",registroConvertir))
    {
        return &registro.AX;
    }
    else if(string_equals_ignore_case("BX",registroConvertir))
    {
        return &registro.BX;
    }
    else if(string_equals_ignore_case("CX",registroConvertir))
    {
        return &registro.CX;
    }
    else if(string_equals_ignore_case("DX",registroConvertir))
    {
        return &registro.DX;
    }
    else{
        return NULL;
    }
}

/*Funcion que obtiene el valor del registro*/
int obtener_valor_registro(char* registro_chequear)
{
    if(strcmp(registro_chequear, "AX") == 0) {
        return registro.AX;
    } else if(strcmp(registro_chequear, "BX") == 0) {
        return registro.BX;
    } else if(strcmp(registro_chequear, "CX") == 0) {
        return registro.CX;
    } else if(strcmp(registro_chequear, "DX") == 0) {
        return registro.DX;
    } else if(strcmp(registro_chequear, "EAX") == 0) {
        return registro.EAX;
    } else if(strcmp(registro_chequear, "EBX") == 0) {
        return registro.EBX;
    } else if(strcmp(registro_chequear, "ECX") == 0) {
        return registro.ECX;
    } else if(strcmp(registro_chequear, "EDX") == 0) {
        return registro.EDX;
    } else if(strcmp(registro_chequear, "SI") == 0) {
        return registro.SI;
    } else if(strcmp(registro_chequear, "DI") == 0) {
        return registro.DI;
    } else {
        // Retorna -1 si el registro no es encontrado
        return -1;
    }
}

/*Funcion que convierte char* a uint32_t*, en caso de error retorna NULL*/
uint32_t* string_to_register32(char* registroConvertir)
{
    if(string_equals_ignore_case("EAX",registroConvertir))
    {
        return &registro.EAX;
    }
    else if(string_equals_ignore_case("EBX",registroConvertir))
    {
        return &registro.EBX;
    }
    else if(string_equals_ignore_case("ECX",registroConvertir))
    {
        return &registro.ECX;
    }
    else if(string_equals_ignore_case("EDX",registroConvertir))
    {
        return &registro.EDX;
    }
    else if(string_equals_ignore_case("PC",registroConvertir))
    {
        return &registro.PC;
    }
    else if(string_equals_ignore_case("DI",registroConvertir))
    {
        return &registro.DI;
    }
    else if(string_equals_ignore_case("SI",registroConvertir))
    {
        return &registro.SI;
    }
    else{
        return NULL;
    }
}

