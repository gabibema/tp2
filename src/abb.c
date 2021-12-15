#include "abb.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


#define NINGUNO 0
#define UN_ELEMENTO 1


typedef struct abb_datos_recorrido{
  bool puedo_recorrer;
  size_t recorridos;
} abb_datos_recorrido_t;

typedef struct datos_array {
  void** array;
  size_t tamanio_maximo; //La cantidad máxima de elementos que puede almacenar
  size_t cantidad_elementos; //La cantidad actual de elementos en el array
} datos_array_t;


abb_t* abb_crear(abb_comparador comparador){
    if(comparador == NULL){
      perror(" ERROR! El comparador enviado es NULL, no se puede crear el árbol");
      return NULL;
    }

    abb_t* arbol = calloc(1, sizeof(abb_t));

    if(arbol == NULL){
      perror(" ERROR! No hay memoria suficiente para crear el arbol");
      return NULL;
    }

    arbol->comparador = comparador;

    return arbol;
}

/*
 * PRE: -
 * POS: Creará un nodo que almacena el elemento enviado e inicializará sus otros campos.
 *      Devolverá el nodo si pudo crearlo o NULL si no hay memoria disponible.
 */
nodo_abb_t* nodo_crear(void* elemento){
  nodo_abb_t* nodo = calloc(1, sizeof(nodo_abb_t));
  if(nodo == NULL){
    perror(" ERROR! No hay memoria suficiente para insertar el elemento");
    return NULL;
  }

  nodo->elemento = elemento;
  return nodo;
}

/*
 * Pre: El comparador no sea NULL.
        El el puntero al booleano no sea NULL.
 * Pos: Recorrerá el árbol a partir del nodo ingresado hasta encontrar donde insertar.
 *      Si hay memoria para crear el nodo, lo agregará en el lugar correspondiente
 *      Si pudo insertar cambiará insertado a true por referencia
 *      En caso de error no se modificará la rama del árbol
 *      Si no pudo insertar cambiará insertado a false por referencia
 */
nodo_abb_t* nodo_insertar(nodo_abb_t* actual, void* elemento, abb_comparador comparador, bool* insertado){
  if(actual == NULL){
    actual = nodo_crear(elemento);

    if(actual == NULL)
      (*insertado) = false;
    else
      (*insertado) = true;
    return actual;
  }

  int comparacion = comparador(elemento, actual->elemento);
  if(comparacion <= 0){
    actual->izquierda = nodo_insertar(actual->izquierda, elemento, comparador, insertado);
  } else{
    actual->derecha = nodo_insertar(actual->derecha, elemento, comparador, insertado);
  }

  return actual;
}


abb_t* abb_insertar(abb_t* arbol, void* elemento){

  if(arbol == NULL){
    perror(" ERROR! El árbol enviado es NULL, no se pueden agregar elementos");
    return NULL;
  }

  bool insertado = false;
  arbol->nodo_raiz = nodo_insertar(arbol->nodo_raiz, elemento, arbol->comparador, &insertado);
  if(insertado == false)
    return NULL;

  arbol->tamanio++;
  return arbol;
}


/* 
 * PRE: -
 * POS: Devolverá true si el nodo actual tiene un hijo derecho válido.
 *      En caso contrario o error devolverá false.
 */
bool tiene_nodo_derecho(nodo_abb_t* nodo){
  bool tiene = true;
  if(nodo == NULL || nodo->derecha == NULL)
    tiene = false;

  return tiene;
}

/* 
 * PRE: -
 * POS: Devolverá true si el nodo actual tiene un hijo izquierdo válido.
 *      En caso contrario o error devolverá false.
 */
bool tiene_nodo_izquierdo(nodo_abb_t* nodo){
  bool tiene = true;
  if(nodo == NULL || nodo->izquierda == NULL)
    tiene = false;

  return tiene;
}


/*
 * PRE: -
 * POS: Devolverá true si el nodo enviado tiene hijo derecho e izquierdo válidos. 
 *      Caso contrario devolverá false.
 */
bool es_nodo_doble(nodo_abb_t* actual){
  if(actual == NULL)
    return false;
  else if (actual->izquierda == NULL || actual->derecha == NULL)
    return false;

  return true;
}


/* 
 * PRE: -
 * POS: Extraerá el predecesor directo de la rama, reasignando los punteros correspondientes.
 *      Si la rama enviada es NULL
 *      Si la rama no tiene nodo izquierdo devolverá NULL
 */
void quitar_nodo_doble(nodo_abb_t* nodo){
  if(nodo == NULL)
    return;
  else if(!es_nodo_doble(nodo))
    return;

  nodo_abb_t* predecesor = nodo->izquierda;
  nodo_abb_t* padre_predecesor = nodo;

  if(!tiene_nodo_derecho(predecesor))
    padre_predecesor->izquierda = predecesor->izquierda;
  else{
    while(tiene_nodo_derecho(predecesor)){ //busco el último
      padre_predecesor = predecesor;
      predecesor = predecesor->derecha;
    }

    padre_predecesor->derecha = predecesor->izquierda;
  }

  nodo->elemento = predecesor->elemento;
  free(predecesor);

}


/*
 * PRE: -
 * POS: Devuelve true si el hijo derecho del padre es la rama enviada.
 *      Devuelve false en caso contrario.
 */
bool es_hijo_derecho(nodo_abb_t* rama, nodo_abb_t* padre){
  bool hijo_derecho = true;
  if(rama == NULL || padre == NULL)
    hijo_derecho = false;
  else if (padre->derecha != rama)
    hijo_derecho = false;

  return hijo_derecho;
}

/*
 * PRE: -
 * POS: Devuelve true si el hijo izquierda del padre es la rama enviada.
 *      Devuelve false en caso contrario.
 */
bool es_hijo_izquierdo(nodo_abb_t* rama, nodo_abb_t* padre){
  bool hijo_izquierda = true;
  if(rama == NULL || padre == NULL)
    hijo_izquierda = false;
  else if (padre->izquierda != rama)
    hijo_izquierda = false;

  return hijo_izquierda;
}


/*
 * PRE: -
 * POS: Devuelve true si nodo tiene un nodo izquierda o derecha válido.
 *      Devuelve false en caso contrario.
 */
bool es_nodo_simple(nodo_abb_t* nodo){
  if(nodo == NULL)
    return false;
  else if(nodo->izquierda == NULL && nodo->derecha == NULL)
    return false;
  
  return true;
}


/*
 * PRE: La rama no sea NULL y debe tener un hijo.
 * POS: Quitará el elemento nodo enviado reasignando sus elementos y punteros por los de 
 *      su hijo. Libera el hijo de la rama.
 */
void quitar_nodo_simple(nodo_abb_t* rama){
  nodo_abb_t* hijo = NULL;
  if(rama == NULL)
    return;
  
  if(rama->derecha != NULL){
    hijo = rama->derecha;
  } else {
    hijo = rama->izquierda;
  }

  rama->elemento = hijo->elemento;
  rama->derecha = hijo->derecha;
  rama->izquierda = hijo->izquierda;

  free(hijo);
}

/*
 * PRE: El nodo no sea NULL y tenga padre válido
 * POS: Reasigna la izquierda o derecha del padre por NULL según corresponda
 *      Libera el nodo que se desea quitar.
 */
void quitar_hoja(nodo_abb_t* nodo, nodo_abb_t* padre){
  if(nodo == NULL)
    return;
  
  if(es_hijo_derecho(nodo, padre))
    padre->derecha = NULL;
  else if (es_hijo_izquierdo(nodo, padre))
    padre->izquierda = NULL;

  free(nodo);
}


/*
 * PRE: Comparador o actual no sean NULL
 * POS: Recorre recursivamente a partir del nodo enviado hasta encontrar
 *      el elemento que se desea enviar. Al encontrar el elemento buscado, se 
 *      lo elimina del árbol, destruyendo la memoria correspondiente.
 */
void* abb_quitar_nodo(nodo_abb_t* actual, void* elemento, abb_comparador comparador, nodo_abb_t* padre, size_t* arbol_tamanio){
  if(actual == NULL || comparador == NULL)
     return NULL; //caso de no encontrado

  int comparacion = comparador(elemento, actual->elemento);
  if(comparacion == 0){
    void* quitado = actual->elemento;

    if(es_nodo_doble(actual))
      quitar_nodo_doble(actual);
    else if(es_nodo_simple(actual))
      quitar_nodo_simple(actual);
    else
      quitar_hoja(actual, padre);

    (*arbol_tamanio)--;
    return quitado;

  } else if (comparacion < 0)
    return abb_quitar_nodo(actual->izquierda, elemento, comparador, actual, arbol_tamanio);

  return abb_quitar_nodo(actual->derecha, elemento, comparador, actual, arbol_tamanio);
}



void* abb_quitar(abb_t* arbol, void *elemento){  
  void* elemento_quitado = NULL;

  if(abb_vacio(arbol)){
    perror(" ERROR! No se pueden quitar elementos del árbol");
    return NULL;
  }

  elemento_quitado = abb_quitar_nodo(arbol->nodo_raiz, elemento, arbol->comparador, NULL, &arbol->tamanio);

  if(abb_tamanio(arbol) == NINGUNO)
    arbol->nodo_raiz = NULL;

  return elemento_quitado;
}


/*
 * PRE: Comparador no sea NULL
 * POS: Recorre recursivamente el árbol en función del resultado de la comparación entre
 *      el elemento actual y el elemento enviado. 
 *      Si se encuentra el elemento el elemento almacenador en el nodo.
 *      Sinó devuelve NULL.
 */
void* elemento_buscar(nodo_abb_t* actual, void* elemento, abb_comparador comparador){
  int comparacion = 0;
  if(actual == NULL) //Caso de elemento no encontrado
    return NULL; 

  comparacion = comparador(elemento, actual->elemento);
  if(comparacion == 0)
    return actual->elemento;
  else if(comparacion < 0)
    return elemento_buscar(actual->izquierda, elemento, comparador);

  return elemento_buscar(actual->derecha, elemento,comparador);
} 


void* abb_buscar(abb_t* arbol, void* elemento){
  void* elemento_encontrado = NULL;
  if(abb_vacio(arbol)){
    perror(" ERROR! El árbol está vacío, no se puede buscar");
    return NULL;
  }

  if(arbol->comparador == NULL){
    perror(" ERROR! El árbol no tiene comparador, no se pueden buscar elementos");
    return NULL;
  }

  elemento_encontrado = elemento_buscar(arbol->nodo_raiz, elemento, arbol->comparador);
  return elemento_encontrado;
}


bool abb_vacio(abb_t* arbol){
  bool vacio = false;

  if(arbol == NULL)
    vacio = true;
  else if(arbol->tamanio == 0)
    vacio = true;

  return vacio;
}


size_t abb_tamanio(abb_t *arbol){
  size_t tamanio = NINGUNO;

  if(arbol == NULL){
    perror(" ERROR! El arbol es NULL");
    return tamanio;
  }

  tamanio = arbol->tamanio;
  return tamanio;
}


/* 
 * PRE: -
 * POS: -
 */ 
void destructor_vacio (void* a){
  return;
}

/* 
 * PRE: El destructor sea válido.
 * POS: Destruye los nodos del árbol recorriendo postorden y aplica el destructor enviado al 
 *      elemento de cada nodo.
 */ 
void nodos_destruir_con_destructor(nodo_abb_t* actual, void (*destructor)(void *)){
  if(actual == NULL){
    return;
  }

  nodos_destruir_con_destructor(actual->izquierda, destructor);
  nodos_destruir_con_destructor(actual->derecha, destructor);

  destructor(actual->elemento);
  free(actual);
}


void abb_destruir(abb_t *arbol){
  if(arbol == NULL){
    perror(" ERROR! El árbol es NULL, no se puede destruir");
    return;
  }

  nodos_destruir_con_destructor(arbol->nodo_raiz, destructor_vacio); //Envío un destructor que no hace nada
  free(arbol);
}


void abb_destruir_todo(abb_t *arbol, void (*destructor)(void *)){
  if(arbol == NULL){
    perror(" ERROR! El árbol es NULL, no se puede destruir");
    return;
  }

  if(destructor == NULL){
    perror(" ERROR! El destructor enviado es NULL, no se pueden destruir los elementos");
    return;
  }

  nodos_destruir_con_destructor(arbol->nodo_raiz, destructor);
  free(arbol);
}


/*
 * Pre: -
 * Pos: Recorre el árbol en preorden y actualiza la estructura por puntero (datos), en la que se guardará
 *      la cantidad de veces aplicada la función hasta devolver false o al no tener más elementos en el árbol.
 */
void abb_con_cada_elemento_preorden(nodo_abb_t* actual, abb_datos_recorrido_t* datos, bool (*funcion)(void*, void*), void* aux){
  if(!actual || !datos->puedo_recorrer){
    return;
  }

  datos->puedo_recorrer = funcion(actual->elemento, aux);
  datos->recorridos ++;
  abb_con_cada_elemento_preorden(actual->izquierda, datos, funcion, aux);
  abb_con_cada_elemento_preorden(actual->derecha, datos, funcion, aux);
}


/*
 * Pre: -
 * Pos: Recorre el árbol en inorden y actualiza la estructura por puntero (datos), en la que se guardará
 *      la cantidad de veces aplicada la función hasta devolver false o al no tener más elementos en el árbol.
 */
void abb_con_cada_elemento_inorden(nodo_abb_t* actual, abb_datos_recorrido_t* datos, bool (*funcion)(void*, void*), void* aux){
  if(!actual || !datos->puedo_recorrer){
    return;
  }
  
  abb_con_cada_elemento_inorden(actual->izquierda, datos, funcion, aux);
  if(datos->puedo_recorrer){
    datos->puedo_recorrer = funcion(actual->elemento, aux);
    datos->recorridos ++;
  }
  abb_con_cada_elemento_inorden(actual->derecha, datos, funcion, aux);

}


/*
 * Pre: -
 * Pos: Recorre el árbol en postorden y actualiza la estructura por puntero (datos), en la que se guardará
 *      la cantidad de veces aplicada la función hasta devolver false o al no tener más elementos en el árbol.
 */
void abb_con_cada_elemento_postorden(nodo_abb_t* actual, abb_datos_recorrido_t* datos, bool (*funcion)(void*, void*), void* aux){
  if(actual == NULL || datos == NULL|| datos->puedo_recorrer == false){
    return;
  }

  abb_con_cada_elemento_postorden(actual->izquierda, datos, funcion, aux);
  abb_con_cada_elemento_postorden(actual->derecha, datos, funcion, aux);
  if(datos->puedo_recorrer){
    datos->puedo_recorrer = funcion(actual->elemento, aux);
    datos->recorridos ++;
  }
}


size_t abb_con_cada_elemento(abb_t *arbol, abb_recorrido recorrido, bool (*funcion)(void *, void *), void *aux){
  abb_datos_recorrido_t datos;

  //PREGUNTAR SI SE DEBE CHEQUEAR EL RECORRIDO
  if(arbol == NULL || funcion == NULL){
    return NINGUNO;
  }

  datos.recorridos = NINGUNO;
  datos.puedo_recorrer = true;

  switch (recorrido){
    case PREORDEN:
      abb_con_cada_elemento_preorden(arbol->nodo_raiz ,&datos ,funcion, aux);
      break;
    case INORDEN:
       abb_con_cada_elemento_inorden(arbol->nodo_raiz ,&datos ,funcion,aux);
      break;
    case POSTORDEN:
      abb_con_cada_elemento_postorden(arbol->nodo_raiz ,&datos ,funcion,aux);
      break;
  }
  
  return datos.recorridos;
}

/*
 * PRE: El primer parámetro recibe un puntero al elmento que se guardará.
        El segundo parámetro recibe un puntero a la estructura que contiene el arreglo
 * POS: Se guardará en el array el elemento enviado y devolverá true.
        Si el array alcanzó su tamaño máximo, devolverá false. Sin añadirse el elemento.
        Si el segundo parámetro es NULL, se devolverá false.
 */
bool guardar_en_array(void* elemento, void* vector){
  datos_array_t* datos = vector;

  if(datos == NULL)
    return false;
  else if (datos->cantidad_elementos >= datos->tamanio_maximo)
    return false; //Se llegó al tamaño máximo del array


  datos->array[datos->cantidad_elementos] = elemento;
  datos->cantidad_elementos++;
  return true;
}


size_t abb_recorrer(abb_t* arbol, abb_recorrido recorrido, void** array, size_t tamanio_array){
  datos_array_t datos_array;
  if(arbol == NULL || array == NULL){
    return NINGUNO;
  }

  datos_array.array = array;
  datos_array.tamanio_maximo = tamanio_array;
  datos_array.cantidad_elementos = NINGUNO;

  abb_con_cada_elemento(arbol, recorrido, guardar_en_array, &datos_array); //Se carga el array

  return datos_array.cantidad_elementos;
}

