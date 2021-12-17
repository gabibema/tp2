#include "simulador.h"
#include "string.h"
#include "struct.h"
#include <stdlib.h>
#include <time.h>

int comparador_nivel(void* poke_1, void* poke_2){
    if(!poke_1 || !poke_2) return 0;

    pokemon_t* pokemon_1 = poke_1;
    pokemon_t* pokemon_2 = poke_2;

    return (int)(pokemon_nivel(pokemon_1) - pokemon_nivel(pokemon_2));
}


unsigned calcular_puntaje(unsigned intentos){
    return 100/intentos;
}

#define MENSAJE_ACERTADO "Acertaste"

#define MENSAJE_FACIL_1 "Muy cerca"
#define MENSAJE_FACIL_2 "Cerca"
#define MENSAJE_FACIL_3 "Lejos"
#define MENSAJE_FACIL_4 "Muy lejos"
#define MENSAJE_NORMAL_1 "Caliente"
#define MENSAJE_NORMAL_2 "Frío"
#define MENSAJE_DIFICIL "No es :("

int verificar_nivel(unsigned nivel_adivinado, unsigned nivel_pokemon){
    return (int)nivel_pokemon - (int)nivel_adivinado;
}

const char* verificacion_facil(int resultado){
    char* mensaje = NULL;

    if (resultado <= -50){
        mensaje = MENSAJE_FACIL_1;
    } else if (resultado <= -25){
        mensaje = MENSAJE_FACIL_2;
    } else if(resultado == 0){
        mensaje = MENSAJE_ACERTADO;
    } else if(resultado <= 25){
        mensaje = MENSAJE_FACIL_3;
    } else {
        mensaje = MENSAJE_FACIL_4;
    }

    return mensaje;
}

const char* verificacion_normal(int resultado){
    char* mensaje = NULL;

    if(resultado == 0){
        mensaje = MENSAJE_ACERTADO;
    } else if ( -25 <= resultado && resultado <= 25){
        mensaje = MENSAJE_NORMAL_1;
    } else {
        mensaje = MENSAJE_NORMAL_2;
    }

    return mensaje;
}




const char* verificacion_dificil(int resultado){
    char* mensaje = NULL;

    if(resultado == 0){
        mensaje = MENSAJE_ACERTADO;
    } else{
        mensaje = MENSAJE_DIFICIL;
    }

    return mensaje;
}

char string[60];

const char* verificacion_random(int resultado){
    srand((unsigned)time(NULL));
    int nivel = rand() % (resultado + 1);

    if(resultado == 0){
        strcpy(string, "☘ La fortuna te acompaña ☘");
    } else if(resultado > 0) {
        sprintf(string, " Quizás la fortuna te sonría con %i niveles menos", nivel);
    } else {
        sprintf(string, " Quizás la fortuna te sonría con %i niveles más", nivel);
    }

    return (const char*)string;
}


const char* verificacion_tutorial(int resultado){
    if(resultado == 0){
        strcpy(string, " Continuemos con el siguiente!");
    } else if(resultado > 0) {
        sprintf(string, " Probá con %i niveles más abajo!", resultado);
    } else {
        sprintf(string, " Probá con %i niveles más arriba!", resultado);
    }

    return (const char *)string;
}

void destruir_dificultad(void* dif){
    DatosDificultad* dificultad = dif;
    if(!dif) return;

    free((char*)dificultad->nombre);
    free(dificultad);
}


DatosDificultad* copiar_dificultad(DatosDificultad* dificultad){
    if(!dificultad) return NULL;

    DatosDificultad* copia = malloc(sizeof(DatosDificultad));
    if(!copia) return NULL;

    copia->nombre = malloc(strlen(dificultad->nombre) + 1);
    if(!copia->nombre){
        free(copia);
        return NULL;
    }

    strcpy((char*)copia->nombre, dificultad->nombre);
    copia->calcular_puntaje = dificultad->calcular_puntaje;
    copia->verificacion_a_string = dificultad->verificacion_a_string;
    copia->verificar_nivel = dificultad->verificar_nivel;
    return copia;
}
