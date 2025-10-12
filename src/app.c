/**
 * @file app.c
 * @brief Implementacion del modulo principal de control de escenas
 *
 * Este modulo contiene la logica del bucle principal del programa,
 * el manejo de eventos globales, y las funciones especificas de cada escena
 *
 * Tambien gestiona las transiciones entre pantallas (Splash, Menu, Game, Stats)
 * y centraliza el flujo general de la aplicacion
 *
 * Funciones principales:
 * - app_run(): ejecuta el bucle principal del programa
 * - app_changeScene(): cambia entre escenas y reinicia el temporizador
 *
 * Funciones internas:
 * - scene_name(): devuelve el nombre textual de una escena (para logs)
 * - update_/render_(): lógica y renderizado de cada pantalla
 *
 * @author
 *   Ferney Quiroga(devFQ)
 * @date
 *   Octubre 2025
 * @version 
 *   1.0
 */

#include "../include/app.h"
#include <stdio.h>

/**
*@brief Actualiza la escena de la presentacion (Splash)
*@param app Puntero a la aplicacion principal
*/

static void update_splash(Application* app);


/**
*@brief Dibuja la pantalla de presentacion(Splash)
*@param app Puntero a la aplicacion principal
*/

static void render_splash(Application* app);


/**
*@brief Maneja los eventis del menu principal
*@param app Puntero a la aplicacion principal
*@param event Evento capturado por SDL
*/

static void handle_menu_events(Application* app, SDL_Event* event);


/**
*@brief Dibuja la pantalla del menu principal
*@param app Puntero a la aplicacion principal
*/

static void render_menu(Application* app);


/**
*@brief Actualiza la escena del juego
*@param app Puntero a la aplicacion principal
*/

static void update_game(Application* app);


/**
*@brief Renderiza la escena del juego principal
*@param app Puntero a la aplicacion principal
*/

static void render_game(Application* app);


/**
*@brief Actualiza la escena de estadisticas
*@param app Puntero a la aplicacion principal
*/

static void update_stats(Application* app);


/**
*@brief Dibuja las pantalla de estadisticas
*@param app Puntero a la aplicacion principal
*/

static void render_stats(Application* app);


/**
*@brief Devuelve el nombre textual de una escena
* Esta funcion de utiliza para mostrar mensajes informativos en la consola
* cuando la aplicacion cambia de escena
Devuelve una cadena constante
*(literal) asociada al valor del enum Scene
*@param scene Valor de la enumeracion Scene
*@return const char* Cadena con el nombre de la escena
*/

static const char* scene_name(Scene scene);







static const char* scene_name(Scene scene){
    switch(scene){
        case SCENE_SPLASH: return "SPLASH";
        case SCENE_MENU: return "MENU";
        case SCENE_GAME: return "GAME";
        case SCENE_STATS: return "STATS";
        case SCENE_QUIT: return "QUIT";
        default: return "UNKNOWN";
    }
}


void app_changeScene(Application* app, Scene newScene){
    printf("[INFO] Cambiando escena: %s -> %s\n",scene_name(app->currentScene), scene_name(newScene));
    app->currentScene = newScene;
    app->sceneStartTime = SDL_GetTicks();
}



void app_run(SDL_Renderer* renderer){
    Application app = {
        .renderer = renderer,
        .currentScene = SCENE_SPLASH,
        .isRunning = true,
        .sceneStartTime = SDL_GetTicks()
    };

    SDL_Event event;


    while(app.isRunning){

        /*--- 1. Manejo de eventos ---*/
        while(SDL_PollEvent(&event)){
            //Eventos globales
            if(event.type == SDL_QUIT)
                app.isRunning = false;
            if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
                app.isRunning = false;

            //Eventos por escena
            if(app.currentScene == SCENE_MENU)
                handle_menu_events(&app,&event);
        }

        /*--- 2. Actualizacion ---*/
        switch(app.currentScene){
            case SCENE_SPLASH: update_splash(&app);
            break;
            case SCENE_GAME: update_game(&app);
            break;
            case SCENE_STATS: update_stats(&app);
            break;
            default: break;
        }

        /*--- 3. Renderizado ---*/
        switch(app.currentScene){
            case SCENE_SPLASH: render_splash(&app);
            break;
            case SCENE_MENU: render_menu(&app);
            break;
            case SCENE_GAME: render_game(&app);
            break;
            case SCENE_STATS: render_stats(&app);
            break;
            case SCENE_QUIT: app.isRunning = false;
            break;
            default: break;
        }

        SDL_Delay(16);
    }
}



static void update_splash(Application* app){
    Uint32 elapsed = SDL_GetTicks() - app->sceneStartTime;
    if(elapsed > 1200)
        app_changeScene(app,SCENE_MENU);
}

static void render_splash(Application* app){
    SDL_SetRenderDrawColor(app->renderer,COLOR_SPLASH);
    SDL_RenderClear(app->renderer);
    //TODO: Dibujar el titulo con SDL_ttf
    SDL_RenderPresent(app->renderer);
}

static void handle_menu_events(Application* app, SDL_Event* event){
    if(event->type == SDL_KEYDOWN)
        app_changeScene(app,SCENE_GAME);
}

static void render_menu(Application* app){
    SDL_SetRenderDrawColor(app->renderer, COLOR_MENU);
    SDL_RenderClear(app->renderer);
    //TODO: Dibujar opciones
    SDL_RenderPresent(app->renderer);
}

static void update_game(Application* app){
    Uint32 elapsed = SDL_GetTicks() - app->sceneStartTime;
    if(elapsed > 2000)
        app_changeScene(app,SCENE_STATS);
}

static void render_game(Application* app){
    SDL_SetRenderDrawColor(app->renderer,COLOR_GAME);
    SDL_RenderClear(app->renderer);
    //TODO: Dibujar la botonera / ecualizador
    SDL_RenderPresent(app->renderer);
}

static void update_stats(Application* app){
    Uint32 elapsed = SDL_GetTicks() - app->sceneStartTime;
    if(elapsed > 1000)
        app_changeScene(app, SCENE_QUIT);
}

static void render_stats(Application* app){
    SDL_SetRenderDrawColor(app->renderer, COLOR_STATS);
    SDL_RenderClear(app->renderer);
    //TODO: Mostrar resultados
    SDL_RenderPresent(app->renderer);
}



