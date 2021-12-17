#include <stdbool.h>
#include <stdio.h>
#include <time.h> 
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "src/lista.h"
#include "src/hospital.h"
#include "src/simulador.h"
#include "src/dificultades.h"
#include "src/pokemones.h"
#include "src/graficos.h"
#include "src/struct.h"
#include "src/colors.h"

#define ERROR -1

typedef struct{
    simulador_t* simulador;
    bool animaciones;
    bool jugando;
}Juego;

int leer_numero(){
    char linea[100];
    char* leido;
    leido = fgets(linea, 100, stdin);
    if(!leido)
        return 0;
    while(*leido == ' ')
        leido++;
    return atoi(leido);
}


char leer_comando(){
    char linea[100];
    char* leido;
    leido = fgets(linea, 100, stdin);
    if(!leido)
        return 0;
    while(*leido == ' ')
        leido++;
    return (char)tolower(*leido);
}

#define EMPEZAR 'a'
#define VER_ESTADISTICAS 'e'
#define ATENDER_ENTRENADOR 'p'
#define INFORMACION_POKEMON 'i'
#define ADIVINAR_NIVEL 'a'
#define AGREGAR_DIFICULTAD 'n'
#define SELECCIONAR_DIFICULTAD 'd'
#define INFORMACION_DIFICULTAD 'o'
#define FINALIZAR 'q'
#define VOLVER_ATRAS 'b'

void inicializar_juego(Juego* juego, hospital_t* hospital){
    juego->simulador = simulador_crear(hospital);
    juego->animaciones = true;
    juego->jugando = true;
}

//Loop o volver hacia
void agregar_espacios(char* string, int tamanio){
    if(!string) return;

    int espacios = tamanio - (int)strlen(string) - 1;
    for(int i = 0; i < espacios; i ++) strcat(string," ");

    strcat(string, "║");
}

#define MENSAJE 51
void mostrar_estadisticas(Juego* juego){
    EstadisticasSimulacion e;
    simulador_simular_evento(juego->simulador, ObtenerEstadisticas, &e);
    char mensaje[MENSAJE];

    char opcion;
    do{
        system("clear");
        printf("    ╔═══════════════════◓═════════════════════╗\n");
        printf("    ║                                         ║\n");
        sprintf(mensaje,"    ║   ➤ Cantidad de Eventos Simulados: %i", e.cantidad_eventos_simulados);
        agregar_espacios(mensaje, MENSAJE);
        printf("%s\n",mensaje);

        sprintf(mensaje,"    ║   ➤ Entrenadores Atendidos: %i", e.entrenadores_atendidos);
        printf("    ║                                         ║\n");
        agregar_espacios(mensaje, MENSAJE);
        printf("%s\n",mensaje);

        sprintf(mensaje,"    ║   ➤ Pokemones Atendidos: %i", e.pokemon_atendidos);
        printf("    ║                                         ║\n");
        agregar_espacios(mensaje, MENSAJE);
        printf("%s\n",mensaje);

        sprintf(mensaje,"    ║   ➤ Pokemones en Espera: %i", e.pokemon_en_espera);
        printf("    ║                                         ║\n");
        agregar_espacios(mensaje, MENSAJE);
        printf("%s\n",mensaje);
        
        sprintf(mensaje,"    ║   ➤ Pokemones en Espera: %i", e.pokemon_en_espera);
        printf("    ║                                         ║\n");
        agregar_espacios(mensaje, MENSAJE);
        printf("%s\n",mensaje);

        sprintf(mensaje,"    ║   ➤ Entrenadores Totales: %i", e.entrenadores_totales);
        printf("    ║                                         ║\n");
        agregar_espacios(mensaje, MENSAJE);
        printf("%s\n",mensaje);

        sprintf(mensaje,"    ║   ➤ Pokemones Totales: %i", e.pokemon_totales);
        printf("    ║                                         ║\n");
        agregar_espacios(mensaje, MENSAJE);
        printf("%s\n",mensaje);

        sprintf(mensaje,"    ║   ➤ Cantidad de Puntos: %i", e.puntos);
        printf("    ║                                         ║\n");
        agregar_espacios(mensaje, MENSAJE);
        printf("%s\n",mensaje);
        printf("    ║                                         ║\n");
        printf("    ╚═══════════════════◓═════════════════════╝\n");
        printf("     [ Para Volver al Menu Ingresa (B) ]\n");
        printf(" ➤ ");
        opcion = leer_comando();
    } while(opcion != VOLVER_ATRAS);

}


bool mostrar_pokemones(void* poke, void* cont){
    pokemon_t* pokemon = poke;
    int* contador = cont;

    char string[51];
    sprintf(string, " ║   Pokemon N°%i : %s", *contador, pokemon->nombre);
    agregar_espacios(string, 51);
    printf("%s\n", string);
    printf(" ║                                             ║\n");

    (*contador)++;
    return true;
}

void mostrar_entrenador_nombre(char* nombre){
    char string[50];
    sprintf(string, " ║   Entrenador: %s", nombre);
    agregar_espacios(string, 50);
    printf("%s\n", string);
    printf(" ║                                             ║\n");   

}

void mostrar_entrenador(Juego* juego){
    entrenador_t* entrenador = lista_iterador_elemento_actual(juego->simulador->iterador_entrenadores);
    if(!entrenador){
        printf(" [ No hay más entrenadores para atender ]\n");
        sleep(2);
        return;
    }
    
    if (simulador_simular_evento(juego->simulador, AtenderProximoEntrenador, NULL) == ErrorSimulacion) return;

    char opcion;
    int contador = 1;
    do{
        system("clear");
        contador = 1;
        printf(" ╔═══════════════════◓═════════════════════════╗\n");
        printf(" ║                                             ║\n");
        mostrar_entrenador_nombre(entrenador->nombre);
        abb_con_cada_elemento(entrenador->pokemones,INORDEN, mostrar_pokemones, &contador);
        printf(" ╚═══════════════════◓═════════════════════════╝\n");
        printf("     [ Para Volver al Menu Ingresa (B) ]\n");
        printf(" ➤ ");
        opcion = leer_comando();
    } while(opcion != VOLVER_ATRAS);
}

void mostrar_poke_a(Juego* juego){
    if(!juego) return;

    InformacionPokemon info;
    char mensaje[50];

    if (simulador_simular_evento(juego->simulador, ObtenerInformacionPokemonEnTratamiento, &info) == ErrorSimulacion) return;
    if(!info.nombre_pokemon || !info.nombre_entrenador) return;

    char opcion;
    do{
        system("clear");
        printf(" ╔═══════════════════◓═════════════════════════╗\n");
        printf(" ║                                             ║\n");
        mostrar_entrenador_nombre((char*)info.nombre_entrenador);
        sprintf(mensaje, " ║   Pokemon: %s", info.nombre_pokemon);
        agregar_espacios(mensaje, 50);
        printf("%s\n", mensaje);
        printf(" ║                                             ║\n");   
        printf(" ╚═══════════════════◓═════════════════════════╝\n");
        printf("     [ Para Volver al Menu Ingresa (B) ]\n");
        printf(" ➤ ");
        opcion = leer_comando();
    } while(opcion != VOLVER_ATRAS);
}

void intentar_adivinar(Juego* juego){
    if(!juego) return;

    Intento intento;

    if(!juego->simulador->paciente.pokemon){
        system("clear");
        printf("═══════════════════════◓══════════════════════\n");
        printf("    No hay pokemones en la sala de espera :(\n");
        printf("═══════════════════════◓══════════════════════\n");
        sleep(2);
        return;
    }

    system("clear");
    printf("═══════════════════◓══════════════════\n");
    printf("  El Pokémon a atender es: %s\n", juego->simulador->paciente.pokemon->nombre);
    printf("═══════════════════◓══════════════════\n");
    intento.nivel_adivinado = (unsigned)leer_numero();

    if (simulador_simular_evento(juego->simulador, AdivinarNivelPokemon, &intento) == ErrorSimulacion) return;


    char opcion;
    do{
        system("clear");
        printf("═══════════════════◓══════════════════\n");
        printf("  %s\n", intento.resultado_string);
        printf("═══════════════════◓══════════════════\n");
        printf("     [ Para Volver al Menu Ingresa (B) ]\n");
        printf(" ➤ ");
        opcion = leer_comando();
    } while(opcion != VOLVER_ATRAS);
}

void aniadir_dificultad(Juego* juego){
    if(!juego) return;

    system("clear");
    printf("═══════════════════◓══════════════════\n");
    printf("  Agregando Dificultad.......\n");
    printf("═══════════════════◓══════════════════\n");
    sleep(1);

    int opcion;
    do{
        system("clear");
        printf("═══════════════════◓══════════════════\n");
        printf("            << Ingresa >>\n");
        printf(" --> (1) Para añadir la dificultad random\n\n");
        printf(" --> (2) Para añadir la dificultad tutorial\n");
        printf("═══════════════════◓══════════════════\n");
        printf("     [ Para Volver al Menu Ingresa (-1) ]\n");
        printf(" ➤ ");
        opcion = leer_numero();
    } while(opcion != -1 && opcion != 1 && opcion != 2);

    if(opcion == -1) return;

    size_t id = opcion == 1 ? 3 : 4;
    DatosDificultad* datos;

    if(id == 3)
        simulador_simular_evento(juego->simulador, AgregarDificultad, (DatosDificultad*)&DIFICULTAD_RANDOM);
    else if (lista_tamanio(juego->simulador->lista_dificultades) == 4)
        simulador_simular_evento(juego->simulador, AgregarDificultad, (DatosDificultad*)&DIFICULTAD_TUTORIAL);
    
    datos = lista_elemento_en_posicion(juego->simulador->lista_dificultades, id);
    char opcion_1;
    do{
        system("clear");
        printf("═══════════════════◓══════════════════\n");
        printf("  Se Agrego la dificultad: \n");
        printf("  ➤➤ %s\n", datos->nombre);
        printf("═══════════════════◓══════════════════\n");
        printf("     [ Para Volver al Menu Ingresa (B) ]\n");
        printf(" ➤ ");
        opcion_1 = leer_comando();
    } while(opcion_1 != VOLVER_ATRAS);

}


bool mostrar_dificultades(void* dif, void* pos){
    if(!dif || !pos) return false;

    DatosDificultad* dificultad = dif;
    size_t* posicion = pos;

    printf("  ➤➤ %s : ID [%li]\n\n", dificultad->nombre, *posicion);
    (*posicion)++;
    return true;
}

void cambiar_dificultad(Juego* juego){
    if(!juego) return;

    system("clear");
    printf("═══════════════════◓══════════════════\n");
    printf("  Las Dificultades Elegibles son:\n");
    printf("═══════════════════◓══════════════════\n");

    size_t posicion = 0;
    lista_con_cada_elemento(juego->simulador->lista_dificultades, mostrar_dificultades, &posicion);
    printf("═════════════════════════════════════\n");
    int id = 0;
    char opcion;


    printf("[ Ingrese el ID al que Desea Cambiar ]\n");
    printf("[ Para Volver al Menu Ingresa (-1) ]\n");

    id = leer_numero();
    if(id == -1) return;

    simulador_simular_evento(juego->simulador, SeleccionarDificultad, &id);

    do{
        system("clear");
        printf("═══════════════════◓══════════════════\n");
        printf("  Se Cambió a la Dificcultad: \n");
        printf("  ➤➤ %s\n", juego->simulador->dificultad_actual->nombre);
        printf("═══════════════════◓══════════════════\n");
        printf("     [ Para Volver al Menu Ingresa (B) ]\n");
        printf(" ➤ ");
        opcion = leer_comando();
    } while(opcion != VOLVER_ATRAS);


}

void mostrar_dificultad(Juego* juego){
    if(!juego) return;

    InformacionDificultad info;

    printf("\n\n");
    printf("═══════════════════◓══════════════════\n");
    printf("  [Ingrese el ID que Desea Ver] \n\n");
    printf("[ Para Volver al Menu Ingresa (-1) ]\n");
    printf("═══════════════════◓══════════════════\n\n");
    int id = leer_numero();
    if(id == -1) return;

    info.id = id;
    simulador_simular_evento(juego->simulador, ObtenerInformacionDificultad, &info);

    do{
        system("clear");
        if (info.id == -1){
            printf("\n\n");
            printf("\t═══════════════════◓══════════════════\n");
            printf("\t  El ID ingresado no existe... \n\n");
            printf("\t═══════════════════◓══════════════════\n\n");
        } else {
            printf("\t═══════════════════◓══════════════════\n");
            printf("\t  -> [ Nombre: %s ] \n\n", info.nombre_dificultad);
            if(info.en_uso){
                printf("\t  -> [ En Uso: Sí ]\n\n");
            } else {
                printf("\t  -> [ En Uso: No ]\n\n");
            }
            printf("\t  -> [ ID: %i]\n", info.id);
            printf("\t═══════════════════◓══════════════════\n\n");
        }
        printf("\t[ Para Volver al Menu Ingresa (-1) ]\n");
        id = leer_numero();
    } while (id != -1);
}

void finalizar_juego(Juego* juego){
    system("clear");
    
    EstadisticasSimulacion e;
    simulador_simular_evento(juego->simulador, ObtenerEstadisticas, &e);
    char mensaje[MENSAJE];

    printf("       ------> Estadísticas Finales <--------\n");
    printf("    ╔═══════════════════◓═════════════════════╗\n");
    printf("    ║                                         ║\n");
    sprintf(mensaje,"    ║   ➤ Cantidad de Eventos Simulados: %i", e.cantidad_eventos_simulados);
    agregar_espacios(mensaje, MENSAJE);
    printf("%s\n",mensaje);

    sprintf(mensaje,"    ║   ➤ Entrenadores Atendidos: %i", e.entrenadores_atendidos);
    printf("    ║                                         ║\n");
    agregar_espacios(mensaje, MENSAJE);
    printf("%s\n",mensaje);

    sprintf(mensaje,"    ║   ➤ Pokemones Atendidos: %i", e.pokemon_atendidos);
    printf("    ║                                         ║\n");
    agregar_espacios(mensaje, MENSAJE);
    printf("%s\n",mensaje);

    sprintf(mensaje,"    ║   ➤ Pokemones en Espera: %i", e.pokemon_en_espera);
    printf("    ║                                         ║\n");
    agregar_espacios(mensaje, MENSAJE);
    printf("%s\n",mensaje);
    
    sprintf(mensaje,"    ║   ➤ Pokemones en Espera: %i", e.pokemon_en_espera);
    printf("    ║                                         ║\n");
    agregar_espacios(mensaje, MENSAJE);
    printf("%s\n",mensaje);

    sprintf(mensaje,"    ║   ➤ Entrenadores Totales: %i", e.entrenadores_totales);
    printf("    ║                                         ║\n");
    agregar_espacios(mensaje, MENSAJE);
    printf("%s\n",mensaje);

    sprintf(mensaje,"    ║   ➤ Pokemones Totales: %i", e.pokemon_totales);
    printf("    ║                                         ║\n");
    agregar_espacios(mensaje, MENSAJE);
    printf("%s\n",mensaje);

    sprintf(mensaje,"    ║   ➤ Cantidad de Puntos: %i", e.puntos);
    printf("    ║                                         ║\n");
    agregar_espacios(mensaje, MENSAJE);
    printf("%s\n",mensaje);
    printf("    ║                                         ║\n");
    printf("    ╚═══════════════════◓═════════════════════╝\n");

    juego->jugando = false;
}

void ejecutar_comando(Juego *juego, char comando){
    switch (comando) {
        case VER_ESTADISTICAS:
            mostrar_estadisticas(juego);
            break;
        case ATENDER_ENTRENADOR:
            mostrar_entrenador(juego);
            break;
        case INFORMACION_POKEMON: 
            mostrar_poke_a(juego);
            break;
        case ADIVINAR_NIVEL:
            intentar_adivinar(juego);
            break;
        case AGREGAR_DIFICULTAD:
            aniadir_dificultad(juego);
            break;
        case SELECCIONAR_DIFICULTAD:
            cambiar_dificultad(juego);
            break;
        case INFORMACION_DIFICULTAD:
            mostrar_dificultad(juego);
            break;
        case FINALIZAR:
            finalizar_juego(juego);
            break;
        default:
            printf(" [ La Opción Ingresada no es Válida ]\n");
            sleep(2);
            break;

    }
}

void destruir_juego(Juego* juego){
    simulador_destruir(juego->simulador);
}

// #define BLKB "\033[40m"
// #define REDB "\033[41m"
// #define GRNB "\033[42m"
// #define YELB "\033[43m"
// #define BLUB "\033[44m"
// #define MAGB "\033[45m"
// #define CYNB "\033[46m"
// #define WHTB "\033[47m"

void pantalla_carga(const char* color, int pokemon, int n_color){
    if(pokemon == 0){
        printf("%s\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⠄⠀⠒⠒⠒⠂⠄⠀⠀⠀⠀⠀⢀⠠⠤⠀⠤⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣐⣊⣀⣀⣀⣀⠀⠀⠀⣀⠤⢄⠪⠖⠁⢰⡔⠡⠠⡄⠀⠀⠀⠀⠀⠁⠢⢀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠐⠀⠀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠉⠐⢪⠑⠁⠑⠊⠁⠀⠑⠂⠐⡰⠂⠀⠀⠀⠀⠀⠀⠀⠁⠐⠀⠄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠂⠀⠀⠀⠀⠀⠀⠠⠠⠐⠈⠁⠀⠀⠀⠀⠀⠀⠀⠁⠀⠀⠀⠀⠠⠒⠊⠉⠉⠉⠉⠁⠐⠂⠄⠀⣀⠀⠀⠀⠀⠀⠐⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⠁⠀⠀⠀⠠⠠⠒⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣾⣾⣶⣤⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠂⢄⠀⠀⠀⠀⠀⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⢀⠀⠊⡠⠊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢴⣿⣿⣿⣿⣿⣿⣿⣿⡗⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠢⡤⠀⠀⠐⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠓⠀⠀⡔⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢂⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠆⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠂⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠂⠃⢀⠀⢀⣥⣤⣤⣾⣾⠉⠀⠉⠉⠉⠏⣿⣿⣿⣿⣥⣄⣤⠄⡁⠀⠀⢀⡀⠀⡀⠄⠐⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡀⠀⠀⠠⢄⠰⣀⡤⢰⡖⠐⣴⣤⣼⣿⣿⣿⣿⣿⣿⣦⡞⢣⡞⢓⣿⣿⣿⣿⣧⣿⣿⣿⣿⣦⣧⣤⣾⣆⡁⣤⡀⠠⣄⡀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⢀⠸⠄⠀⠁⠉⠂⠀⣉⣽⣦⣾⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⣶⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡂⠀⠽⠓⠀⡦⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⢀⠔⠂⠀⠐⠀⠀⠀⠀⠆⠛⢛⣛⣿⣧⣯⣏⣉⠙⠛⠛⠛⠿⠿⣁⡠⠔⠂⠉⠛⠉⢸⠿⠆⠘⠁⣀⡀⠙⢟⠛⠋⠛⠛⠉⣩⣏⣇⣥⣿⣿⡿⡍⠘⠃⠶⠠⠞⠋⠄⠄⡀⠀⠀⠀⠀⠀\n\
⠀⠀⢀⠀⠔⠈⠁⠀⠀⠀⢰⠂⠀⠀⠈⢸⠿⣿⣿⣿⣿⣶⣄⠀⠀⠀⠘⣉⣠⣤⠔⠀⠀⠀⣘⠀⠀⠀⠀⠀⠙⠣⡞⠀⠀⢀⣰⡿⣟⡨⢡⣿⡇⠩⠃⠀⠀⠈⢢⡀⠀⠀⠀⢀⣀⠁⠐⠂⡀⠀\n\
⠀⠀⠁⢀⡀⠀⡠⠆⠀⠠⠱⠀⢀⠀⢠⣿⠀⢛⣿⣿⣧⣂⡹⠛⠆⠀⠈⠙⡋⠀⣀⢤⠆⠀⠀⠀⠀⠀⢦⡀⢦⡴⠀⠀⠀⠙⠋⢤⢢⣿⣿⣿⣉⣀⣳⣆⠀⣦⡀⠱⢆⠀⠠⡄⠈⠋⠆⠊⠀⠀\n\
⠀⠀⠀⠨⠀⠀⠊⠀⠠⠂⡧⣤⣾⣶⣾⣿⣿⣿⣿⣿⠋⠀⠀⢀⠀⠀⠀⠀⠘⠃⡟⢁⡠⠀⠀⠀⠀⡄⣀⡏⠉⠀⠀⠤⠄⡀⠀⠀⠈⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⡖⠁⠁⠤⠵⠅⠀⠂⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣥⣿⡿⢿⣿⠏⠁⢈⠍⠀⠀⠀⠐⠅⡀⠀⠀⠀⠀⠀⠀⠸⡁⠀⠼⠄⡀⠘⠁⠀⢀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠹⣿⣿⢿⠛⢿⣿⣏⣀⠈⠑⡄⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠐⠘⠛⠃⠉⣼⠃⠀⢀⠆⠀⠀⠀⠀⠀⠀⠈⢀⠦⡀⠀⠀⠀⠀⠈⠙⠃⠈⠀⠀⠀⠀⢠⣀⠂⠣⠄⠀⠃⠀⠀⠀⠀⠘⢀⠀⠀⠈⣿⣿⣤⣤⣤⡱⡄⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⢸⣶⣶⠿⠿⣟⠀⠀⠈⣄⠀⠀⠀⠀⠀⠁⠄⠀⠐⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠂⠐⠀⠀⠀⣠⠀⠀⣠⠃⠀⠀⠀⣿⢛⣋⣉⣁⣀⡧⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⢓⣤⣞⠀⣄⡿⠀⠀⠀⠐⢀⠀⠀⡒⢰⠄⠤⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⢀⡀⢤⣤⣆⢐⠒⠉⡀⣀⡴⠏⠀⠀⠀⠠⣿⠟⠋⠉⣉⣾⡇⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣿⣿⣿⣷⠀⠀⠀⠀⠀⠉⠃⠗⣄⠘⣾⣟⠀⠀⠉⠉⠐⠄⣤⠀⠀⠀⣄⠤⠊⠈⠀⠀⠀⢹⣿⠊⣠⡿⠘⠁⠀⠀⠀⠀⠀⠰⣿⣀⣤⣼⣿⠿⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⣿⣦⠀⠀⠀⠀⠀⠀⠀⠐⢄⠸⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⡾⠟⡰⠂⠀⠀⠀⠀⠀⠀⠀⢠⡴⣿⣿⣿⣿⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠊⣻⣿⡿⢿⣧⠀⠁⠀⠀⠀⢀⣀⣎⠳⢄⡙⠏⡆⢂⡀⣀⣀⣀⣀⢀⣀⣀⡀⡐⢄⡹⠃⣠⢾⣴⡶⠾⠷⠓⠂⠐⠞⣀⣨⡼⣿⣿⣿⠫⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠁⠈⠉⠱⣒⠂⠀⠐⢙⠋⠀⠀⠀⣹⡆⠠⡙⠉⠿⠿⠿⠏⠽⠿⠿⠿⠗⢊⣤⡚⠁⡀⢼⠿⠚⠛⠛⠁⠀⢀⣀⠜⠁⠘⠉⠀⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣷⣵⣽⣶⣾⣷⡿⡿⠁⠀⠀⠈⠉⠉⠉⠀⠀⠀⠈⠉⠉⢠⣟⡿⠛⠀⠁⠀⠰⣶⣾⣿⡿⣫⠟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⢛⠙⡿⠿⠁⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣨⣁⣀⣦⣶⡿⣿⣿⣉⣁⡤⠔⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢾⣿⠝⠉⢿⣿⣗⠋⠻⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠊⠉⠉⠢⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n", color);
    } else if (pokemon == 1){
        printf("%s\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠤⠰⠃⠀⠀⠀⠀⠀⠀⠈⠀⠀⠊⠀⠀⠀⠂⢐⡲⡆⣄⢄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠃⣈⠘⠁⠢⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⠀⠀⢁⣀⣤⣶⣷⣷⣦⡀⠀⠀⠀⠀⠀⠀⠀⠐⠤⠔⠓⠫⠕⠦⣄⡀⠈⠐⢬⡀⢱⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⠀⣰⣶⣶⣄⡀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠂⠀⠀⣠⣤⣾⣿⣿⣿⣿⣿⣿⣿⣇⠈⠄⠀⠀⠀⠀⠀⠀⢉⠂⠀⠀⠀⠀⠈⠙⠂⠬⣦⠀⠈⣀⣐⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⠀⣸⣿⣿⣿⣿⣿⣤⡀⠂⢀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡄⠨⠀⠀⠀⠀⠀⠀⠀⠁⠒⠚⢏⠉⠀⠀⠀⠀⠈⠀⠀⡟⠛⠄⢀⢀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠀⣰⣿⣿⣿⣿⣿⣿⣿⣿⣦⡀⠁⠀⢀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⢃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⡀⠐⠲⣤⣀⣤⠀⢣⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠂⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⣀⠀⠀⠀⠂⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠁⠆⠲⠾⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠄⠀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣦⣄⠀⠁⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠱⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠀⠀⠀⠀⠀⢀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⠑⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠆⠀⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⡀⠀⠀⠁⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⠀⠀⠀⠀⠀⠈⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡄⠠⡀⠀⠀⠀\n\
⠀⠀⠀⠐⠀⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣶⣄⡀⠈⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠁⠀⠀⠀⠀⠀⠀⢀⠄⠀⠀⠀⠀⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⢁⠀⠀⠀\n\
⠀⠀⢀⠂⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣤⡀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⢀⠀⠀⠀⠀⠀⠂⠀⣀⣤⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⠣⠀⠀\n\
⠀⠀⠂⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⡄⠁⠀⠑⡀⠃⠀⠀⠀⠀⠀⠈⠀⠔⠀⠀⠀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⠄⠀\n\
⠀⠰⢀⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⠛⠋⠉⠙⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠧⣀⡠⠊⠀⠀⠀⠀⠀⠛⠂⠀⠑⣄⠤⠼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣇⠈⠀\n\
⠀⠃⣼⣿⣿⣿⣿⡿⠟⠋⠁⠀⠀⠀⠀⠀⠀⠀⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⠿⠿⠿⠿⠛⢫⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⠻⠿⠻⠿⠿⠿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁⠀⠀⠀⠉⠛⠿⣿⣿⣿⣿⣿⣿⣿⣿⡄⢁\n\
⢀⢰⣿⣿⣿⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣿⣿⣿⣿⣿⣿⣿⠿⠿⣿⠋⠀⠀⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⣀⡀⠀⠀⠀⠀⠀⠀⠈⠙⢿⣿⣿⣿⡿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠿⣿⣿⣿⣿⣿⠀⠀\n\
⡀⢸⣿⡿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⡿⠟⠁⠀⠀⠀⠁⠀⠀⠀⣸⣿⣿⡿⣿⣿⡿⠙⠂⠀⠀⠀⠀⠀⠀⣀⠀⠀⣀⠀⠀⠀⠀⢻⣿⡿⠿⣿⣷⣷⡀⠀⠀⠀⠘⠿⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣿⣿⣿⡀⢀\n\
⡇⣼⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⠟⠁⠀⣀⠄⠀⠀⠀⠀⠀⣸⣿⠛⠉⠀⠈⠏⠀⠀⠀⠀⠀⠀⡠⠂⠉⠀⠀⠀⠀⠈⠀⠢⠀⠀⡏⠀⠀⠈⠻⣿⡟⠢⡀⠀⠀⠀⠀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⣿⠇⡘\n\
⠇⡿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⡜⠿⠁⠀⠀⠀⡐⠀⠀⠀⠀⠀⢀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢄⠘⠀⠀⠀⠀⠈⠀⠀⠱⡀⠀⠀⠀⠀⡀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⣿⠀⠀\n\
⠀⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢄⢀⢲⠂⠀⡀⠀⠀⠁⠀⠀⠀⠀⢀⠀⠀⠀⠀⠀⠠⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢡⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠂⠂⡀⢡⠈⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⡏⡘⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⢃⢀⡊⠀⢀⠀⠀⠀⠀⠀⠀⡆⠀⠀⠀⠀⠠⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⠀⠀⠀⠀⠀⠀⠀⠑⠐⠄⠐⠓⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠁⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠂⠐⠒⢁⠀⠀⠀⠀⠂⠀⠀⠀⠀⠀⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⡗⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⠂⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡉⠈⠀⠀⠀⠀⠀⠀⠀⢉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠃⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⠊⠀⠉⠁⠚⠓⠒⠂⠉⢈⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡱⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⠁⠀⠀⠀⠀⠀⠀⠀⠠⠀⠀⠀⠀⠀⢀⠀⠐⡠⠊⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡘⠀⠀⠀⠀⠀⠀⠀⠄⠀⠀⠀⡶⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡔⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⠀⠀⠈⣀⡠⠊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⡀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣰⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⢖⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡀⢔⠙⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣈⠠⠤⠔⢊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⢗⠒⠂⠐⠒⠂⠛⠈⠁⢨⡇⠀⠀⠀⠀⠀⠀⠀⣀⠔⠁⠀⠈⠐⠂⠒⡀⠄⠀⠀⠒⠤⢂⠁⠀⠀⠉⠒⢄⣀⡀⠀⠀⠀⠀⠀⢠⠎⢀⠠⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⠀⠀⠀⠀⠀⠀⢀⢀⢃⣤⣄⣀⡀⠀⠢⡎⠁⠒⠒⠂⠀⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⢩⠎⠈⠁⠀⢀⣠⣤⡔⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠉⠉⠀⢁⡀⠀⠁⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠱⣀⠀⠀⠀⠀⠀⠀⠀⠀⣈⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠃⠘⡠⠉⠡⠀⠉⢑⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠂⠄⠄⣠⠂⠢⠄⠐⡀⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠂⠂⢠⠀⠂⠀⠐⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠐⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n", color);
    } else {
        printf("%s\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣀⠀⠀⠐⠈⠹⠶⠊⠉⠐⠠⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢐⠠⣯⣄⡀⠀⠀⠀⠀⠀⣤⡤⠀⣀⢀⠀⢰⣬⠀⠀⠤⠄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠄⢹⣿⡿⠀⠀⠀⠀⣖⡷⠄⠀⠉⠁⠀⠀⠀⣀⡀⠀⠀⠈⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⡀⠟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⣉⣁⡀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡄⠀⠀⠀⠀⠀⢠⡆⠀⠀⠀⠀⠀⢀⠀⠰⠀⠁⠀⠀⠘⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣁⣀⣀⣀⠀⠃⠀⠀⠛⠁⠀⠈⠛⠈⠀⠀⠀⠀⠀⠀⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠤⢀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠀⠠⠄⠀⠒⠒⠒⠀⠀⠤⢀⠉⠀⠂⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⢀⠀⠀⠀⠀⠀⠁⡀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡀⢨⡑⢤⣤⣈⠀⡄⠀⠀⠀⠀⠀⠐⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠘⠀⠀⠀⠀⠀⠀⠀⢀⡀⠶⢶⣶⣤⡐⢠⡄⠀⠀⢠⡄⢀⡀⠀⠀⡀⢤⠐⠆⠛⠈⠁⠀⠀⣾⣿⣿⣷⣄⠀⢄⠀⠀⠀⠀⠀⠀⠀⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠖⠈⠀⣰⠛⠛⠀⠁⠀⠀⠀⠀⠀⠀⠀⠙⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⣿⣿⣧⡀⡀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣆⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⢄⢀⡚⠁⠀⣠⠞⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡰⠆⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⣿⣿⠉⠀⠙⠻⠗⠅⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣿⠀⣠⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡿⠐⠀⠀⠀⣀⠀⠀⠀⣀⢠⣿⣿⡟⠀⠀⠀⠀⠀⢃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣤⣀⣀⠀⠀⠀⠀⠀⠀⣸⣿⣿⠃⠐⠧⠀⠀⠂⠛⠠⡀⠀⠀⣀⠀⢤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣿⣿⡿⠁⣠⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⢠⣶⣿⣿⣿⣿⣿⣷⣶⣶⣶⣶⣴⣿⣿⣟⣀⡤⠄⠒⠁⠀⠀⢠⠃⠀⠀⠀⠀⠠⠈⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⠔⣰⢿⣟⠁⠀⠀⠀⠀⠀⠀⠀⢴⣦⠀⠀⣀⣴⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠢⡀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⢘⣠⣿⣟⠀⡀⢰⡦⠀⠒⠀⢀⣤⣈⣴⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⢀⠀⠀⠀⠀⠀⢠⠁⠀⠀⠀⠀⢹⠂⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⡌⢿⣿⣿⣿⣿⣶⣶⣤⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⠀⠀⠀⢸⠀⠀⠀⠀⣰⠃⠀⠀⠀⠀⠀⠀⠀⠈⠠⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠉⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀⠀⠀⢸⡀⣀⡤⠚⠁⠀⠀⠀⠀⠀⠀⣘⠀⠀⢰⡦⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀⠀⠀⠀⢸⢋⠉⠀⠀⠀⠀⠀⠀⠀⠀⡰⠃⠀⠀⣸⠇⢀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠁⠀⠀⠐⠀⠀⡘⠈⠙⠐⠖⠀⠀⠀⠴⣞⣃⣀⢤⠐⠣⠁⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠀⠀⠀⠀⠀⢀⣔⠃⠀⠀⠀⠀⢰⠀⠀⠀⠀⠀⠈⠈⠀⠀⠀⠁⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠁⠀⠀⠀⠀⠀⡌⠀⠈⠄⠀⠀⠀⠀⠀⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠠⠂⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠘⠀⠀⠀⠀⠀⢠⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠐⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠋⠁⠀⠀⠀⠀⠀⠀⠀⡠⠠⡅⠘⠐⢾⠇⠀⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⡀⠀⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⢀⠄⠀⠀⠀⠈⠄⠀⠀⠑⢄⠀⠀⠀⠘⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⢀⠀⠀⠀⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠋⠀⠀⠀⠀⠀⠀⠀⢀⠴⠁⠀⠀⠀⠀⠀⠀⢂⠀⠀⠐⣦⡐⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⢸⠀⠀⠀⠀⠙⣿⣿⣿⡿⠿⠿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠆⠀⠀⠘⡁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⢸⠀⠀⠀⣠⠀⠈⠉⠀⠀⠀⠀⠀⠀⠉⠛⢿⣿⣿⣿⣿⠿⠛⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⠀⠀⠀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠘⣄⠀⣶⣷⠆⠀⠀⡀⠤⠄⠤⠄⣀⠀⠀⠀⠈⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⢿⣦⡿⣿⣿⠆⠀⠀⠀⠀⠀⠀⣸⣷⣦⣄⡀⠀⠀⠀⠀⢀⠀⠀⣄⠀⣤⣤⡀⣤⣾⣿⣿⣿⣿⣆⠀⣀⣀⣀⣀⠀⠀⠀⠀⠀⣇⣀⣠⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠚⠁⠀⡹⠃⠀⠀⠀⠀⠀⢀⣼⣿⣿⣿⣿⣿⡇⠒⠂⠁⠀⠀⠀⠀⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⣤⣾⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⢀⠎⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⣿⣿⣿⡟⠀⠀⠀⠀⠀⣀⠀⠠⠄⠐⠀⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠅⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠈⠁⠀⠀⠀⠀⣠⣴⡿⠋⠉⠀⠛⢿⣿⠏⠉⠉⠉⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡓⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠀⠀⠀⠀⢀⣴⠞⠋⠁⠀⠀⠀⣀⠴⠊⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠄⠉⠀⠀⠉⠛⢿⣿⣿⣿⣿⣿⣿⣿⠏⠀⠑⠀⠀⢀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n\
⠀⠀⠀⠣⠴⠶⠻⠿⠀⠠⠤⠄⠐⠂⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠆⣀⣀⣀⠀⠠⠤⠚⠛⠛⠉⠉⠉⠉⠀⠈⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n", color);

    }

    //printf("Color: %i", n_color);
}


#define VER_ESTADISTICAS 'e'
#define ATENDER_ENTRENADOR 'p'
#define INFORMACION_POKEMON 'i'
#define ADIVINAR_NIVEL 'a'
#define AGREGAR_DIFICULTAD 'n'
#define SELECCIONAR_DIFICULTAD 'd'
#define INFORMACION_DIFICULTAD 'o'
#define FINALIZAR 'q'

void mostrar_opciones(){
printf("\
    ╔═══════════════════◓══════════════════╗\n\
    ║                                      ║  \n\
    ║   ➤ Ver Estadísticas (E)             ║\n\
    ║                                      ║\n\
    ║   ➤ Atender Entrenador (P)           ║\n\
    ║                                      ║\n\
    ║   ➤ Información Pokémon (I)          ║\n\
    ║                                      ║\n\
    ║   ➤ Adivinar Nivel (A)               ║\n\
    ║                                      ║\n\
    ║   ➤ Agregar Dificultad (N)           ║\n\
    ║                                      ║\n\
    ║   ➤ Seleccionar Dificultad (D)       ║\n\
    ║                                      ║\n\
    ║   ➤ Información Dificultad (O)       ║\n\
    ║                                      ║\n\
    ║   ➤ Finalizar (Q)                    ║\n\
    ║                                      ║\n\
    ╚═══════════════════◓══════════════════╝\n\
");
}

void mostrar_pantalla_carga(int pokemon, char* color){
    char opcion;

    do{
        mostrar_pokemon(pokemon, color);
        printf("\n%s     [ Para Iniciar Ingrese (A) ]\n",color);
        printf(" ➤ %s", END_COLOR);
        opcion = leer_comando();
    } while(opcion != EMPEZAR);

}

int main(int argc, char *argv[]) {
    srand ((unsigned)time(NULL));

    /*
    char colores[6][20];
    strcpy(colores[0], RED);
    strcpy(colores[1], GRN);
    strcpy(colores[2], YEL);
    strcpy(colores[3], BBLU);
    strcpy(colores[4], MAG);
    strcpy(colores[5], CYN);
    // char pokemones[3][1000] = {{CHARIZARD}, {BLASTOISE}, {VENUSAUR}};
    // char colores [7][1000] = {{RED},{GRN}, {YEL}, {BLU}, {MAG}, {CYN}, {WHT}};

    int poke = rand() % 3;
    int color = rand() % 6;

    //pantalla_carga(colores[color], poke, color);
    */
    hospital_t* hospital = hospital_crear();
    bool error = hospital_leer_archivo(hospital, "ejemplos/varios_entrenadores.hospital");

    int i = 0;
    if(argc > 1){
        while(i < argc && !error)
        error = hospital_leer_archivo(hospital, argv[i]);
    }

    if(!error) return ERROR;

    Juego juego;
    //bool animaciones = false;
    int pokemon = rand()%9;
    char colores[15][13] = {TXT_GREEN_112, TXT_GREEN_118, TXT_LIGHT_GREEN_119, TXT_RED_124, TXT_RED_160, TXT_RED_196,TXT_LIGHT_BLUE_32 ,TXT_LIGHT_BLUE_32,TXT_LIGHT_BLUE_33};

printf("%s<<------------------------------------------------------------------------------------------------------------------------------------>>\n", TXT_ORANGE_214);
printf("\t\t$$$$$$$l  $$l                                                   $$l       $$l                                                     \n");
printf("\t\t$$  __$$l l__|                                                  l__|      $$ |                                                      \n");
printf("\t\t$$ |  $$ |$$l  $$$$$$l  $$$$$$$l $$l    $$l  $$$$$$l  $$$$$$$l  $$l  $$$$$$$ | $$$$$$l                                              \n");
printf("\t\t$$$$$$$l |$$ |$$  __$$l $$  __$$ll$$l  $$  |$$  __$$l $$  __$$l $$ |$$  __$$ |$$  __$$l                                             \n");
printf("\t\t$$  __$$l $$ |$$$$$$$$ |$$ |  $$ |l$$l$$  / $$$$$$$$ |$$ |  $$ |$$ |$$ /  $$ |$$ /  $$ |                                            \n");
printf("\t\t$$ |  $$ |$$ |$$   ____|$$ |  $$ | l$$$  /  $$   ____|$$ |  $$ |$$ |$$ |  $$ |$$ |  $$ |                                            \n");
printf("\t\t$$$$$$$  |$$ |l$$$$$$$l $$ |  $$ |  l$  /   l$$$$$$$l $$ |  $$ |$$ |l$$$$$$$ |l$$$$$$  |                                            \n");
printf("\t\tl_______/ l__| l_______|l__|  l__|   l_/     l_______|l__|  l__|l__| l_______| l______/                                             \n\n");

printf(" $$$$$$l  $$l        $$$$$$l  $$l                         $$l                 $$l                                 $$l           $$l \n");
printf("$$  __$$l $$ |      $$  __$$l l__|                        $$ |                $$ |                                $$ |          $$ |\n");
printf("$$ /  $$ |$$ |      $$ /  l__|$$l $$$$$$l$$$$l  $$l   $$l $$ | $$$$$$l   $$$$$$$ | $$$$$$l   $$$$$$l         $$$$$$$ | $$$$$$l  $$ |\n");
printf("$$$$$$$$ |$$ |      l$$$$$$l  $$ |$$  _$$  _$$l $$ |  $$ |$$ | l____$$l $$  __$$ |$$  __$$l $$  __$$l       $$  __$$ |$$  __$$l $$ |\n");
printf("$$  __$$ |$$ |       l____$$l $$ |$$ / $$ / $$ |$$ |  $$ |$$ | $$$$$$$ |$$ /  $$ |$$ /  $$ |$$ |  l__|      $$ /  $$ |$$$$$$$$ |$$ |\n");
printf("$$ |  $$ |$$ |      $$l   $$ |$$ |$$ | $$ | $$ |$$ |  $$ |$$ |$$  __$$ |$$ |  $$ |$$ |  $$ |$$ |            $$ |  $$ |$$   ____|$$ |\n");
printf("$$ |  $$ |$$ |      l$$$$$$  |$$ |$$ | $$ | $$ |l$$$$$$  |$$ |l$$$$$$$ |l$$$$$$$ |l$$$$$$  |$$ |            l$$$$$$$ |l$$$$$$$l $$ |\n");
printf("l__|  l__|l__|       l______/ l__|l__| l__| l__| l______/ l__| l_______| l_______| l______/ l__|             l_______| l_______|l__ \n\n");

printf("\t\t$$l   $$l                               $$l   $$l               $$l                                                                 \n");
printf("\t\t$$ |  $$ |                              l__|  $$ |              $$ |                                                                \n");
printf("\t\t$$ |  $$ | $$$$$$l   $$$$$$$l  $$$$$$l  $$l $$$$$$l    $$$$$$l  $$ |                                                                \n");
printf("\t\t$$$$$$$$ |$$  __$$l $$  _____|$$  __$$l $$ |l_$$  _|   l____$$l $$ |                                                                \n");
printf("\t\t$$  __$$ |$$ /  $$ |l$$$$$$l  $$ /  $$ |$$ |  $$ |     $$$$$$$ |$$ |                                                                \n");
printf("\t\t$$ |  $$ |$$ |  $$ | l____$$l $$ |  $$ |$$ |  $$ |$$l $$  __$$ |$$ |                                                                \n");
printf("\t\t$$ |  $$ |l$$$$$$  |$$$$$$$  |$$$$$$$  |$$ |  l$$$$  |l$$$$$$$ |$$ |                                                                \n");
printf("\t\tl__|  l__| l______/ l_______/ $$  ____/ l__|   l____/  l_______|l__|                                                                \n");
printf("\t\t                              $$ |                                                                                                  \n");
printf("\t\t                              $$ |                                                                                                  \n");
printf("\t\t                              l__|                                                                                                  \n");
printf("<<------------------------------------------------------------------------------------------------------------------------------------>>\n%s", END_COLOR);

sleep(2);
system("clear");
    /*for(int i = 0; i < 9; i++){
        printf("%s Color: %i - num: %i\n", colores[i], i, pokemon);
    }*/
    mostrar_pantalla_carga(pokemon, colores[pokemon]);
    inicializar_juego(&juego, hospital);
    if(!juego.simulador) return ERROR;

    do{
        system("clear");
        mostrar_opciones();
        printf(" ➤ ");
        char c = leer_comando();
        system("clear");
        ejecutar_comando(&juego, c);
    }while(juego.jugando);

//sleep(4);
destruir_juego(&juego);

    return 0;
}
