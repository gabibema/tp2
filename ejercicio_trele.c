#include <stdio.h>

void mostrar_subgrupos_aux(const char * string, char separador, int posicion){
    if(string[posicion] == '\0'){
        return;
    } else if (string[posicion] == separador){
        mostrar_subgrupos_aux(string, separador, posicion + 1);
        printf("%c", string[posicion]);
    } 
}

void mostrar_subgrupos(const char* string, char separador){
    mostrar_subgrupos_aux(string, separador, 0);
}


int main(){
    mostrar_subgrupos("hola;hola;123", ';');
    return 0;
}



