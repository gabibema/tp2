#ifndef __STRUCT_H__
#define __STRUCT_H__

#include "hash.h"
#include "lista.h"

typedef struct hash_elemento{
    char* clave;
    void* valor;
} hash_elemento_t;

struct hash{
    lista_t** tabla_hash;
    hash_destruir_dato_t destructor;
    size_t cantidad; //Elementos en la tabla hash
    size_t capacidad; //Capacidad total de elementos
};



#endif /* __STRUCT_H__ */
