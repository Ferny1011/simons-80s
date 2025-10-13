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
#include "../include/renderer.h"
#include <stdio.h>

static TTF_Font* gUiFont = NULL; /*fuente de UI*/
static SDL_Texture* gPauseTex = NULL;/*textura "PAUSA" */
static SDL_Rect gPauseDst = {0, 0, 0, 0}; /*destino centrado*/


static EqRenderer eqRendererState; /*Estructura del renderer(barras, layout,etc)*/
static bool isEqInitialized = false; /*Se inicializa una sola vez en GAME*/

/**
*@brief Carga la fuente y prepara la textura "PAUSA"
*@param app Puntero a la aplicacion principal
*@return int 0 si OK, -1 si falla
*/
static int pause_ui_init(Application* app){
    int windowWidth = 0, windowHeight = 0;
    gUiFont = TTF_OpenFont(FONT_UI_PATH, 48);
    if(!gUiFont){
        fprintf(stderr, "[ERROR] TTF_OpenFont (%s): %s\n", FONT_UI_PATH, TTF_GetError());
        return -1;
    }
    /*Renderiza el texto "PAUSA" en una superfice temporal*/
    SDL_Color pauseTextColor = (SDL_Color){COLOR_PAUSE_TEXT};
    SDL_Surface* pauseSurface = TTF_RenderUTF8_Blended(gUiFont,"PAUSA",pauseTextColor);
    if(!pauseSurface){
        fprintf(stderr,"[ERROR] TTF_RenderUTF8_Blended: %s\n", TTF_GetError());
        return -1;
    }
    /* crea la textura a partir de la superfice renderizada*/
    gPauseTex = SDL_CreateTextureFromSurface(app->renderer,pauseSurface);
    gPauseDst.w = pauseSurface->w;
    gPauseDst.h = pauseSurface->h;

    SDL_FreeSurface(pauseSurface); //se libera superfice temporal

    SDL_GetRendererOutputSize(app->renderer, &windowWidth, &windowHeight);
    gPauseDst.x = (windowWidth - gPauseDst.w)/2;
    gPauseDst.y = (windowHeight - gPauseDst.h)/2;

    return(gPauseTex ? 0 : -1);
}


/**
*@brief Libera los recursos de la UI de pausa.
*/

static void pause_ui_shutdown(){
    if(gPauseTex){
        SDL_DestroyTexture(gPauseTex);
        gPauseTex = NULL;
    }
    if(gUiFont){
        TTF_CloseFont(gUiFont);
        gUiFont = NULL;
    }
}


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


/**
*@brief Dibuja un velo semitransparente y el texto "PAUSA" centrado
*@param app Puntero a la aplicacion
*/

static void render_pause_overlay(Application* app);




static void render_pause_overlay(Application* app){
    /*velo*/
    SDL_SetRenderDrawBlendMode(app->renderer,SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(app->renderer,COLOR_PAUSE_OVERLAY);
    SDL_Rect full = {0, 0, 0, 0};
    SDL_GetRendererOutputSize(app->renderer,&full.w,&full.h);
    SDL_RenderFillRect(app->renderer,&full);

    /*texto "PAUSA" (si esta disponible)*/
    if(gPauseTex){
        SDL_Rect shadow = gPauseDst;
        shadow.x +=2;
        shadow.y +=2;
        SDL_SetTextureAlphaMod(gPauseTex,80);
        SDL_RenderCopy(app->renderer,gPauseTex, NULL, &shadow);

        SDL_SetTextureAlphaMod(gPauseTex,255);
        SDL_RenderCopy(app->renderer,gPauseTex,NULL, &gPauseDst);
        }

        SDL_SetRenderDrawBlendMode(app->renderer,SDL_BLENDMODE_NONE);

    SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_NONE);
}




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

void app_togglePause(Application* app){
    if(!app->isPaused){
        app->isPaused = true;
        app->pauseStartTime = SDL_GetTicks();
        printf("[INFO] Pausa: ON\n");
    } else {
        app->isPaused = false;
        Uint32 pausedMs = SDL_GetTicks() - app->pauseStartTime;
        app->sceneStartTime += pausedMs; //Compensa el timer de escena
        printf("[INFO] Pausa: OFF (compensados %u ms)\n", pausedMs);
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
        .sceneStartTime = SDL_GetTicks(),
        .isPaused = false,
        .pauseStartTime = 0
    };

    SDL_Event event;

    /*--- Inicializacion de UI de pausa ---*/
    if(pause_ui_init(&app)!=0){
            /*No se aborta la app si falla el texto. El overlay seguira saliendo*/
        fprintf(stderr,"[WARN] No se pudo preparar texto de pausa.\n");
    }



    while(app.isRunning){

        /*--- 1. Manejo de eventos ---*/
        while(SDL_PollEvent(&event)){
            //Eventos globales
            if(event.type == SDL_QUIT)
                app.isRunning = false;
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    app.isRunning = false;
                if(event.key.keysym.sym == SDLK_p)
                    app_togglePause(&app);
            }
            if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
                /*Actualiza dimensiones y recalcula layout del ecualizador si ya estaba inciado*/
                if(isEqInitialized){
                    SDL_GetRendererOutputSize(app.renderer, &eqRendererState.rendererWindowWidth, &eqRendererState.rendererWindowHeight);
                    eq_layout(&eqRendererState);
                }
            }

            /*si estamos en GAME y NO estamos en pausa*/
            if(!app.isPaused && app.currentScene == SCENE_GAME){
                if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT){
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;
                    if(isEqInitialized){
                        int hitIndex = eq_detectHit(&eqRendererState, mouseX, mouseY);
                        if(hitIndex >= 0){
                            eq_triggerPulse(&eqRendererState, hitIndex);
                        }
                    }
                }
            }

            //Eventos de escena si no esta en pausa
            if(!app.isPaused && app.currentScene == SCENE_MENU)
                handle_menu_events(&app,&event);
        }

        /*--- 2. Actualizacion ---*/
        if(!app.isPaused){
         switch(app.currentScene){
            case SCENE_SPLASH: update_splash(&app);
            break;
            case SCENE_GAME: update_game(&app);
            break;
            case SCENE_STATS: update_stats(&app);
            break;
            default: break;
            }
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

        //overlay de pausa
        if(app.isPaused){
            render_pause_overlay(&app);
            SDL_RenderPresent(app.renderer);
        }

        SDL_Delay(16);
    }
    /*Liberar recursos de UI pausa*/
    pause_ui_shutdown();
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
    /*Inicializacion del ecualizador (solo la primera vez que entramos a GAME)*/
    if(!isEqInitialized){
        isEqInitialized = eq_init(app->renderer, &eqRendererState,4);
        if(isEqInitialized){
            /*Golpecito de presentacion en todas las barras*/
            for(int barIndex = 0; barIndex < eqRendererState.totalBars; ++barIndex){
                eq_triggerPulse(&eqRendererState, barIndex);
            }
        }
    } else{
        /*Anima: aplica decaimineto de altura y brillo en cada frame*/
        eq_update(&eqRendererState);
    }

    /*Temporal de flujo (tras 2s salta a STATS)*/
    Uint32 elapsed = SDL_GetTicks() - app->sceneStartTime;
    if(elapsed > 2000)
        app_changeScene(app,SCENE_STATS);
}

static void render_game(Application* app){
    /*Fondo del juego (paleta centralizada en app.h)*/
    SDL_SetRenderDrawColor(app->renderer,COLOR_GAME);
    SDL_RenderClear(app->renderer);

    /*Dibuja las barras si el renderer esta listo*/
    if(isEqInitialized){
        eq_render(&eqRendererState);
    }

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



