#include "hospital.h"
#include "simulador.h"
#include "lista.h"
#include "struct.h"
#include "heap.h"
#include "abb.h"

#define TAMANIO_MINIMO 1
#define ERROR -1

struct _simulador_t{
    heap_t* pokemon_en_espera;
    lista_iterador_t* entrenadores_en_espera; //Iterador externo en el cual el elemento actual es el entrenador que se deberá atender
    hospital_t* hospital;

    unsigned entrenadores_atendidos;
    unsigned pokemones_atendidos;
    unsigned pokemones_en_espera;
    unsigned puntos;
    unsigned cantidad_eventos_simulados;
};


int comparador_nivel(void* poke_1, void* poke_2){
    if(!poke_1 || !poke_2) return 0;

    pokemon_t* pokemon_1 = poke_1;
    pokemon_t* pokemon_2 = poke_2;

    return (int)(pokemon_nivel(pokemon_1) - pokemon_nivel(pokemon_2));
}

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

    simulador->pokemon_en_espera = heap_crear(comparador_nivel, TAMANIO_MINIMO); //Creo un heap de tamaño 
    if(!simulador->pokemon_en_espera){
        free(simulador);
        return NULL;
    }

    simulador->entrenadores_en_espera = lista_iterador_crear(hospital->lista_entrenadores);
    if(!simulador->entrenadores_en_espera){
        heap_destruir(simulador->pokemon_en_espera);
        free(simulador);
        return NULL;
    }

    return simulador;
}

ResultadoSimulacion obtener_estadisticas(simulador_t *simulador, void* datos){
    if(!simulador || !simulador->hospital || !datos) return ErrorSimulacion;

    EstadisticasSimulacion* estadisticas = datos;

    estadisticas->cantidad_eventos_simulados = simulador->cantidad_eventos_simulados;
    estadisticas->entrenadores_atendidos = simulador->entrenadores_atendidos;
    estadisticas->entrenadores_totales = (unsigned)hospital_cantidad_entrenadores(simulador->hospital);
    estadisticas->pokemon_atendidos = simulador->pokemones_atendidos;
    estadisticas->pokemon_totales = (unsigned)hospital_cantidad_pokemon(simulador->hospital);
    estadisticas->puntos = simulador->puntos;

    simulador->cantidad_eventos_simulados++;

    return ExitoSimulacion;
}


bool guardar_en_heap(void* poke, void* h){
    if(!poke || !h) return false;

    pokemon_t* pokemon = poke;
    heap_t* heap = h;

    if(heap_insertar(heap, pokemon) == ERROR) return false;

    return true;
}

ResultadoSimulacion atender_proximo_entrenador(simulador_t* simulador){
    if(!simulador) return ErrorSimulacion;

    entrenador_t* entrenador = lista_iterador_elemento_actual(simulador->entrenadores_en_espera);
    if(!entrenador){
        //ERROR:
        return ErrorSimulacion;
    }

    abb_con_cada_elemento(entrenador->pokemones, INORDEN, guardar_en_heap, simulador->pokemon_en_espera);
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
    
    if(!simulador) return ErrorSimulacion;

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
            break;
        case SeleccionarDificultad: //Debo incluirlo en el struct simulador
            break;
        case ObtenerInformacionDificultad:
            break;
        case AgregarDificultad:
            break;
        case FinalizarSimulacion:
            break;

    }

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

    lista_iterador_destruir(simulador->entrenadores_en_espera);
    heap_destruir(simulador->pokemon_en_espera);
    free(simulador);
}