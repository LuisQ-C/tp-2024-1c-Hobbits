#include "../include/conversores.h"

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
    else if(string_equals_ignore_case("EXIT",instruccion))
    {
        return EXIT;
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
    else{
        return NULL;
    }
}