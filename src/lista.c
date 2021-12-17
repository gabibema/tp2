#include "lista.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define PRIMER_POSICION 0
#define UN_ELEMENTO 1
#define NINGUNO 0

/**
 * Crea la lista reservando la memoria necesaria.
 * Devuelve un puntero a la lista creada o NULL en caso de error.
 */
lista_t* lista_crear(){
    lista_t* lista = calloc(1, sizeof(lista_t));
    return lista;
}


/**
 * Inserta un elemento al final de la lista.
 *
 * Devuelve NULL si no pudo insertar el elemento a causa de un error, o la lista en caso de exito.
 */
lista_t* lista_insertar(lista_t* lista, void* elemento){
    nodo_t* nodo_insertar = NULL;

    if(lista == NULL) return NULL;

    nodo_insertar = calloc(1, sizeof(nodo_t));

    if(nodo_insertar == NULL) return NULL;
    nodo_insertar->elemento = elemento;

    if(lista_vacia(lista))
        lista->nodo_inicio = nodo_insertar;
    else 
        lista->nodo_fin->siguiente = nodo_insertar;

    lista->nodo_fin = nodo_insertar;
    lista->cantidad++;

    return lista;
}


/*
 * PRE: El nodo inicial esté inicializado y sus siguientes hasta la posición también o sea NULL.
 * POS: Devuelve el nodo que se encuentra en la posición enviada o hasta encontrar un nodo NULL.
 *      Si el nodo enviado es NULL muestra un mensaje y devuelve NULL;
 */
nodo_t* nodo_en_posicion(nodo_t* nodo_inicial, size_t posicion){
    if(nodo_inicial == NULL)
        return NULL;

    nodo_t* nodo_actual = nodo_inicial;
    int i = 0;

    while((i < posicion) && (nodo_actual != NULL)){
        nodo_actual = nodo_actual->siguiente;
        i++;
    }

    return nodo_actual;
}


/**
 * Inserta un elemento en la posicion indicada, donde 0 es insertar
 * como primer elemento y 1 es insertar luego del primer elemento.  
 * En caso de no existir la posicion indicada, lo inserta al final.
 *
 * Devuelve NULL si no pudo insertar el elemento a causa de un error, o la lista en caso de exito.
 */
lista_t* lista_insertar_en_posicion(lista_t* lista, void* elemento, size_t posicion){
    nodo_t* nodo_anterior = NULL;
    nodo_t* nodo_insertar = NULL;

    if(lista == NULL)
        return NULL;

    nodo_insertar = calloc(1, sizeof(nodo_t));
    if(nodo_insertar == NULL)
        return NULL;
    
    nodo_insertar->elemento = elemento;

    if(posicion >= lista_tamanio(lista)){
        free(nodo_insertar);
        return lista_insertar(lista, elemento);
        
    } else if (posicion == PRIMER_POSICION){
        nodo_insertar->siguiente = lista->nodo_inicio;
        lista->nodo_inicio = nodo_insertar;

    } else {
        nodo_anterior = nodo_en_posicion(lista->nodo_inicio, posicion - 1);

        nodo_insertar->siguiente = nodo_anterior->siguiente;
        nodo_anterior->siguiente = nodo_insertar;
    }
    lista->cantidad++;
    return lista;
}


/**
 * Quita de la lista el elemento que se encuentra en la ultima posición.
 *
 * Devuelve el elemento removido de la lista o NULL en caso de error.
 */
void* lista_quitar(lista_t* lista){
    if(lista_vacia(lista))
        return NULL;

    size_t tamanio = lista_tamanio(lista);
    size_t posicion_ultimo = tamanio - 1;
    void* elemento = lista_ultimo(lista);
    nodo_t* nodo_ultimo = lista->nodo_fin;
    nodo_t* nodo_anteultimo = NULL;

    if(tamanio == UN_ELEMENTO){
        lista->nodo_fin = NULL;
        lista->nodo_inicio = NULL;

    } else{
        nodo_anteultimo = nodo_en_posicion(lista->nodo_inicio, posicion_ultimo - 1);
        nodo_anteultimo->siguiente = NULL;
        lista->nodo_fin = nodo_anteultimo;
    }

    free(nodo_ultimo);
    lista->cantidad --;
    return elemento;
}


/**
 * Quita de la lista el elemento que se encuentra en la posición
 * indicada, donde 0 es el primer elemento de la lista.
 *
 * En caso de no existir esa posición se intentará borrar el último
 * elemento.
 *
 * Devuelve el elemento removido de la lista o NULL en caso de error.
 *
 */
void* lista_quitar_de_posicion(lista_t* lista, size_t posicion){
    if(lista == NULL)
        return NULL;
    else if(lista_vacia(lista))
        return NULL;

    size_t cantidad_elementos = lista_tamanio(lista);
    nodo_t* nodo_anterior = NULL;
    nodo_t* nodo_quitar = NULL;
    void* elemento = NULL;

    if (posicion >= cantidad_elementos - 1){
        return lista_quitar(lista);

    } else if (posicion == PRIMER_POSICION){
        nodo_quitar = lista->nodo_inicio;
        elemento = nodo_quitar->elemento;
        lista->nodo_inicio = nodo_quitar->siguiente;

    } else{
        nodo_anterior = nodo_en_posicion(lista->nodo_inicio, posicion - 1);

        nodo_quitar = nodo_anterior->siguiente;
        nodo_anterior->siguiente = nodo_quitar->siguiente;
        elemento = nodo_quitar->elemento;
    }

    free(nodo_quitar);
    lista->cantidad--;
    return elemento;
}


/**
 * Devuelve el elemento en la posicion indicada, donde 0 es el primer
 * elemento.
 *
 * Si no existe dicha posicion devuelve NULL.
 */
void* lista_elemento_en_posicion(lista_t* lista, size_t posicion){
    if(lista  == NULL)
        return NULL;
    else if(lista_vacia(lista))
        return NULL;
     else if (posicion >= lista_tamanio(lista))
        return NULL;
    

    nodo_t* nodo_actual = nodo_en_posicion(lista->nodo_inicio, posicion);

    return nodo_actual->elemento;
}


/**
 * Devuelve el primer elemento de la lista o NULL si la lista se
 * encuentra vacía o no existe.
 */
void* lista_primero(lista_t* lista){
    if(lista  == NULL)
        return NULL;
    else if(lista_vacia(lista))
        return NULL;
    
    return lista->nodo_inicio->elemento;
}


/**
 * Devuelve el último elemento de la lista o NULL si la lista se
 * encuentra vacía o no existe.
 */
void* lista_ultimo(lista_t* lista){
    if(lista  == NULL)
        return NULL;
    else if(lista_vacia(lista))
        return NULL;

    return lista->nodo_fin->elemento;
}


/**
 * Devuelve true si la lista está vacía (o no existe) o false en caso contrario.
 */
bool lista_vacia(lista_t* lista){
    bool vacia = false;
    
    if(lista == NULL){
        vacia = true;
    }else if(lista->nodo_inicio == NULL){
        vacia = true;
    }else if(lista->nodo_fin == NULL){
        vacia = true;
    }else if(lista->cantidad == NINGUNO){
        vacia = true;
    }

    return vacia;
}


/**
 * Devuelve la cantidad de elementos almacenados en la lista.
 * Una lista que no existe no puede tener elementos.
 */
size_t lista_tamanio(lista_t* lista){
    size_t tamanio = 0;

    if(lista != NULL){
        tamanio = lista->cantidad;
    }

    return tamanio;
}


//Hacer pruebas
/**
 * Libera la memoria reservada por la lista y aplica el destructor a cada elemento.
 */
void lista_destruir_todo(lista_t* lista, void (*destructor)(void*)){
    nodo_t* nodo_actual = NULL;
    nodo_t* nodo_siguiente = NULL;

    if(lista == NULL)
        return;
    
    nodo_actual = lista->nodo_inicio;
    for(int i = 0; i < lista->cantidad; i++){
        nodo_siguiente = nodo_actual->siguiente;
        if(destructor)
            destructor(nodo_actual->elemento);
        free(nodo_actual);
        nodo_actual = nodo_siguiente;
    }

    free(lista);
}


/**
 * Libera la memoria reservada por la lista.
 */
void lista_destruir(lista_t* lista){
    nodo_t* nodo_actual = NULL;
    nodo_t* nodo_siguiente = NULL;

    if(lista == NULL)
        return;
    
    nodo_actual = lista->nodo_inicio;
    for(int i = 0; i < lista->cantidad; i++){
        nodo_siguiente = nodo_actual->siguiente;
        free(nodo_actual);
        nodo_actual = nodo_siguiente;
    }

    free(lista);
}

/**
 * Crea un iterador para una lista. El iterador creado es válido desde
 * el momento de su creación hasta que no haya mas elementos por
 * recorrer o se modifique la lista iterada (agregando o quitando
 * elementos de la lista). 
 *
 * Al momento de la creación, el iterador queda listo para devolver el
 * primer elemento utilizando lista_iterador_elemento_actual.
 *
 * Devuelve el puntero al iterador creado o NULL en caso de error.
 */
lista_iterador_t* lista_iterador_crear(lista_t* lista){
    if(lista == NULL)
        return NULL;

    lista_iterador_t* iterador = malloc(sizeof(lista_iterador_t));
    if(iterador == NULL)
        return NULL;
    

    iterador->corriente = lista->nodo_inicio;
    iterador->lista = lista;

    return iterador;
}


/**
 * Devuelve true si hay mas elementos sobre los cuales iterar o false
 * si no hay mas.
 */
bool lista_iterador_tiene_siguiente(lista_iterador_t* iterador){
    bool hay_siguiente = true;

    if(iterador == NULL){
        hay_siguiente = false;
    } else if(iterador->corriente == NULL){
        hay_siguiente = false;
    } 

    return hay_siguiente;
}


/**
 * Avanza el iterador al siguiente elemento.
 * Devuelve true si pudo avanzar el iterador o false en caso de
 * que no queden elementos o en caso de error.
 *
 * Una vez llegado al último elemento, si se invoca a
 * lista_iterador_elemento_actual, el resultado siempre será NULL.
 */
bool lista_iterador_avanzar(lista_iterador_t* iterador){
    bool avanzo = true;

    if(lista_iterador_tiene_siguiente(iterador)){
        iterador->corriente = iterador->corriente->siguiente;
    }
    //Caso de último elemento
    if(iterador->corriente == NULL){
        avanzo = false;
    }

    return avanzo;
}


/**
 * Devuelve el elemento actual del iterador o NULL en caso de que no
 * exista dicho elemento o en caso de error.
 */
void* lista_iterador_elemento_actual(lista_iterador_t* iterador){
    void* elemento_actual = NULL;

    if(iterador != NULL && iterador->corriente != NULL)
        elemento_actual = iterador->corriente->elemento;
    
    return elemento_actual;
}


/**
 * Libera la memoria reservada por el iterador.
 */
void lista_iterador_destruir(lista_iterador_t* iterador){
    if(iterador == NULL){
        return;
    }

    free(iterador);
}


/**
 * Iterador interno. Recorre la lista e invoca la funcion con cada elemento de
 * la misma como primer parámetro. Dicha función puede devolver true si se deben
 * seguir recorriendo elementos o false si se debe dejar de iterar elementos.
 *
 * El puntero contexto se pasa como segundo argumento a la función del usuario.
 *
 * La función devuelve la cantidad de elementos iterados o 0 en caso de error.
 */
size_t lista_con_cada_elemento(lista_t* lista, bool (*funcion)(void*, void*), void *contexto){
    size_t cantidad_elementos = NINGUNO;
    size_t cantidad_iterados = NINGUNO;
    void* elemento_actual = NULL;
    bool puedo_iterar = true;
    nodo_t* actual = NULL;

    if(lista == NULL){
        return cantidad_iterados;
    } else if(lista_vacia(lista)){
        return cantidad_iterados;
    } else if(funcion == NULL){
        return cantidad_iterados;
    }

    cantidad_elementos = lista_tamanio(lista);
    actual = lista->nodo_inicio;

    while((puedo_iterar) && (cantidad_iterados < cantidad_elementos)){
        elemento_actual = actual->elemento;
        puedo_iterar = funcion(elemento_actual, contexto);

        actual = actual->siguiente;
        cantidad_iterados++;
    }

    return cantidad_iterados;
}
