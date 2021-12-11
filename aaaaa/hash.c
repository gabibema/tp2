#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"
#include "lista.h"
#include "struct.h"

#define EXITO 0
#define ERROR -1
#define NINGUNO 0
#define MAX_ELEMENTOS 2
#define MIN_HASH_TAMANIO 3
#define MAX_FACTOR_CARGA 0.75

typedef struct datos_funcion{
    bool termina;
    bool (*funcion)(hash_t* hash, const char* clave, void* aux);
    hash_t* hash;
    void* aux;

} datos_funcion_t;


/*
 * Destruye el valor aplicando el destructor enviado si es válido.
 */
void liberar_valor(void* valor, void (*destructor)(void*)){
    if(destructor)
        destructor(valor);
}


/*
 * Destruye el toda la memoria almacenada por el elemento y el valor.
 */
void liberar_elemento(hash_elemento_t* elemento, void (*destructor)(void*)){
    if(elemento == NULL) return;

    liberar_valor(elemento->valor, destructor);
    free(elemento->clave);
    free(elemento);   
}


/*
 * Destruye el toda la memoria almacenada en las listas internas de la tabla (no libera los elementos de cada nodo).
 */
void liberar_tabla(lista_t** tabla, size_t cantidad){
    if(tabla == NULL) return;

    for(int i = 0;  i < cantidad; i++)
        lista_destruir(tabla[i]);
        
    free(tabla);
}


/*
 * Devuelve un puntero a un elemento que contiene la clave y el valor enviados
 * O Null en caso de error.
 */
hash_elemento_t* crear_elemento(const char* clave, void* valor){
    hash_elemento_t* elemento = malloc(sizeof(hash_elemento_t));
    if(!elemento) return NULL;

    elemento->clave = calloc(1, strlen(clave) + 1);
    if(!elemento->clave) return NULL;

    strcpy(elemento->clave, clave);
    elemento->valor = valor;

    return elemento;
}


/*
 * Crea una tabla reservando la tabla reservando la memoria necesaria para esta.
 * Devuelve un puntero a la tabla con la cantidad de listas creadas enviadas por parámetro, cada lista estará inicializada.
 * Devuelve Null en caso de error.
 */
lista_t** tabla_crear(size_t cantidad){
    lista_t** tabla = calloc(cantidad, sizeof(lista_t*));
    bool error = false;
    size_t i = 0;

	if (tabla == NULL) return NULL;
    while(i < cantidad && !error){
        tabla[i] = lista_crear();
        if(tabla[i] == NULL)
            error = true;
        else
            i++;
    }
    
    if(error){
        liberar_tabla(tabla, cantidad);
        return NULL;
    }

    return tabla;
}


hash_t* hash_crear(hash_destruir_dato_t destruir_elemento, size_t tamanio_inicial){
    if(tamanio_inicial < MIN_HASH_TAMANIO)
        tamanio_inicial = MIN_HASH_TAMANIO;

    hash_t* hash = calloc(1,sizeof(hash_t));
    if(!hash) return NULL;

    hash->destructor = destruir_elemento;
    hash->capacidad = tamanio_inicial;
    hash->tabla_hash = tabla_crear(tamanio_inicial);

    if(hash->tabla_hash == NULL){
        free(hash);
        return NULL;
    }
    return hash;
}


/*
 * Devuelve la cantidad de elementos almacenados en el hash o 0 en
 * caso de error.
 */
size_t hash_cantidad(hash_t* hash){
    if(hash == NULL) return NINGUNO;
    return hash->cantidad;
}


/*
 * Devuelve un número en función de la clave enviada estará dentro del rango 0 a tamanio - 1 (incluidos). 
   Devuelve -1 en caso de error.
 */
int funcion_hash(const char* clave, size_t tamanio){
    if(clave == NULL || !tamanio) return ERROR;

    int indice = 0;
    size_t clave_tamanio = strlen(clave) + 1;
    
    for(size_t i = 0; i < clave_tamanio; i++){
        indice += (int)(((size_t)clave[i])*(i+1));
    }

    return (indice % (int)tamanio); 
}


/*
 * Devuelve la posición en la que se encuentra el elemento dentro de la lista asociado a la clave enviada.
 * Devuelve -1 en caso de error.
 */
int obtener_posicion_elemento(lista_t* lista, const char* clave){
    int posicion_lista = 0;
    hash_elemento_t* elemento = NULL;
    bool encontrado = false, error = false;
    
    lista_iterador_t* iterador = lista_iterador_crear(lista);
    if(iterador == NULL)
        return ERROR;

    while(lista_iterador_tiene_siguiente(iterador) && !encontrado && !error) {
        elemento = lista_iterador_elemento_actual(iterador);
        if(elemento == NULL)
            error = true;
        else if(strcmp(elemento->clave, clave) == 0)
            encontrado = true;
        else
            posicion_lista++;

        lista_iterador_avanzar(iterador);
    }
    lista_iterador_destruir(iterador);

    posicion_lista = encontrado ? posicion_lista : ERROR;    
    return posicion_lista;
}


/*
 * Devuelve el puntero al elemento dentro de la lista que contiene la clave enviada
 * Devuelve NULL en caso de no encontrarlo o error.
 */
hash_elemento_t* obtener_elemento(lista_t* lista, const char* clave){
    bool error = false, encontrado = false;
    lista_iterador_t* iterador = lista_iterador_crear(lista);
    hash_elemento_t* actual = NULL;

    if(!iterador) return NULL;

    while(lista_iterador_tiene_siguiente(iterador) && !encontrado && !error){
        actual = lista_iterador_elemento_actual(iterador);
        if(actual == NULL)
            error = true;
        else if(strcmp(actual->clave, clave) == 0)
            encontrado = true;

        lista_iterador_avanzar(iterador);
    }
    lista_iterador_destruir(iterador);

    actual = encontrado ? actual : NULL;
    return actual;
}


/*
 * Reemplaza el valor del elemento de la lista que coincida con la clave provista
 * Devuelve true si pudo reemplazar el valor 
 * Devuelve false en caso contrario o si hubo error.
 */
bool reemplazar_repetido(lista_t* lista, const char* clave, void* valor, void (*destructor)(void*)){
    if(lista == NULL|| clave == NULL) return false;

    hash_elemento_t* reemplazar = obtener_elemento(lista, clave);
    bool reemplazado = false;

    if(reemplazar != NULL){
        liberar_valor(reemplazar->valor, destructor);
        reemplazar->valor = valor;
        reemplazado = true;
    }

    return reemplazado;
}


/*
 * Quita un elemento del hash e invoca la funcion destructora
 * pasandole dicho elemento.
 * Devuelve 0 si pudo eliminar el elemento o -1 si no pudo.
 */
int hash_quitar(hash_t* hash, const char* clave){
    if(!hash || !hash->tabla_hash || !clave) return ERROR;

    int estado = ERROR;
    int posicion_tabla = funcion_hash(clave, hash->capacidad);
    if(posicion_tabla == ERROR) return ERROR;

    int posicion_lista = obtener_posicion_elemento(hash->tabla_hash[posicion_tabla], clave);
    if(posicion_lista == ERROR) return ERROR;

    hash_elemento_t* elemento = lista_quitar_de_posicion(hash->tabla_hash[posicion_tabla], (size_t)posicion_lista);
    if(elemento != NULL){
        liberar_elemento(elemento, hash->destructor);
        hash->cantidad --;
        estado = EXITO;
    }

    return estado;
}


/*
 * Insertará el valor asociado a la clave dentro de la lista (o reemplazará al existente)
 * Devuelve 0 si pudo insertarlo.
 * Devuelve -1 si no pudo o hubo error.
 */
int insertar_elemento(lista_t* lista, const char* clave, void* valor, size_t* hash_tamanio, void (*destructor)(void*)){
    hash_elemento_t* insertar = NULL;
    if((lista == NULL) || (clave == NULL))return ERROR;

    bool reemplazado = reemplazar_repetido(lista, clave, valor, destructor);

    if(!reemplazado){
        insertar = crear_elemento(clave, valor);
        if(insertar == NULL) return ERROR;

        if(lista_insertar(lista, insertar) == NULL){
            liberar_elemento(insertar, destructor);
            return ERROR;
        } 
        (*hash_tamanio)++;
    }

    return EXITO;
}


/*
 * Devuelve true si el factor si la tabla alcanzó el factor de carga o lo superó.
 * Devuelve false en caso contrario.
 */
bool tabla_llena(size_t casilleros_llenos, size_t casilleros_totales){    
    if(casilleros_totales == 0) return false;

    float factor_carga = (float)casilleros_llenos/(float)casilleros_totales;
    return (factor_carga >= MAX_FACTOR_CARGA);
}

/*
 * Inserta todos los elementos de la tabla_1 en la tabla_2.
 * Devuelve true si pudo insertar todos los elementos en la tabla_2.
 * Devuelve false si no pudo insertarlos o hubo error.
 */
bool transcribir_tabla(lista_t** tabla_1, size_t capacidad_1, lista_t** tabla_2, size_t capacidad_2){
    if(tabla_1 == 0 || tabla_2 == 0) return false;
    
    size_t i = 0;
    size_t posicion = 0;
    bool error = false;

    while(i < capacidad_1 && !error){
        lista_iterador_t* iterador = lista_iterador_crear(tabla_1[i]);
        hash_elemento_t* elemento = NULL;

        while(lista_iterador_tiene_siguiente(iterador) && !error){
            elemento = lista_iterador_elemento_actual(iterador);
            if(elemento == NULL)
                error = true;
            else
                posicion = (size_t)funcion_hash(elemento->clave, capacidad_2); //No puede dar -1 al existir las claves en el primer hash
            
            if((!error) && (lista_insertar(tabla_2[posicion], elemento) == NULL)) //El elemento es válido, pero no pude insertar
                error = true;
        
            lista_iterador_avanzar(iterador);
        }
        lista_iterador_destruir(iterador);
        i++;
    }

    return error;
}

#define AMPLIFICADOR_TABLA 2

/*
 * Reorganiza todos los nuevos elementos del hash original en otra nueva tabla.
 * Dicha tabla tendrá el tamaño de la capacidad del hash * AMPLIFICADOR_TABLA
 */
void rehashear(hash_t* hash){
    bool estado = false;
    if(!hash) return;

    lista_t** tabla_aux = tabla_crear(hash->capacidad * AMPLIFICADOR_TABLA);
    if(!tabla_aux) return;

    estado = transcribir_tabla(hash->tabla_hash, hash->capacidad, tabla_aux, hash->capacidad * AMPLIFICADOR_TABLA);
    if(!estado){
        liberar_tabla(tabla_aux, hash->capacidad * AMPLIFICADOR_TABLA);
        return;
    }

    liberar_tabla(hash->tabla_hash, hash->capacidad);
    hash->capacidad = hash->capacidad * AMPLIFICADOR_TABLA;
    hash->tabla_hash = tabla_aux;
}

/*
 * Inserta un elemento en el hash asociado a la clave dada.
 *
 * Nota para los alumnos: Recordar que si insertar un elemento provoca
 * que el factor de carga exceda cierto umbral, se debe ajustar el
 * tamaño de la tabla para evitar futuras colisiones.
 *
 * Devuelve 0 si pudo guardarlo o -1 si no pudo.
 */
int hash_insertar(hash_t* hash, const char* clave, void* elemento){
    if(!hash || !clave) return ERROR;

    int indice = funcion_hash(clave, hash->capacidad);  //No puede dar -1 porque ya se validó la clave

    insertar_elemento(hash->tabla_hash[indice], clave, elemento, &(hash->cantidad), hash->destructor);
    if(tabla_llena(hash->cantidad, hash->capacidad))
        rehashear(hash);

    return EXITO;
}


/*
 * Libera toda la memoria almacenada de cada elemento de la lista aplicando el correspondiente destructor.
 */
void destruir_elementos(lista_t* lista, void (*destructor)(void*)){
    if(!lista) return;
    lista_iterador_t* iterador = lista_iterador_crear(lista);
    hash_elemento_t* elemento = NULL;

    if(!iterador) return;

    while(lista_iterador_tiene_siguiente(iterador)){
        elemento = lista_iterador_elemento_actual(iterador);
        liberar_elemento(elemento, destructor); //la función valida si es NULL el elemento
        lista_iterador_avanzar(iterador);     
    }
    lista_iterador_destruir(iterador);
}

void hash_destruir(hash_t* hash){
    if(!hash || !hash->tabla_hash) return;

    for(int i = 0; i < hash->capacidad; i++){
        destruir_elementos(hash->tabla_hash[i], hash->destructor);
    }

    liberar_tabla(hash->tabla_hash, hash->capacidad);
    free(hash);
}


/*
 * Devuelve un elemento del hash con la clave dada o NULL si dicho
 * elemento no existe (o en caso de error).
 */
void* hash_obtener(hash_t* hash, const char* clave){
    if(!hash || !hash->tabla_hash || !clave) return NULL;

    hash_elemento_t* elemento = NULL;
    int indice = funcion_hash(clave, hash->capacidad); //No puede dar -1 porque ya se validó la clave;  
    void* valor = NULL;

    elemento = obtener_elemento(hash->tabla_hash[indice], clave);
    valor = (elemento != NULL) ? elemento->valor : NULL;

    return valor;
}

/*
 * Devuelve true si el hash contiene un elemento almacenado con la
 * clave dada o false en caso contrario (o en caso de error).
 */
bool hash_contiene(hash_t* hash, const char* clave){
    if(!hash || !hash->tabla_hash || !clave) return NULL;

    hash_elemento_t* elemento = NULL;
    int indice = funcion_hash(clave, hash->capacidad);  //No puede dar -1 porque ya se validó la clave
    bool encontrado = false;

    elemento = obtener_elemento(hash->tabla_hash[indice], clave);
    encontrado = (elemento != NULL) ? true : false;

    return encontrado;
}


/*
 * Castea elemento_aux a elemento_t* y datos_aux a datos_t*. 
 * Aplica la funcion guardada en datos devolviendo true si se debe
 * seguir aplicando la función guardado o false en caso contrario.
 */
bool aplicar_funcion(void* elemento_aux, void* datos_aux){
    if(!elemento_aux || !datos_aux) return false;

    hash_elemento_t* elemento = elemento_aux;
    datos_funcion_t* datos = datos_aux;

    if(!datos->termina)
        datos->termina = datos->funcion(datos->hash, elemento->clave, datos->aux);

    return !datos->termina;
}


/*
 * Recorre cada una de las claves almacenadas en la tabla de hash e
 * invoca a la función funcion, pasandole como parámetros el hash, la
 * clave en cuestión y el puntero auxiliar.
 *
 * Mientras que queden mas claves o la funcion retorne false, la
 * iteración continúa. Cuando no quedan mas claves o la función
 * devuelve true, la iteración se corta y la función principal
 * retorna.
 *
 * Devuelve la cantidad de claves totales iteradas (la cantidad de
 * veces que fue invocada la función) o 0 en caso de error.
 *
 */
size_t hash_con_cada_clave(hash_t* hash, bool (*funcion)(hash_t* hash, const char* clave, void* aux), void* aux){
    if(!hash || !funcion) return 0;
    datos_funcion_t datos = {false, funcion, hash, aux};
    size_t iterados = 0;
    size_t i = 0;

    while(i < hash->capacidad && !datos.termina){
        iterados += lista_con_cada_elemento(hash->tabla_hash[i], aplicar_funcion, &datos); 
        i++;   
    }
    return iterados;
}