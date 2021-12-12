#include "hospital.h"
#include "simulador.h"
#include "lista.h"
#include "struct.h"
#include "heap.h"
#include "abb.h"
#include <string.h>

#define TAMANIO_MINIMO 1
#define ERROR -1

int comparador_nivel(void* poke_1, void* poke_2){
    if(!poke_1 || !poke_2) return 0;

    pokemon_t* pokemon_1 = poke_1;
    pokemon_t* pokemon_2 = poke_2;

    return (int)(pokemon_nivel(pokemon_1) - pokemon_nivel(pokemon_2));
}


unsigned calcular_puntaje(unsigned intentos){
    return 100/intentos;
}

int verificar_nivel(unsigned nivel_adivinado, unsigned nivel_pokemon){
    return (int)nivel_pokemon - (int)nivel_adivinado;
}
#define MENSAJE_ACERTADO "Acertaste"

#define MENSAJE_FACIL_1 "Muy cerca"
#define MENSAJE_FACIL_2 "Cerca"
#define MENSAJE_FACIL_3 "Lejos"
#define MENSAJE_FACIL_4 "Muy lejos"

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


#define MENSAJE_NORMAL_1 "Caliente"
#define MENSAJE_NORMAL_2 "Frío"



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

#define MENSAJE_DIFICIL "No es :("

const char* verificacion_dificil(int resultado){
    char* mensaje = NULL;

    if(resultado == 0){
        mensaje = MENSAJE_ACERTADO;
    } else{
        mensaje = MENSAJE_DIFICIL;
    }

    return mensaje;
}

#define FACIL "Facil"
#define NORMAL "Normal"
#define DIFICIL "Dificil"

const DatosDificultad DIFICULTAD_FACIL = {FACIL, calcular_puntaje, verificar_nivel, verificacion_facil};
const DatosDificultad DIFICULTAD_NORMAL = {NORMAL,calcular_puntaje, verificar_nivel, verificacion_normal};
const DatosDificultad DIFICULTAD_DIFICIL = {DIFICIL,calcular_puntaje, verificar_nivel, verificacion_dificil};

// const char* nombre;
// unsigned (*calcular_puntaje)(unsigned cantidad_intentos);
// int (*verificar_nivel)(unsigned nivel_adivinado, unsigned nivel_pokemon);
// const char* (*verificacion_a_string)(int resultado_verificacion);



/**
 * Crea un simulador para un hospital. El simulador toma control del hospital y
 * el mismo no debe ser modificado ni liberado por fuera del simulador.
 *
 * Devuelve el simulador creado o NULL en caso de error.
 */
simulador_t* simulador_crear(hospital_t* hospital){
    if(!hospital) return NULL;

    simulador_t* simulador = calloc(1, sizeof(simulador_t));
    if(!simulador) return NULL;

    simulador->hospital = hospital;
    
    simulador->heap_pokemones = heap_crear(comparador_nivel, TAMANIO_MINIMO); //Creo un heap de tamaño 
    if(!simulador->heap_pokemones){
        free(simulador);
        return NULL;
    }

    simulador->iterador_entrenadores = lista_iterador_crear(hospital->lista_entrenadores);
    if(!simulador->iterador_entrenadores){
        heap_destruir(simulador->heap_pokemones);
        free(simulador);
        return NULL;
    }

    simulador->lista_dificultades = lista_crear();
    if(!lista_insertar(simulador->lista_dificultades, (void*)&DIFICULTAD_FACIL) ||
       !lista_insertar(simulador->lista_dificultades, (void*)&DIFICULTAD_NORMAL) || 
       !lista_insertar(simulador->lista_dificultades, (void*)&DIFICULTAD_DIFICIL)){
           lista_iterador_destruir(simulador->iterador_entrenadores);
           lista_destruir(simulador->lista_dificultades);
           heap_destruir(simulador->heap_pokemones);
           free(simulador);
    }

    simulador->nombre_dificultad_actual = NORMAL; // Casinho do macaquinho
    simulador->estadisticas.entrenadores_totales = (unsigned)hospital_cantidad_entrenadores(hospital);
    simulador->estadisticas.pokemon_totales = (unsigned)hospital_cantidad_pokemon(hospital);
    return simulador;
}

ResultadoSimulacion obtener_estadisticas(simulador_t *simulador, void* datos){
    if(!simulador || !simulador->hospital || !datos) return ErrorSimulacion;

    EstadisticasSimulacion* estadisticas = datos;

    estadisticas->cantidad_eventos_simulados = simulador->estadisticas.cantidad_eventos_simulados;
    estadisticas->entrenadores_atendidos = simulador->estadisticas.entrenadores_atendidos;
    estadisticas->entrenadores_totales = simulador->estadisticas.entrenadores_totales;
    estadisticas->pokemon_atendidos = simulador->estadisticas.pokemon_atendidos;
    estadisticas->pokemon_totales = simulador->estadisticas.pokemon_totales;
    estadisticas->pokemon_en_espera = simulador->estadisticas.pokemon_en_espera;
    estadisticas->puntos = simulador->estadisticas.puntos;

    return ExitoSimulacion;
}


bool guardar_en_heap(void* poke, void* h){
    if(!poke || !h) return false;

    pokemon_t* pokemon = poke;
    heap_t* heap = h;

    if(heap_insertar(heap, pokemon) == ERROR) return false;

    return true;
}

#define NINGUNO 0

void atender_pokemon(paciente_t* paciente, heap_t* pokemones){
    if(!paciente || !pokemones) return;

    pokemon_t* pokemon = heap_extraer_raiz(pokemones);
    if(!pokemon) return;

    paciente->pokemon = pokemon;
    paciente->intentos = NINGUNO;
}

ResultadoSimulacion atender_proximo_entrenador(simulador_t* simulador){
    if(!simulador || hospital_cantidad_pokemon(simulador->hospital) == 0) return ErrorSimulacion;

    entrenador_t* entrenador = lista_iterador_elemento_actual(simulador->iterador_entrenadores);
    if(!entrenador || !entrenador->pokemones)
        return ErrorSimulacion;

    abb_con_cada_elemento(entrenador->pokemones, INORDEN, guardar_en_heap, simulador->heap_pokemones);
    lista_iterador_avanzar(simulador->iterador_entrenadores);

    // if(simulador->paciente.pokemon == NULL)
    //     atender_pokemon(&(simulador->paciente), simulador->heap_pokemones);

    simulador->estadisticas.pokemon_en_espera = (unsigned)heap_tamanio(simulador->heap_pokemones);
    simulador->estadisticas.entrenadores_atendidos++;
    return ExitoSimulacion;
}

ResultadoSimulacion obtener_informacion_dificultad(simulador_t* simulador, InformacionDificultad* datos){
    if(!simulador || !datos) return ErrorSimulacion;

    DatosDificultad* dificultad = lista_elemento_en_posicion(simulador->lista_dificultades, (size_t)datos->id);   

    datos->nombre_dificultad = dificultad->nombre;
    if(strcmp(dificultad->nombre, simulador->nombre_dificultad_actual) == 0){
        datos->en_uso = true;
    }
    return ExitoSimulacion;
}

ResultadoSimulacion finalizar_simulacion(bool* finalizado){
    if(!finalizado || (*finalizado)) return ErrorSimulacion;

    (*finalizado) = true;
    return ExitoSimulacion;
}

/**
 * Simula un evento. Cada evento puede recibir un puntero a un dato que depende
 * de cada evento. En caso de no necesitarse, se debe pasar NULL.
 *
 * Ejecuta el evento correspondiente (ver la documentación) y en caso de ser
 * necesario modifica la información apuntada por dato.
 *
 * Devuelve ErrorSimulacion si no existe el simulador o existe algún error en el
 * manejo de memoria.
 *
 * Devuelve ExitoSimulacion o ErrorSimulacion según corresponda a cada evento.
 */
ResultadoSimulacion simulador_simular_evento(simulador_t* simulador, EventoSimulacion evento, void* datos){
    ResultadoSimulacion resultado = ErrorSimulacion;
    
    if(!simulador || simulador->finalizado) return ErrorSimulacion;

    switch(evento){
        case ObtenerEstadisticas:
            resultado = obtener_estadisticas(simulador, datos);
            break;
        case AtenderProximoEntrenador:
            resultado = atender_proximo_entrenador(simulador); //?
            break;
        case ObtenerInformacionPokemonEnTratamiento:
            break;
        case AdivinarNivelPokemon:
            //resultado = adivinar_nivel_Pokemon(simulador);
            break;
        case SeleccionarDificultad: 
            break; //Debo incluirlo en el struct simulador
        case ObtenerInformacionDificultad:
            resultado = obtener_informacion_dificultad(simulador, datos);
            break;
        case AgregarDificultad:
            break;
        case FinalizarSimulacion:
            resultado = finalizar_simulacion(&(simulador->finalizado));
            break;
    }

    simulador->estadisticas.cantidad_eventos_simulados++;
    return resultado;
    // ObtenerEstadisticas,

    // AtenderProximoEntrenador,

    // ObtenerInformacionPokemonEnTratamiento,
    // AdivinarNivelPokemon,

    // AgregarDificultad,
    // SeleccionarDificultad,
    // ObtenerInformacionDificultad,

    // FinalizarSimulacion
}



/**
 * Destruye el simulador y libera la memoria asociada (incluida la del hospital).
 */
void simulador_destruir(simulador_t* simulador){
    if(!simulador || !simulador->hospital) return;

    lista_iterador_destruir(simulador->iterador_entrenadores);
    heap_destruir(simulador->heap_pokemones);
    lista_destruir(simulador->lista_dificultades);
    free(simulador);
}