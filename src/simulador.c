#include "hospital.h"
#include "simulador.h"
#include "dificultades.h"
#include "lista.h"
#include "struct.h"
#include "heap.h"
#include "abb.h"
#include <string.h>

#define POSICION_NORMAL 1
#define TAMANIO_MINIMO 1
#define ERROR -1

bool cargar_dificultades(lista_t* lista_dificultades){
    DatosDificultad* copia_facil = copiar_dificultad((DatosDificultad*)&DIFICULTAD_FACIL);
    DatosDificultad* copia_normal = copiar_dificultad((DatosDificultad*)&DIFICULTAD_NORMAL);
    DatosDificultad* copia_dificil = copiar_dificultad((DatosDificultad*)&DIFICULTAD_DIFICIL);
    bool estado = true;

    if( !copia_facil || !copia_normal || !copia_dificil  ||
       !lista_insertar(lista_dificultades, copia_facil)  ||
       !lista_insertar(lista_dificultades, copia_normal) || 
       !lista_insertar(lista_dificultades, copia_dificil)){
           destruir_dificultad(copia_facil);
           destruir_dificultad(copia_normal);
           destruir_dificultad(copia_dificil);
           estado = false;
    }

    return estado;
}

/**
 * Crea un simulador para un hospital. El simulador toma control del hospital y
 * el mismo no debe ser modificado ni liberado por fuera del simulador.
 *
 * Devuelve el simulador creado o NULL en caso de error.
 */
simulador_t* simulador_crear(hospital_t* hospital){
    bool estado = true;
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
    if(!simulador->lista_dificultades){
        lista_iterador_destruir(simulador->iterador_entrenadores);
        heap_destruir(simulador->heap_pokemones);
        free(simulador);
    }

    estado = cargar_dificultades(simulador->lista_dificultades);
    if(estado == false){
        heap_destruir(simulador->heap_pokemones);
        lista_iterador_destruir(simulador->iterador_entrenadores);
        lista_destruir(simulador->lista_dificultades);
        free(simulador);
    }

    simulador->dificultad_actual = lista_elemento_en_posicion(simulador->lista_dificultades, POSICION_NORMAL); // Casinho do macaquinho
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

void atender_pokemon(paciente_t* paciente, heap_t* pokemones, unsigned* pokemones_en_espera){
    if(!paciente || !pokemones) return;

    pokemon_t* pokemon = heap_extraer_raiz(pokemones);
    if(pokemon)
        (*pokemones_en_espera)--;

    paciente->pokemon = pokemon;
    paciente->intentos = NINGUNO;
}

ResultadoSimulacion atender_proximo_entrenador(simulador_t* simulador){
    if(!simulador || hospital_cantidad_pokemon(simulador->hospital) == 0) return ErrorSimulacion;

    entrenador_t* entrenador = lista_iterador_elemento_actual(simulador->iterador_entrenadores);
    if(!entrenador || !entrenador->pokemones)
        return ErrorSimulacion;

    abb_con_cada_elemento(entrenador->pokemones, INORDEN, guardar_en_heap, simulador->heap_pokemones);
    simulador->estadisticas.pokemon_en_espera = (unsigned)heap_tamanio(simulador->heap_pokemones);
    lista_iterador_avanzar(simulador->iterador_entrenadores);

    if(simulador->paciente.pokemon == NULL)
        atender_pokemon(&simulador->paciente, simulador->heap_pokemones, &(simulador->estadisticas.pokemon_en_espera));

    simulador->estadisticas.pokemon_en_espera = (unsigned)heap_tamanio(simulador->heap_pokemones);
    simulador->estadisticas.entrenadores_atendidos++;
    return ExitoSimulacion;
}

bool es_id_valido(lista_t* lista_dificultades, int id){
    if(!lista_dificultades) return false;

    return (lista_tamanio(lista_dificultades) > id);
}

ResultadoSimulacion obtener_informacion_dificultad(simulador_t* simulador, InformacionDificultad* datos){
    if(!simulador || !datos ) return ErrorSimulacion;

    if(!es_id_valido(simulador->lista_dificultades, datos->id)){
        datos->id = -1;
        datos->nombre_dificultad = NULL;
        datos->en_uso = false;
        return ErrorSimulacion;
    }

    DatosDificultad* dificultad = lista_elemento_en_posicion(simulador->lista_dificultades, (size_t)datos->id);   

    datos->nombre_dificultad = dificultad->nombre;
    if(strcmp(dificultad->nombre, simulador->dificultad_actual->nombre) == 0){
        datos->en_uso = true;
    } else {
        datos->en_uso = false;
    }
    return ExitoSimulacion;
}

ResultadoSimulacion finalizar_simulacion(bool* finalizado){
    if(!finalizado || (*finalizado)) return ErrorSimulacion;

    (*finalizado) = true;
    return ExitoSimulacion;
}

bool dificultad_vacia(DatosDificultad* datos){
    return ((!datos) || (!datos->nombre) || (!datos->calcular_puntaje) || (!datos->verificacion_a_string) || (!datos->verificar_nivel));
}

bool dificultad_repetida(lista_t* lista_dificultades, DatosDificultad* datos){
    if(!lista_dificultades || !datos) return false;
    lista_iterador_t* iterador = lista_iterador_crear(lista_dificultades);
    DatosDificultad* dificultad = NULL;
    bool repetida = false;
    bool error = false;

    if(!iterador) return false;
    while(lista_iterador_tiene_siguiente(iterador) && !error && !repetida){
        dificultad = lista_iterador_elemento_actual(iterador);
        if(dificultad != NULL)
            repetida = strcmp(dificultad->nombre, datos->nombre) == 0 ? true : false;
        else
            error = true;
        lista_iterador_avanzar(iterador);
    }

    lista_iterador_destruir(iterador);
    return repetida;
}

bool es_dificultad_valida(lista_t* lista_dificultades, DatosDificultad* datos){
    return ((lista_dificultades) && (!dificultad_vacia(datos)) && (!dificultad_repetida(lista_dificultades, datos)));
}


ResultadoSimulacion agregar_dificultad(simulador_t* simulador, DatosDificultad* datos){
    if(!simulador || !datos || !es_dificultad_valida(simulador->lista_dificultades,datos)) return ErrorSimulacion;

    DatosDificultad* copia = copiar_dificultad(datos);
    if(!copia) return ErrorSimulacion;;

    ResultadoSimulacion resultado = (lista_insertar(simulador->lista_dificultades, copia) != NULL) ? ExitoSimulacion : ErrorSimulacion;
    if(resultado == ErrorSimulacion){
        destruir_dificultad(copia);
    }

    return resultado;
}

ResultadoSimulacion seleccionar_dificultad(simulador_t* simulador, int* id){
    if(!simulador || !id || !es_id_valido(simulador->lista_dificultades, *id)) return ErrorSimulacion;

    DatosDificultad* dificultad = lista_elemento_en_posicion(simulador->lista_dificultades, (size_t)*id);
    if(!dificultad) return ErrorSimulacion;

    simulador->dificultad_actual = dificultad;
    return ExitoSimulacion;
}

ResultadoSimulacion obtener_informacion_paciente(simulador_t* simulador, InformacionPokemon* datos){
    if(!simulador || !datos) return ErrorSimulacion;

    paciente_t paciente = simulador->paciente;

    if(paciente.pokemon == NULL){
        datos->nombre_entrenador = NULL;
        datos->nombre_pokemon = NULL;
    } else {
        datos->nombre_entrenador = paciente.pokemon->entrenador->nombre;
        datos->nombre_pokemon = paciente.pokemon->nombre;
    }

    return paciente.pokemon != NULL ? ExitoSimulacion : ErrorSimulacion;
}

ResultadoSimulacion tratar_pokemon(DatosDificultad* dificultad, heap_t* pokemones, paciente_t* paciente, Intento* intento, EstadisticasSimulacion* estadisticas){
    if(!dificultad || !paciente || !paciente->pokemon || !intento || !estadisticas) return ErrorSimulacion;


    paciente->intentos++;
    int puntaje = dificultad->verificar_nivel((unsigned)paciente->pokemon->nivel, intento->nivel_adivinado);

    if(puntaje == 0){
        estadisticas->puntos += (unsigned)dificultad->calcular_puntaje(paciente->intentos);
        atender_pokemon(paciente, pokemones, &estadisticas->pokemon_en_espera);
        estadisticas->pokemon_atendidos++;
    }
    
    intento->es_correcto = puntaje == 0 ? true : false;
    intento->resultado_string = dificultad->verificacion_a_string(puntaje);
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

    simulador->estadisticas.cantidad_eventos_simulados++;
    switch(evento){
        case ObtenerEstadisticas:
            resultado = obtener_estadisticas(simulador, datos);
            break;
        case AtenderProximoEntrenador:
            resultado = atender_proximo_entrenador(simulador); //?
            break;
        case ObtenerInformacionPokemonEnTratamiento:
            resultado = obtener_informacion_paciente(simulador, datos);
            break;
        case AdivinarNivelPokemon:
            resultado = tratar_pokemon(simulador->dificultad_actual, simulador->heap_pokemones, &simulador->paciente, datos, &simulador->estadisticas);
            break;
        case SeleccionarDificultad: 
            resultado = seleccionar_dificultad(simulador, datos);
            break; //Debo incluirlo en el struct simulador
        case ObtenerInformacionDificultad:
            resultado = obtener_informacion_dificultad(simulador, datos);
            break;
        case AgregarDificultad:
            resultado = agregar_dificultad(simulador, datos);
            break;
        case FinalizarSimulacion:
            resultado = finalizar_simulacion(&(simulador->finalizado));
            break;
    }

    return resultado;
}


/**
 * Destruye el simulador y libera la memoria asociada (incluida la del hospital).
 */
void simulador_destruir(simulador_t* simulador){
    if(!simulador || !simulador->hospital) return;

    lista_iterador_destruir(simulador->iterador_entrenadores);
    heap_destruir(simulador->heap_pokemones);
    lista_destruir_todo(simulador->lista_dificultades, destruir_dificultad);
    hospital_destruir(simulador->hospital);
    free(simulador);
}