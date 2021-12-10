#include "split.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "string_aux.h"

#define ERROR 0

const char FIN = '\0';

/*
 * PRE: El string no sea NULL
 * POS: Devuelve la cantidad de substrings en las que se puede fraccionar el 
 *      string original según el separador.
 */
size_t contar_substrings(const char* string, size_t tamanio_string, char separador){

    if(string == NULL){
        perror("ERROR: El string es NULL");
        return ERROR;
    }

    size_t cantidad = 1;

    for( int i = 0; i < tamanio_string; i++){
        if(string[i] == separador){
            cantidad++;
        }
    }
    return cantidad;
}


/*
 * PRE: String inicializados.
 * POS: Devuelve la cantidad de carácteres que hay en el string hasta encontrar un separador.
 */
size_t tamanio_hasta_separador(const char *string, size_t tamanio_string, char separador, size_t posicion_actual){
    
    size_t tamanio_substring = 0;

    while(posicion_actual < tamanio_string && string[posicion_actual] != separador){
        posicion_actual++;
        tamanio_substring++;
    }

    return tamanio_substring;
}


/*
 * PRE: Strings NULL's o inicializados.
 * POS: Copia en el string destino un string con cantidad de caracteres "cantidad_a_copiar" del string 
 *      origen a partir de la posición "desde", devolviendo este nuevo string.
 *      Si el string destino u origen es NULL devuelve NULL
 */
char* string_copiado (char* destino, const char* origen, size_t desde, size_t cantidad_a_copiar){
    if (!destino){
        perror("ERROR: el string destino es NULL\n");
        return NULL;
    }

    if(!origen){
        perror("ERROR: El string origen es NULL\n");
        return NULL;
    }
    
    memcpy(destino, &(origen[desde]), cantidad_a_copiar);
    destino[cantidad_a_copiar] = FIN;
    
    return destino;
}


char** split(const char* string, char separador){
    
    size_t i = 0, j = 0;
    size_t cantidad_substrings = 0;
    size_t tamanio_substring = 0;
    size_t tamanio_string = 0;
    char** substrings = NULL;
    char* substring = NULL;

    if(string) {

        tamanio_string = strlen(string);
        cantidad_substrings = contar_substrings(string,tamanio_string, separador);
        substrings = malloc(sizeof(char*)*(cantidad_substrings+1));

        while((j < cantidad_substrings) && substrings != NULL){
            tamanio_substring = tamanio_hasta_separador(string,tamanio_string, separador, i);
            substring = malloc(sizeof(char)*(tamanio_substring + 1));

            if(!substring){
               liberar_substrings(substrings);
            } else {
                substring = string_copiado(substring, string, i, tamanio_substring);

                if(!substring){
                    perror("ERROR copiando el substring. \n");
                    liberar_substrings(substrings);
                    free(substrings);

                } else {
                    substrings[j] = substring;
                }
            }

            i+= (tamanio_substring + 1);
            j++;
        }
        substrings[j] = NULL;
    }
    return substrings;
}