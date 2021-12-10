#include "cola.h"
#include "lista.h"
#include <stdio.h>

#define PRIMER_POSICION 0
#define NINGUNO 0

struct _cola_t{
    lista_t* lista;
};

/**
 * Crea una cola vacía y la devuelve.
 *
 * En caso de error devuelve NULL
 */
cola_t* cola_crear(){
    cola_t* cola = calloc(1, sizeof(cola_t));

    if(cola == NULL){
        perror("  ERROR: No hay memoria disponible para crear la cola");
        return NULL;
    }

    cola->lista = calloc(1, sizeof(lista_t));

    if(cola->lista == NULL){
        perror("  ERROR: No hay memoria disponible para crear la cola");
        free(cola);
        return NULL;
    }
    
    return cola;
}


/**
 * Encola un elemento en la cola.
 *
 * Devuelve la cola o NULL en caso de error.
 */
cola_t* cola_encolar(cola_t* cola, void* elemento){
    if(cola == NULL){
        perror("  ERROR: La cola enviada es NULL");
        return NULL;
    }

    lista_t* lista_aux = NULL;
    lista_aux = lista_insertar(cola->lista, elemento);

    if(lista_aux == NULL){
        perror("  ERROR No se pudo encolar el elemento");
        return NULL;
    }

    cola->lista = lista_aux;

    return cola;
}


/**
 * Desencola un elemento de la cola y lo devuelve.
 *
 * Devuelve NULL en caso de error.
 */
void* cola_desencolar(cola_t* cola){
    if(cola == NULL){
        perror("  ERROR: La cola enviada es NULL");
        return NULL;
    }

    return lista_quitar_de_posicion(cola->lista, PRIMER_POSICION);
}


/**
 * Devuelve el elemento en el frente de la cola o NULL en caso de que no exista.
 */
void* cola_frente(cola_t* cola){
    if(cola == NULL){
        perror("  ERROR: La cola enviada es NULL");
        return NULL;
    }
    
    return lista_primero(cola->lista);
}


/**
 * Devuelve la cantidad de elementos de la cola o 0 si no existe.
 */
size_t cola_tamanio(cola_t* cola){
    size_t tamanio = NINGUNO;

    if(cola == NULL){
        perror("  ERROR: La cola enviada es NULL");
    } else {
        tamanio = lista_tamanio(cola->lista);
    }

    return tamanio;
}


/**
 * Devuelve true si la cola tiene 0 elementos.
 */
bool cola_vacia(cola_t* cola){
    bool vacia = true;

    if(cola == NULL){
        perror("  ERROR: La cola enviada es NULL");
        vacia = false;
    } else {
        vacia = lista_vacia(cola->lista);
    }
    
    return vacia;
}


/**
 * Libera la memoria reservada por la cola.
 */
void cola_destruir(cola_t* cola){
    if(cola == NULL){
        perror("  ERROR: La cola enviada es NULL no se puede destruir");
        return;
    }
    
    lista_destruir(cola->lista);
    free(cola);
}


void cola_destruir_todo(cola_t* cola, void(*destructor)(void*)){
    lista_destruir_todo(cola->lista, destructor);
    free(cola);
}

