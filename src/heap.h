#ifndef HEAP_H_
#define HEAP_H_

typedef struct _heap_t heap_t;

heap_t* heap_crear(int (*comparador)(void*, void*), size_t tamanio_inicial);


/*
 * Se inserta un elemento en el heap manteniÃ©ndolo minimal
 * Devuelve 0 en caso de exito o -1 en caso de error
 */
int heap_insertar(heap_t* heap,void* elemento);

void* heap_extraer_raiz(heap_t* heap);

size_t heap_tamanio(heap_t* heap);


void heap_destruir(heap_t* heap);


#endif // HEAP_H_
