#include "pa2mm.h"
#include "src/hospital.h"
#include "src/simulador.h"
#include "src/struct.h"

#include "string.h"
#include <stdbool.h>

bool ignorar_pokemon(pokemon_t* p){
    p = p;
    return true;
}

/* No intenten esto en sus casas */
/* Ya vamos a ver como evitar esto en el TDA Lista */
struct{
    pokemon_t* pokemon[500];
    size_t cantidad;
} acumulados;

void resetear_acumulados(){
    acumulados.cantidad = 0;
}

bool acumular_pokemon(pokemon_t* p){
    acumulados.pokemon[acumulados.cantidad] = p;
    acumulados.cantidad++;
    return true;
}

bool acumular_pokemon_hasta_miltank(pokemon_t* p){
    acumulados.pokemon[acumulados.cantidad] = p;
    acumulados.cantidad++;
    return strcmp(pokemon_nombre(p), "miltank");
}

bool acumulados_en_orden_correcto(){
    if(acumulados.cantidad < 2)
        return true;
    pokemon_t* anterior = acumulados.pokemon[0];
    for(int i=1;i<acumulados.cantidad;i++){
        pokemon_t* actual =  acumulados.pokemon[i];
        if(strcmp(pokemon_nombre(anterior), pokemon_nombre(actual)) > 0)
            return false;
    }
    return true;
}

/* Pruebas */

void puedoCrearYDestruirUnHospital(){
    hospital_t* h=NULL;

    pa2m_afirmar((h=hospital_crear()), "Crear un hospital devuelve un hospital");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==0, "Un hospital se crea con cero entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==0, "Un hospital se crea con cero pokemon");

    pa2m_afirmar(hospital_a_cada_pokemon(h, ignorar_pokemon)==0, "Recorrer los pokemon resulta en 0 pokemon recorridos");

    hospital_destruir(h);
}

void dadoUnHospitalNULL_lasPuedoAplicarLasOperacionesDelHospitalSinProblema(){
    hospital_t* h=NULL;

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==0, "Un hospital NULL tiene cero entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==0, "Un hospital NULL tiene cero pokemon");

    pa2m_afirmar(hospital_a_cada_pokemon(h, ignorar_pokemon)==0, "Recorrer los pokemon de un hospital NULL resulta en 0 pokemon recorridos");

    hospital_destruir(h);
}

void dadoUnArchivoVacio_NoSeAgreganPokemonAlHospital(){
    hospital_t* h=hospital_crear();

    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/archivo_vacio.hospital"), "Puedo leer un archivo vacío");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==0, "Un hospital vacío tiene cero entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==0, "Un hospital vacío tiene tiene cero pokemon");

    pa2m_afirmar(hospital_a_cada_pokemon(h, ignorar_pokemon)==0, "Recorrer los pokemon resulta en 0 pokemon recorridos");

    hospital_destruir(h);
}

void dadoUnArchivoConUnEntrenador_SeAgregaElEntrenadorYSusPokemonAlHospital(){
    hospital_t* h=hospital_crear();

    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/un_entrenador.hospital"), "Puedo leer un archivo con un entrenador");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==1, "El hospital tiene 1 entrenador");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==3, "El hospital tiene 3 pokemon");

    resetear_acumulados();
    pa2m_afirmar(hospital_a_cada_pokemon(h, acumular_pokemon)==3, "Recorrer los pokemon resulta en 3 pokemon recorridos");
    pa2m_afirmar(acumulados_en_orden_correcto(), "Los pokemon se recorrieron en orden alfabetico");

    hospital_destruir(h);
}

void dadoUnArchivoConVariosEntrenadores_SeAgreganLosEntrenadoresYSusPokemonAlHospital(){
    hospital_t* h=hospital_crear();

    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital"), "Puedo leer un archivo con varios entrenadores");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==5, "El hospital tiene 5 entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==24, "El hospital tiene 24 pokemon");

    resetear_acumulados();
    pa2m_afirmar(hospital_a_cada_pokemon(h, acumular_pokemon)==24, "Recorrer los pokemon resulta en 24 pokemon recorridos");
    pa2m_afirmar(acumulados_en_orden_correcto(), "Los pokemon se recorrieron en orden alfabetico");

    hospital_destruir(h);
}


void dadosVariosArchivos_puedoAgregarlosTodosAlMismoHospital(){
    hospital_t* h=hospital_crear();

    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital"), "Puedo leer un archivo con varios entrenadores");
    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital"), "Puedo leer otro archivo con varios entrenadores");
    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital"), "Puedo leer un tercer archivo con varios entrenadores");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==15, "El hospital tiene 15 entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==72, "El hospital tiene 72 pokemon");

    resetear_acumulados();
    pa2m_afirmar(hospital_a_cada_pokemon(h, acumular_pokemon)==72, "Recorrer los pokemon resulta en 72 pokemon recorridos");
    pa2m_afirmar(acumulados_en_orden_correcto(), "Los pokemon se recorrieron en orden alfabetico");

    hospital_destruir(h);
}

void puedoCrearYDestruirUnSimulador(){
    EstadisticasSimulacion* estadisticas = calloc(1,sizeof(EstadisticasSimulacion));
    simulador_t* simulador = NULL;
    hospital_t* hospital = NULL;

    simulador = simulador_crear(hospital);
    pa2m_afirmar(simulador == NULL, "Al intentar crear un simulador de un hospital NULL, no puedo hacerlo");

    hospital = hospital_crear();

    hospital_leer_archivo(hospital, "ejemplos/archivo_vacio.hospital");
    simulador = simulador_crear(hospital);
    pa2m_afirmar(simulador, "Al intentar crear un simulador de un hospital vacío, puedo hacerlo");
    pa2m_afirmar(simulador_simular_evento(simulador, ObtenerEstadisticas, estadisticas) == ExitoSimulacion, "Puedo obtener las estadisticas del simulador recién creado\n");


    simulador_destruir(simulador);
    hospital_destruir(hospital);

    hospital = hospital_crear();
    hospital_leer_archivo(hospital, "ejemplos/varios_entrenadores.hospital");


    simulador = simulador_crear(hospital);
    pa2m_afirmar(simulador, "Al intentar crear un simulador de un hospital con entrenadores, puedo hacerlo");


    pa2m_afirmar(simulador_simular_evento(simulador, ObtenerEstadisticas, estadisticas) == ExitoSimulacion, "Puedo obtener las estadisticas de un simulador recién creado");

    pa2m_afirmar(estadisticas->pokemon_en_espera == 0, "El simulador tiene 0 (cero) pokemones en espera");
    pa2m_afirmar(estadisticas->entrenadores_atendidos == 0 && estadisticas->pokemon_atendidos == 0, "El simulador tiene 0 (cero) pokemones y 0 (cero) entrenadores atendidos");
    pa2m_afirmar(estadisticas->pokemon_totales == 24 && estadisticas->entrenadores_totales == 5, "El simulador la cantidad correcta de entrenadores y pokemones totales");

    

    simulador_destruir(simulador);
    hospital_destruir(hospital);
    free(estadisticas);
}

bool es_mismo_pokemon(pokemon_t poke1, pokemon_t poke2){
    return (poke1.nivel == poke2.nivel && strcmp(poke1.id_entrenador,poke2.id_entrenador) == 0 && strcmp(poke1.nombre, poke2.nombre) == 0);
}

void dadoUnSimuladorValido_puedoAniadirEntrenadores_SoloSiTengoDisponibles(){
    hospital_t* hospital = hospital_crear();
    simulador_t* simulador = simulador_crear(hospital);

    pa2m_afirmar(simulador_simular_evento(simulador, AtenderProximoEntrenador, NULL) == ErrorSimulacion, "Dado un simulador con un hospital vacío, al no tener entrenadores, si intento atender un nuevo entrenadores no puedo hacerlo\n");

    hospital_destruir(hospital); simulador_destruir(simulador);

    hospital = hospital_crear();
    simulador = simulador_crear(hospital);
    hospital_leer_archivo(hospital, "ejemplos/varios_entrenadores.hospital");

    pa2m_afirmar(simulador_simular_evento(simulador, AtenderProximoEntrenador, NULL) == ExitoSimulacion, "Dado el simulador que aún tiene entrenadores para atender, al atender un nuevo entrenador puedo hacerlo.")
    pa2m_afirmar(heap_tamanio(simulador->heap_pokemones) == 4, "Al atender un llamar un nuevo entrenador hay 4 pokemones para ser atendidos\n");
    
    pa2m_afirmar(simulador_simular_evento(simulador, AtenderProximoEntrenador, NULL) == ExitoSimulacion, "Dado el simulador que aún tiene entrenadores para atender, al atender un nuevo entrenador puedo hacerlo.")
    pa2m_afirmar(heap_tamanio(simulador->heap_pokemones) == 8, "Al atender un llamar un nuevo entrenador hay 8 pokemones para ser atendidos\n");

// 1;lucas;charizard;20;rampardos;10;torkal;43;duskull;85
// 2;valen;miltank;45;toxicroak;20;alcremie;65;shuckle;59


    pokemon_t pokemones[8] = {{10, false, NULL, "1", "rampardos"},{20, false, NULL, "2", "toxicroak"},
                              {20, false, NULL, "1", "charizard"},{43, false, NULL, "1", "torkal"},
                              {45, false, NULL, "2", "miltank"}, {59, false, NULL, "2", "shuckle"}, 
                              {65, false, NULL, "2", "alcremie"}, {85,false, NULL, "1", "duskull"}};

    for(int i = 0; i < 8; i++){
        pokemon_t* pokemon = heap_extraer_raiz(simulador->heap_pokemones);
        
        if(es_mismo_pokemon(*pokemon, pokemones[i])){
            printf("%s%s%s El proximo Pokemon es: %s con nivel %li y debería ser: %s con nivel %li.\n", VERDE, TILDE, BLANCO, pokemon->nombre, pokemon->nivel, pokemones[i].nombre, pokemones[i].nivel);
            __pa2m_cantidad_de_pruebas_corridas++;
            
        } else{
            printf("%s%s%s El proximo Pokemon es: %s con nivel %li y debería ser: %s con nivel %li.\n", ROJO, CRUZ, BLANCO, pokemon->nombre, pokemon->nivel, pokemones[i].nombre, pokemones[i].nivel);
            __pa2m_cantidad_de_pruebas_fallidas = 0;
        }
    }

    hospital_destruir(hospital); simulador_destruir(simulador);

}



int main(){

    pa2m_nuevo_grupo("Pruebas de  creación y destrucción");
    puedoCrearYDestruirUnHospital();

    pa2m_nuevo_grupo("Pruebas con NULL");
    dadoUnHospitalNULL_lasPuedoAplicarLasOperacionesDelHospitalSinProblema();

    pa2m_nuevo_grupo("Pruebas con un archivo vacío");
    dadoUnArchivoVacio_NoSeAgreganPokemonAlHospital();

    pa2m_nuevo_grupo("Pruebas con un archivo de un entrenador");
    dadoUnArchivoConUnEntrenador_SeAgregaElEntrenadorYSusPokemonAlHospital();

    pa2m_nuevo_grupo("Pruebas con un archivo de varios entrenadores");
    dadoUnArchivoConVariosEntrenadores_SeAgreganLosEntrenadoresYSusPokemonAlHospital();

    pa2m_nuevo_grupo("Pruebas con mas de un archivo");
    dadosVariosArchivos_puedoAgregarlosTodosAlMismoHospital();

    pa2m_nuevo_grupo("Pruebas de Creación y Destrucción del Simulador");
    puedoCrearYDestruirUnSimulador();

    pa2m_nuevo_grupo("Pruebas de Simulador Atender Proximo Entrenador");
    dadoUnSimuladorValido_puedoAniadirEntrenadores_SoloSiTengoDisponibles();

    return pa2m_mostrar_reporte();
}
