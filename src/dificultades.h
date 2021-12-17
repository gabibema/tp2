#ifndef DIFICULTAD_H_
#define DIFICULTAD_H_

#include "simulador.h"

#define FACIL "Fácil"
#define NORMAL "Normal"
#define DIFICIL "Dificil"
#define TUTORIAL "Tutorial"
#define RANDOM "Random"

int comparador_nivel(void* poke_1, void* poke_2);
unsigned calcular_puntaje(unsigned intentos);
int verificar_nivel(unsigned nivel_adivinado, unsigned nivel_pokemon);

const char* verificacion_facil(int resultado);
const char* verificacion_normal(int resultado);
const char* verificacion_dificil(int resultado);
const char* verificacion_tutorial(int resultado);
const char* verificacion_random(int resultado);

DatosDificultad* copiar_dificultad(DatosDificultad* dificultad);
void destruir_dificultad(void* dif);

static const DatosDificultad DIFICULTAD_FACIL = {FACIL, calcular_puntaje, verificar_nivel, verificacion_facil};
static const DatosDificultad DIFICULTAD_NORMAL = {NORMAL,calcular_puntaje, verificar_nivel, verificacion_normal};
static const DatosDificultad DIFICULTAD_DIFICIL = {DIFICIL,calcular_puntaje, verificar_nivel, verificacion_dificil};
static const DatosDificultad DIFICULTAD_TUTORIAL = {TUTORIAL, calcular_puntaje, verificar_nivel, verificacion_tutorial};
static const DatosDificultad DIFICULTAD_RANDOM = {RANDOM, calcular_puntaje, verificar_nivel, verificacion_random};

#endif // DIFICULTAD_H_

