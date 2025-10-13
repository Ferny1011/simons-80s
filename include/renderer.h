/**
*@file renderer.h
*@brief Modulo de renderizacion del panel tipo ecualizador 8-bit con efecto glow
*
*Este modulo se encarga de administrar y renderizar un conjunto de barras
*luminosas animadas que representan la estetica del juego SIMON 80s
*/

#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <SDL.h>
#include <stdbool.h>

/**
*@struct EqBar
*@brief Representa una barra vertical individual del ecualizador
*/
typedef struct{
    SDL_Rect baseRectangle; //Rect base(pie de la barra) usado para posicion y ancho
    int barCurrentHeight; //Altura actual de la barra(px)
    int barMaximumHeight; //Altura maxima alcanzable (px)
    float barDecayFactor; //Factor de decaimiento por frame(0.90-0.99)
    SDL_Color barColor; //Color neon de la barra
    float barGlowIntensity; //Intensidad del halo (0.0 - 1.0)
}EqBar;


/**
*@struct EqRenderer
*@brief Estructura principal del modulo de renderizado de ecualizador (coleccion de barras)
*/
typedef struct{
    SDL_Renderer* sdlRenderer; //Renderizador SDL utilizado para dibujar
    int rendererWindowWidth; //Ancho actual de la ventana
    int rendererWindowHeight; //Alto actual de la ventana
    int totalBars; //Cantidad de barras visibles (3 - 8)
    int gapBetweenBars; //Espaciado horizontal entre barras (px)
    EqBar barsArray[8];
}EqRenderer;


/**
* @brief Inicializa el ecualizador con un numero determinado de barras
* Configura la paleta de colores neon, establece las dimensiones iniciales
* y asgna un color y una altura maxima a cada barra
* @param sdlRenderer Renderer de destino de SDL
* @param eqRenderer Estructura donde se guardara el estado del ecualizador
* @param numberOfBars Cantidad de barras(se ajusta al rango 3-8)
* @return true si la inicializacion fue exitosa
*/
bool eq_init(SDL_Renderer* sdlRenderer, EqRenderer* eqRenderer, int numberOfBars);


/**
* @brief Recalcula las posiciones y dimensiones de las barras
* Debe llamarse cuando cambia el tamaño de la ventana o se modifica
* el numero de barras visibles
* @param eqRenderer Estado actual del ecualizador
*/
void eq_layout(EqRenderer* eqRenderer);


/**
* @brief Activa un pulso visual en la barra especificada
* Incrementa instantaneamente la altura al maximo y enciende el glow
* @param eqRenderer Estado actual del ecualizador
* @param barIndex Indice de la barra (0 - totalBars-1)
*/
void eq_triggerPulse(EqRenderer* eqRenderer, int barIndex);


/**
* @brief Actualiza la animacion del ecualizador (decaimiento de altura y glow)
* @param eqRenderer Estado actual del ecualizador
*/
void eq_update(EqRenderer* eqRenderer);


/**
* @brief Renderiza todas las barras con su correspondiente efecto de brillo
* @param eqRenderer Estado actual del ecualizador
*/
void eq_render(const EqRenderer* eqRenderer);


/**
* @brief Determina si el mouse se encuentra sobre alguna barra
* @param eqRenderer Estado actual del ecualizador
* @param mouseX Coordenada X del mouse
* @param mouseY Coordenada Y del mouse
* @return Indice de la barra clickeada, o -1 si no hay colision
*/
int eq_detectHit(const EqRenderer* eqRenderer, int mouseX, int mouseY);

#endif // RENDERER_H_INCLUDED
