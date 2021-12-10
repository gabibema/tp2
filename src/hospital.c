#include <stdio.h>
#include <string.h>
#include "hospital.h"
#include "string_aux.h"
#include "lista.h"
#include "split.h"
#include "heap.h"
#include "hash.h"
#include "abb.h"

#define FIN '\0'
#define TAMANIO_MINIMO 3
#define SEPARADOR_ARCHIVO ';' 
#define SALTO_DE_LINEA '\n'

const size_t NINGUNO = 0;
const size_t BLOQUE = sizeof(char)*100;
const size_t POSICION_ID = 0;
const size_t POSICION_NOMBRE = 1;
const size_t POSICION_PRIMER_POKEMON = 2;


struct _entrenador_t{
    char* id;
    char* nombre;
    size_t cantidad_pokemones;
    size_t cantidad_atendidos;
};

struct _hospital_pkm_t{
    hash_t* hash_entrenadores;
    size_t cantidad_entrenadores;

    abb_t* pokemones; //totales del hospital
    heap_t* heap_pokemones; //Heap minimal de nivel
    size_t cantidad_pokemon;
};

struct _pkm_t{
    char* id_entrenador;
    bool atendido;
    char* nombre;
    size_t nivel;
};

//CADA ID ES UNICO

void destructor_entrenadores(void* e){
    if(!e)
        return;

    entrenador_t* entrenador = e;
    free(entrenador->nombre);
    free(entrenador->id);
    free(entrenador);
}


void destructor_pokemon(void* p){
    if(!p)
        return;

    pokemon_t* pokemon = p;
    free(pokemon->nombre);
}


int comparador_nombres(void* poke_1, void* poke_2){
    if(!poke_1 || !poke_2) return 0;

    pokemon_t* pokemon_1 = poke_1;
    pokemon_t* pokemon_2 = poke_2;

    return strcmp(pokemon_1->nombre, pokemon_2->nombre);
}

int comparador_nivel(void* poke_1, void* poke_2){
    if(!poke_1 || !poke_2) return 0;

    pokemon_t* pokemon_1 = poke_1;
    pokemon_t* pokemon_2 = poke_2;

    return (int)(pokemon_1->nivel - pokemon_2->nivel);
}

hospital_t* hospital_crear(){
    hospital_t* hospital = calloc(1, sizeof(hospital_t));
    if(!hospital)
        return NULL;

    hospital->hash_entrenadores = hash_crear(destructor_entrenadores, TAMANIO_MINIMO);
    if(!hospital->hash_entrenadores){
        free(hospital);
        return NULL;
    }

    hospital->pokemones = abb_crear(comparador_nombres);
    if(!hospital->pokemones){
        hash_destruir(hospital->hash_entrenadores);
        free(hospital);
    }

    hospital->heap_pokemones = heap_crear(comparador_nivel, 1);

    return hospital;
}

void destructor_pokemones(void* poke_1){
    if(poke_1 == NULL) return;

    pokemon_t* pokemon = poke_1;
    free(pokemon->nombre);
    free(pokemon);
}


/*
 * PRE: -
 * POS: Devuelve un string desde el caracter ingreso hasta el siguiente salto de línea del archivo
 *      Devuelve NULL si el archivo es NULL
 *      Devuelve NULL si no hay espacio para el string
 */
char* linea_leida (FILE* archivo, char caracter){
    char* datos = NULL;
    char* datos_aux = NULL;
    size_t multiplicador = 0;
    size_t i = 0;

    if(archivo == NULL || caracter == SALTO_DE_LINEA || caracter == EOF) return NULL;
    
    while((caracter != SALTO_DE_LINEA) && (caracter != EOF)){
        if((i + 1) >= (multiplicador*BLOQUE)){ //Si no tengo espacio para asignar el siguiente caracter
            
            datos_aux = realloc(datos, BLOQUE*(multiplicador + 1));
            multiplicador++;
            
            if(datos_aux == NULL){
                free(datos);
                return NULL;
            }
        }

        datos = datos_aux;
        datos[i] = caracter;
        caracter = (char)fgetc(archivo);
        i++;

    }

    datos[i] = FIN;
    datos_aux = realloc(datos, sizeof(char) * (strlen(datos) + 1)); //Reduzco la memoria para no tener exceso
    if(datos_aux != NULL)
        datos = datos_aux;

    return datos;
}

pokemon_t* crear_pokemon(char* nombre, char* nivel, char* id){
    if(!nombre || !nivel) return NULL;    

    pokemon_t* pokemon = malloc(sizeof(pokemon_t));
    if(!pokemon) return NULL;

    pokemon->nombre = nombre;
    pokemon->nivel = (size_t)atoi(nivel);
    pokemon->id_entrenador = id;

    return pokemon;
}

entrenador_t* crear_entrenador(char* id, char* nombre){
    if(!nombre) return NULL;

    entrenador_t* entrenador = malloc(sizeof(entrenador_t));
    if(!entrenador) return NULL;

    entrenador->id = id;
    entrenador->nombre = nombre;

    return entrenador;
}


bool mostrar_pokes(void* poke, void* aux){
    if(!poke) return false;
    pokemon_t* pokemon = poke;


    printf(" Atendido -> Nivel: %li Nombre: %s\n", pokemon->nivel, pokemon->nombre);
    return true;

}


/*
 * PRE: El puntero hospital sea NULL o esté inicializado
 *      El entrenador sea NULL o válido (con el formato del archivo)
 * POS: Agrega los pokemones del entrenador al hospital y devuelve true
 *      Si no hay memoria disponible para agregar los pokemones devuelve false
 *      Si el puntero al hospital es NULL devuelve false
 *      Si datos_entrenador es NULL devuelve false
 */
bool hospital_agregar_pokemones(hospital_t* hospital, size_t* cantidad_pokemones, char** datos_pokemon, char* id_entrenador){
    size_t i = 0;
    bool error = false;

    if(datos_pokemon == NULL) return NULL;

    while(datos_pokemon[i] != NULL && !error){
        pokemon_t* pokemon = crear_pokemon(datos_pokemon[i], datos_pokemon[i + 1], id_entrenador);
        if(pokemon == NULL)
            error = true;
        else{
            abb_insertar(hospital->pokemones, pokemon);
            heap_insertar(hospital->heap_pokemones, pokemon);
            free(datos_pokemon[i + 1]);
            (*cantidad_pokemones)++;    
        }

        i+=2;
    }

    return !error;
}

#define EXITO 0

/*
 * PRE: -
 * POS: Agrega un nuevo elemento entrenador_t al hospital y devuelve true
 *      Si no hay memoria disponible para agregar el entrenador al vector devuelve false
 *      Si el puntero al hospital es NULL devuelve false
 *      Si el string id es NULL devuelve false
 *      Si el string nombre es NULL devuelve false
 */
bool hospital_agregar_entrenador(hospital_t* hospital, char** datos){
    if(hospital == NULL || !datos) return false;
    
    bool agregado = true;
    entrenador_t* entrenador = crear_entrenador(datos[POSICION_ID], datos[POSICION_NOMBRE]);
    if(!entrenador) return false;

    if(hash_insertar(hospital->hash_entrenadores,datos [POSICION_ID], entrenador) == EXITO)
        hospital->cantidad_entrenadores++;
    else
        agregado = false;

    if(!agregado){
        free(entrenador);
        return false;
    }

    agregado = hospital_agregar_pokemones(hospital, &hospital->cantidad_pokemon, &datos[POSICION_PRIMER_POKEMON], datos[POSICION_ID]);

    if(!agregado){
        hash_quitar(hospital->hash_entrenadores, datos[POSICION_ID]);
    }

    //printf("Entrenador: %s\n", entrenador->nombre);
    //abb_con_cada_elemento(entrenador->pokemones, INORDEN, mostrar_pokes, NULL);
    return agregado;
}

/*
 * PRE: El vector de pokemones no sea NULL e inicializado
 *      Cantidad mayor o igual a 0
 * POS: Ordena el vector de pokémon por nombre ascendentemente
 */
void ordenar_pokemones(pokemon_t* pokemones, size_t cantidad_pokemones){
    bool ordenado = false;
    pokemon_t aux;
    int cmp = 0;
    int i = 0;

    while( i < cantidad_pokemones && !ordenado){
        ordenado = true;
        for(int j = 0; j < (int)cantidad_pokemones - i - 1; j++){
            cmp = strcmp(pokemones[j].nombre, pokemones[j + 1].nombre);
            if(cmp > 0){
                aux = pokemones[j +1];
                pokemones[j + 1] = pokemones[j];
                pokemones[j] = aux;
                ordenado = false;
            }
        }
        i++;
    }
}


bool hospital_leer_archivo(hospital_t* hospital, const char* nombre_archivo){
    FILE* entrenadores = fopen(nombre_archivo, "r");
    char** entrenador = NULL;
    char caracter = '-';
    char* datos = NULL;
    bool estado = true;

    if(entrenadores == NULL) return false;
    caracter = (char)fgetc(entrenadores);
    if(caracter == EOF){
        fclose(entrenadores);
        return true;
    }

    while(caracter != EOF && estado){
        datos = linea_leida(entrenadores, caracter);
        caracter = (char)fgetc(entrenadores);

        if(datos == NULL){
            estado = false;
        } else {
            entrenador = split(datos, SEPARADOR_ARCHIVO); //Chequear
            estado = hospital_agregar_entrenador(hospital, entrenador); //Optimizar.
            if(!estado){
                printf("Liberandoo");
                liberar_substrings(entrenador);

            }
            free(entrenador);
            free(datos);
        }
    }


    // //ordenar_pokemones(pokemones, hospital->cantidad_pokemon)
    // // if(estado && !insertar_pokemones(hospital->lista_pokemones, pokemones, hospital->cantidad_pokemon)){
    // //     printf("Liberandoo");
    // //     destruir_pokemones(pokemones, hospital->cantidad_pokemon);
    // //     lista_destruir_todo(hospital->lista_entrenadores, destructor_entrenadores);
    // //     lista_destruir_todo(hospital->lista_pokemones, destructor_pokemon);
    // //     estado = false;
    // // }

    fclose(entrenadores);
    return estado;
}


size_t hospital_cantidad_pokemon(hospital_t* hospital){
    size_t cantidad = NINGUNO;

    if(hospital)
        cantidad = hospital->cantidad_pokemon;

    return cantidad;
}


size_t hospital_cantidad_entrenadores(hospital_t* hospital){
    size_t cantidad = NINGUNO;
    
    if(hospital){
        cantidad = hospital->cantidad_entrenadores;
    }

    return cantidad;
}

bool aplicar_funcion_pokemon(void* poke_1, void* func){
    if(!poke_1 || !func) return false;
    pokemon_t* pokemon = poke_1;
    bool (*funcion)(pokemon_t* ) = func;

    return funcion(pokemon);
}


size_t hospital_a_cada_pokemon(hospital_t* hospital, bool (*funcion)(pokemon_t* p)){
    if(hospital  == NULL || funcion == NULL) 
        return NINGUNO;

    return abb_con_cada_elemento(hospital->pokemones, INORDEN, aplicar_funcion_pokemon, funcion);
}



//Hacer pruebas de destruir todo

void hospital_destruir(hospital_t* hospital){
    if(!hospital)
        return;

    hash_destruir(hospital->hash_entrenadores);
    abb_destruir_todo(hospital->pokemones, destructor_pokemones);
    heap_destruir(hospital->heap_pokemones);
    free(hospital);
}


size_t pokemon_nivel(pokemon_t* pokemon){
    size_t nivel = NINGUNO;

    if(pokemon)
        nivel = pokemon->nivel;

    return nivel;
}


const char* pokemon_nombre(pokemon_t* pokemon){
    char* nombre = NULL;

    if(pokemon)
        nombre = pokemon->nombre;

    return nombre;
}