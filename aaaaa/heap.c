#include "heap.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "struct.h"
#define ERROR -1
#define EXITO 1

struct _heap_t{
  void** vector_heap;
  int (*comparador)(void*,void*);
  size_t tamanio;
  size_t tope;
};


heap_t* heap_crear(int (*comparador)(void*, void*), size_t tamanio_inicial){
    if(!comparador) return NULL;

    heap_t* heap = calloc(1, sizeof(heap_t));
    if(!heap) return NULL;

    heap->comparador = comparador;
    heap->vector_heap = malloc(tamanio_inicial*sizeof(void*));
    if(!heap->vector_heap){
        free(heap);
        return NULL;
    }

    return heap;
}
/*
* Retorna la cantidad de elementos que tiene el heap en total.
*/
int heap_cantidad(heap_t* heap){
    if(!heap) return ERROR;
    return (int)heap->tope;
}

size_t heap_tamanio(heap_t* heap){
    if(!heap) return ERROR;
    return (int)heap->tamanio;
}

/*
 * Swapea los elementos del vector entre si.
*/
void swap(void** vector, size_t i, size_t j){
    void* aux = vector[i];
    vector[i] = vector[j];
    vector[j] = aux;
}

size_t posicion_padre(size_t posicion){
    return ((posicion - 1)/2);
}

/*
* Chequea si el heap se encuentra vacio o no.
*/
bool heap_vacio(heap_t* heap){
    return ((heap == NULL ) || (heap->vector_heap == NULL) || heap->tamanio == 0);
}

/*
 * Recibe el heap y una posicion.
 * Aplica sift up a los elementos del vector.
*/
void sift_up(heap_t* heap, size_t pos){
    if(heap_vacio(heap) || pos == 0) return;

    size_t pos_padre = posicion_padre(pos);

    if(heap->comparador(heap->vector_heap[pos], heap->vector_heap[pos_padre]) < 0){
        swap(heap->vector_heap,pos,pos_padre);
        sift_up(heap,pos_padre);
    }
}

int heap_insertar(heap_t* heap, void* elemento){
    if(!heap) return ERROR;

    if(heap->tamanio == heap->tope){ //aumento el heap cuando llego a la capacidad maxima
        void** heap_aux = realloc(heap->vector_heap,sizeof(void*) * (heap->tope+1)); 
        if(!heap_aux) return ERROR;
        
        heap->vector_heap = heap_aux;
    }

    heap->vector_heap[heap->tope] = elemento;
    sift_up(heap,heap->tope);

    heap->tope++;
    heap->tamanio++;
    return EXITO;
}

size_t posicion_hijo_izquierdo(size_t posicion){
    return posicion*2 + 1;
}

size_t posicion_hijo_derecho(size_t posicion){
    return posicion*2 + 2;
}

/*
 * Recibe el heap y una posicion.
 * Aplica sift down a los elementos del vector. 
*/
void sift_down(heap_t* heap,size_t pos){
    if(heap_vacio(heap)) return;

    size_t pos_izquierdo = posicion_hijo_izquierdo(pos);
    if(pos_izquierdo >= heap->tope) return;
    
    size_t pos_menor = pos_izquierdo;
    size_t pos_derecha = posicion_hijo_derecho(pos);

    if(pos_derecha < heap->tope){
        if(heap->comparador(heap->vector_heap[pos_derecha], heap->vector_heap[pos_menor]) < 0)
            pos_menor = pos_derecha;
        
    }

    if(heap->comparador(heap->vector_heap[pos], heap->vector_heap[pos_menor]) > 0){
        swap(heap->vector_heap, pos, pos_menor);
        sift_down(heap,pos_menor);
    }
}

void* heap_extraer_raiz(heap_t* heap){
    if(heap_vacio(heap)) return NULL;

    void* elemento = heap->vector_heap[0];
    swap(heap->vector_heap, 0, heap->tope-1);

    void** aux = realloc(heap->vector_heap, sizeof(void*) * (heap->tamanio-1));
    if(!aux) return elemento; 
    heap->vector_heap = aux;

    heap->tope--;
    if(heap->tope > 0)
        sift_down(heap, 0);

    heap->tamanio--;
    return elemento;
}

void heap_destruir(heap_t* heap){
    if(!heap) return;

    free(heap->vector_heap);
    free(heap);
}
