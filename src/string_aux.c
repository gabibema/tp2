#include "string_aux.h"
#include <stdio.h>
#include <stdlib.h>

void liberar_substrings(char** lista_strings){
    size_t i = 0;
    if(lista_strings != NULL){
        while(lista_strings[i] != NULL){
            free(lista_strings[i]);
            i++;
        }
    }
}