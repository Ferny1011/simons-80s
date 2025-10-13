/**
 * @file app.h
 * @brief Interfaz publica del modulo principal de control de escenas
 *
 * Este archivo define las estructuras, enumeraciones y funciones pulicas
 * del modulo app.c, encargado del flujo general de la aplicacion SIMON 80s
 *
 * Su objetivo es exponer las herramientas necesarias para controlar:
 * - El estado general de la aplicacion
 * - La escena o pantalla actual
 * - El bucle principal del programa (app_run)
 *
 * Contiene tambien las constantes de color utilizadas por cada escena,
 * las cuales definen la paleta base del proyecto con estetica 80s
 *
 * @author
 *   Ferney Quiroga(devFQ)
 * @date
 *   Octubre 2025
 * @version 
 *   1.0
 */

#ifndef APP_H_INCLUDED
#define APP_H_INCLUDED

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

/**
*@def COLOR_SPLASH
*@brief Color de fondo para la pantalla de presentacion
*/
#define COLOR_SPLASH 8, 8, 20, 255


/**
*@def COLOR_MENU
*@brief Color de fondo para la pantalla del menu principal
*/
#define COLOR_MENU 20, 0, 40, 255


/**
*@def COLOR_GAME
*@brief Color de fondo para la pantalla del juego
*/
#define COLOR_GAME 0, 0, 0, 255


/**
*@def COLOR_STATS
*@brief Color de fondo para la pantalla de estadisticas
*/
#define COLOR_STATS 0, 30, 30, 255


/**
*@def COLOR_PAUSE_OVERLAY
*@brief Color semitransparente usado para la superposicion de pausa
*/
#define COLOR_PAUSE_OVERLAY 0, 0, 0, 128


/**
*@def COLOR_PAUSE_TEXT
*@brief Color del texto "PAUSA" sobre el overlay
*/
#define COLOR_PAUSE_TEXT 255, 128, 255, 255


/**
*@def FONT_UI_PATH
*@brief Fuente principal para titulos
*/
#define FONT_UI_PATH "fnt/PressStart2P.ttf"


/**
*@enum Scene
*@brief Representa las diferentes pantallas o escenas principales del programa
*/

typedef enum {
    SCENE_SPLASH,
    SCENE_MENU,
    SCENE_GAME,
    SCENE_STATS,
    SCENE_QUIT
} Scene;


/**
*@struct Application
*@brief Contiene el estado general de la aplicacion
* Guarda referencias al renderizador, la escena actual y controla el bucle principal
*/

typedef struct {
    SDL_Renderer* renderer; //Renderizado principal
    Scene currentScene; // Escena actual
    bool isRunning; // Control del bucle
    bool isPaused; // Control de la pausa
    Uint32 pauseStartTime; // Tiempo de inicio de la pausa (ms)
    Uint32 sceneStartTime; // Tiempo de inicio de la escena (ms)
} Application;


/**
*@brief Alterna el estado de pausa global de la aplicacion
*Si se pausa, guarda el tiempo de inicio
*Si se reanuda, compensa sceneStartTime
*@param app Puntero a la aplicacion
*/

void app_togglePause(Application* app);


/**
* @brief Cambia la escena actual de la aplicacion y reincia su temporizador
* Ademas, imprime en consola el cambio con el nombre textual
* de la escena anterior y la nueva, para depuracion y seguimiento
*
* @param app Puntero a la estructura principal de la aplicacion
* @param newScene Escena a la que se desea cambiar
*/

void app_changeScene(Application* app, Scene newScene);



/**
* @brief Ejecuta el bucle principal de la aplicacion
*        Controla el flujo del programa, maneja eventos globales, actualiza la logica
*        y renderiza la escena correspondiente
*
* @param renderer Puntero al renderizador principal de SDL
*/

void app_run(SDL_Renderer* renderer);

#endif // APP_H_INCLUDED
