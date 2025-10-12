/**
*@file main.c
*@brief Punto de entrada del programa SIMON 80s
*
* Este archivo contiene la funcion principal (main), encargada de:
* - Inicializar las bibliotecas SDL, SDL_ttf, y SDL_mixer
* - Crear la ventana y el renderizador principal
* - Ejecutar el bucle principal mediante app_run()
* - Liberar todos los recursos antes de salir
*/

#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "../include/app.h"
#undef main

int main(int argc, char* argv[]) {
    /*--- 1. Inicializacion de SDL y modulos ---*/
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)!=0){
        fprintf(stderr, "[ERROR] SDL_Init: %s\n",SDL_GetError());
        return 1;
    }

    if(TTF_Init()== -1){
        fprintf(stderr, "[ERROR] TTF_Init: %s\n",TTF_GetError());
        SDL_Quit();
        return 1;
    }

    if(Mix_OpenAudio(11025, AUDIO_U8, 1, 1024) < 0){
        fprintf(stderr, "[ERROR] Mix_OpenAudio: %s\n",Mix_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    /*--- 2. Creacion de la ventana principal y renderizador ---*/
    const int WIN_WIDTH = 1024;
    const int WIN_HEIGHT = 768;

    SDL_Window* window = SDL_CreateWindow(
        "SIMON 80s",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH, WIN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if(!window){
        fprintf(stderr, "[ERROR] SDL_CreateWindow: %s\n", SDL_GetError());
        Mix_CloseAudio();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!renderer){
        fprintf(stderr, "[ERROR] SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    /*--- 3. Ejecucion del bucle principal de la aplicacion ---*/
    app_run(renderer);

    /*--- 4. Liberacion de recursos y cierre ---*/
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();

    printf("[INFO] Programa finalizado correctamente. \n");
    return 0;
}
