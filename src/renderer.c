/**
*@file renderer.c
*@brief Implementacion del ecualizador 8-bit con barras luminosas y efecto glow
*
*Este modulo maneja el renderizado visual principal del juego SIMON 80s,
*basado en un conjunto de barras verticales que simulan un ecualizador retro
*
*Cada barra tiene un color neon distinto, una altura dinamica y un efecto
*de brillo ("glow") que decae progresivamente con el tiempo
*Se utiliza blending alfa para simular la luminiscencia caracteristica de los 80s
*
*El modulo es completamente independiente y puede reutilizarse en otras escenas
*
*@author
*Ferney Quiroga (devFQ)
*@date
*octubre 2025
*@version
* 1.0
*/

#include "../include/renderer.h"

/**
*@brief Paleta de colores sinteticos inspirados en estetica 80s synthwave
*/

static SDL_Color colorNeonCyan = {0, 255, 255, 255};
static SDL_Color colorNeonLime = {0, 255, 102, 255};
static SDL_Color colorNeonYellow = {255, 255, 0, 255};
static SDL_Color colorNeonMagenta = {255, 0, 255, 255};
static SDL_Color colorNeonBlue = {64, 160, 255, 255};
static SDL_Color colorNeonOrange = {255, 136, 32, 255};
static SDL_Color colorNeonWhite = {255, 255, 255, 255};
static SDL_Color colorNeonPurple = {204, 0, 255, 255};

static SDL_Color paletteNeon[8];

static void draw_bar(SDL_Renderer* sdlRenderer, const EqBar* bar);


bool eq_init(SDL_Renderer* sdlRenderer, EqRenderer* eqRenderer, int numberOfBars){
    if(!sdlRenderer || !eqRenderer)
        return false;

    if(numberOfBars < 3)
        numberOfBars = 3;
    if(numberOfBars > 8)
        numberOfBars = 8;

    eqRenderer->sdlRenderer = sdlRenderer;
    SDL_GetRendererOutputSize(sdlRenderer,&eqRenderer->rendererWindowWidth,&eqRenderer->rendererWindowHeight);
    eqRenderer->totalBars = numberOfBars;
    eqRenderer->gapBetweenBars = 24;

    /*Inicializacion de la paleta local*/
    paletteNeon[0] = colorNeonCyan;
    paletteNeon[1] = colorNeonLime;
    paletteNeon[2] = colorNeonYellow;
    paletteNeon[3] = colorNeonMagenta;
    paletteNeon[4] = colorNeonBlue;
    paletteNeon[5] = colorNeonOrange;
    paletteNeon[6] = colorNeonWhite;
    paletteNeon[7] = colorNeonPurple;

    eq_layout(eqRenderer);

    /*Estado inicial de cada barra*/
    for(int i = 0; i < eqRenderer->totalBars; ++i){
        EqBar* bar = &eqRenderer->barsArray[i];
        bar->barCurrentHeight = 0;
        bar->barMaximumHeight = eqRenderer->rendererWindowHeight - 160;
        bar->barDecayFactor = 0.92f;
        bar->barColor = paletteNeon[i];
        bar->barGlowIntensity = 0.0f;
    }
    return true;
}


void eq_layout(EqRenderer* eqRenderer){
    int windowWidth = eqRenderer->rendererWindowWidth;
    int windowHeight = eqRenderer->rendererWindowHeight;
    int marginX = 80;
    int marginY = 100;

    int usableWidth = windowWidth - marginX * 2;
    int totalGaps = (eqRenderer->totalBars - 1) * eqRenderer->gapBetweenBars;
    int barWidth = (usableWidth - totalGaps) / eqRenderer->totalBars;
    if(barWidth < 8)
        barWidth = 8;

    int currentX = marginX;
    int baseY = windowHeight - marginY;

    for(int i=0; i<eqRenderer->totalBars;++i){
        EqBar* bar = &eqRenderer->barsArray[i];
        bar->baseRectangle.x = currentX;
        bar->baseRectangle.y = baseY;
        bar->baseRectangle.w = barWidth;
        bar->baseRectangle.h = 4; //altura minima del pie
        currentX += barWidth + eqRenderer->gapBetweenBars;
        bar->barMaximumHeight = (windowHeight - marginY * 2);

    }
}

void eq_triggerPulse(EqRenderer* eqRenderer, int barIndex){
    if(!eqRenderer || barIndex < 0 || barIndex >= eqRenderer->totalBars)
        return;
    EqBar* bar = &eqRenderer->barsArray[barIndex];
    bar->barCurrentHeight = bar->barMaximumHeight;
    bar->barGlowIntensity = 1.0f;
}

void eq_update(EqRenderer* eqRenderer){
    for(int i = 0; i < eqRenderer->totalBars; ++i){
        EqBar*bar = &eqRenderer->barsArray[i];
        bar->barCurrentHeight = (int)(bar->barCurrentHeight*bar->barDecayFactor);
        if(bar->barCurrentHeight < 1)
            bar->barCurrentHeight = 0;
        bar->barGlowIntensity *=0.92f;
        if(bar->barGlowIntensity < 0.01f)
            bar->barGlowIntensity = 0.0f;
    }
}

/**
*@brief Dibuja una barra individual y su halo luminoso
*
*Utiliza el modo de blending alfa para el efecto glow
*
*@param sdlRenderer Renderer SDL activo
*@param bar Puntero a la bara que se va a dibujar
*/


static void draw_bar(SDL_Renderer* sdlRenderer, const EqBar* bar){
    /*halo luminoso*/
    if(bar->barGlowIntensity > 0.0f){
        int haloPadding = (int)(8 + 18 * bar->barGlowIntensity);
        Uint8 haloAlpha = (Uint8)(70 + 120 * bar->barGlowIntensity);

        SDL_Rect haloRect = {
            bar->baseRectangle.x - haloPadding,
            bar->baseRectangle.y - bar->barCurrentHeight - haloPadding,
            bar->baseRectangle.w + 2 * haloPadding,
            bar->barCurrentHeight + 2 * haloPadding
        };
        SDL_SetRenderDrawBlendMode(sdlRenderer,SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(sdlRenderer, bar->barColor.r, bar->barColor.g, bar->barColor.b,haloAlpha);
        SDL_RenderFillRect(sdlRenderer, &haloRect);
        SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_NONE);
    }
    /*Cuerpo principal*/
    SDL_Rect barBodyRect = {
        bar->baseRectangle.x,
        bar->baseRectangle.y - bar->barCurrentHeight,
        bar->baseRectangle.w,
        bar->barCurrentHeight
    };

    SDL_SetRenderDrawColor(sdlRenderer, bar->barColor.r, bar->barColor.g, bar->barColor.b, 255);
    SDL_RenderFillRect(sdlRenderer, &barBodyRect);

    /*Contorno tipo pixelado*/
    SDL_SetRenderDrawColor(sdlRenderer, 25, 25, 40,255);
    SDL_RenderDrawRect(sdlRenderer, &barBodyRect);
}

void eq_render(const EqRenderer* eqRenderer){
    for(int i = 0; i < eqRenderer->totalBars; ++i){
        draw_bar(eqRenderer->sdlRenderer, &eqRenderer->barsArray[i]);
    }

}

int eq_detectHit(const EqRenderer* eqRenderer, int mouseX, int mouseY){
    for(int i = 0; i < eqRenderer->totalBars; ++i){
        SDL_Rect hitRect = {
            eqRenderer->barsArray[i].baseRectangle.x,
            eqRenderer->barsArray[i].baseRectangle.y - eqRenderer->barsArray[i].barMaximumHeight,
            eqRenderer->barsArray[i].baseRectangle.w,
            eqRenderer->barsArray[i].barMaximumHeight
        };

        if(mouseX >=hitRect.x && mouseX < hitRect.x + hitRect.w && mouseY >= hitRect.y && mouseY < hitRect.y + hitRect.h){
            return i;
        }
    }
    return -1;
}


