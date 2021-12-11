#ifndef __STRUCT_H__
#define __STRUCT_H__

#include "hospital.h"
#include "lista.h"
#include "abb.h"

struct _entrenador_t{
    char* id;
    char* nombre;
    abb_t* pokemones; //De este entrenador
    size_t cantidad_pokemones;
    size_t cantidad_atendidos;
};

struct _hospital_pkm_t{
    lista_t* lista_entrenadores;
    size_t cantidad_entrenadores;

    abb_t* pokemones; //totales del hospital
    size_t cantidad_pokemon;
};

struct _pkm_t{
    size_t nivel;
    bool atendido;
    entrenador_t* entrenador;
    char* id_entrenador;
    char* nombre;
};



#endif /* __STRUCT_H__ */
