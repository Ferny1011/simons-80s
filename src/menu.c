/**
 * @file menu.c
 * @brief Módulo de configuración del juego SIMON 80s
 */

#include "../include/menu.h"
#include <stdio.h>

SDL_Color colorConfigBg = {30, 30, 60, 255};
SDL_Color colorButtonNormal = {70, 70, 120, 255};
SDL_Color colorButtonPressed = {110, 110, 160, 255};
SDL_Color colorTextWhite = {255, 255, 255, 255};

/**
 * @brief Inicializa la configuración del juego con valores por defecto
 */

void config_init(GameConfig* config) {
    config->noteCount = 4; // Valor por defecto: 4 notas
    config->noteDuration = 3000; // Valor por defecto: 3 segundos
}

/**
 * @brief Inicializa el menú de configuración y sus botones
 */

void config_menu_init(ConfigMenu* menu, GameConfig* config, TTF_Font* font) {
    menu->gameConfig = config;
    menu->font = font;

    int centerX = 512; // Centro para 1024px
    int startY = 200;
    int buttonWidth = 40;
    int buttonHeight = 40;
    int spacing = 80;

    // Botones para cantidad de notas
    menu->noteCountMinusBtn = (Button){
        {centerX - 150, startY, buttonWidth, buttonHeight}, "-", false
    };

    menu->noteCountPlusBtn = (Button){
        {centerX + 110, startY, buttonWidth, buttonHeight}, "+", false
    };

    // Botones para duración
    menu->durationMinusBtn = (Button){
        {centerX - 150, startY + spacing, buttonWidth, buttonHeight}, "-", false
    };

    menu->durationPlusBtn = (Button){
        {centerX + 110, startY + spacing, buttonWidth, buttonHeight}, "+", false
    };

    // Botón iniciar juego
    menu->startGameBtn = (Button){
        {centerX - 80, startY + spacing * 2 + 50, 160, 50}, "START", false
    };
}

/**
 * @brief Limpia recursos del menú de configuración
 */
void config_menu_cleanup(ConfigMenu* menu){
    menu->gameConfig = NULL;
    menu->font = NULL;
}

/**
 * @brief Verifica si un punto está dentro de un botón
 */
bool config_point_in_button(Button* btn, int x, int y){
    return (x >= btn->rect.x && x < btn->rect.x + btn->rect.w && y >= btn->rect.y && y < btn->rect.y + btn->rect.h);
}

/**
 * @brief Renderiza un botón con su texto
 */
void config_render_button(SDL_Renderer* renderer, Button* btn, TTF_Font* font){
    // Determinar color de fondo según estado
    SDL_Color bgColor = btn->isPressed ? colorButtonPressed : colorButtonNormal;

    // Dibujar fondo del botón
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &btn->rect);

    // Dibujar borde
    SDL_SetRenderDrawColor(renderer, colorTextWhite.r, colorTextWhite.g, colorTextWhite.b, colorTextWhite.a);
    SDL_RenderDrawRect(renderer, &btn->rect);

    // Renderizar texto
    if (font){
        SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, btn->text, colorTextWhite);
        if (textSurface){
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture){
                SDL_Rect textRect = {
                    btn->rect.x + (btn->rect.w - textSurface->w) / 2,
                    btn->rect.y + (btn->rect.h - textSurface->h) / 2,
                    textSurface->w,
                    textSurface->h
                };
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
    }
}

/**
 * @brief Maneja eventos del menú de configuración
 */
void config_menu_handle_event(ConfigMenu* menu, SDL_Event* event, bool* shouldChangeScene, int* newScene){
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    *shouldChangeScene = false;

    //manejar eventos de mouse
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT){
       // Activar estado pressed para todos los botones que reciban click
       menu->noteCountMinusBtn.isPressed = config_point_in_button(&menu->noteCountMinusBtn, mouseX, mouseY);
       menu->noteCountPlusBtn.isPressed = config_point_in_button(&menu->noteCountPlusBtn, mouseX, mouseY);
       menu->durationMinusBtn.isPressed = config_point_in_button(&menu->durationMinusBtn, mouseX, mouseY);
       menu->durationPlusBtn.isPressed = config_point_in_button(&menu->durationPlusBtn, mouseX, mouseY);
       menu->startGameBtn.isPressed = config_point_in_button(&menu->startGameBtn, mouseX, mouseY);

       // Procesar acciones
       if (menu->noteCountMinusBtn.isPressed && menu->gameConfig->noteCount > MIN_NOTES){
        menu->gameConfig->noteCount--;
       }
       if (menu->noteCountPlusBtn.isPressed && menu->gameConfig->noteCount < MAX_NOTES){
        menu->gameConfig->noteCount++;
       }
       if (menu->durationMinusBtn.isPressed && menu->gameConfig->noteDuration > MIN_DURATION){
        menu->gameConfig->noteDuration -= DURATION_STEP;
       }
       if (menu->durationPlusBtn.isPressed && menu->gameConfig->noteDuration < MAX_DURATION){
        menu->gameConfig->noteDuration += DURATION_STEP;
       }
       if (menu->startGameBtn.isPressed){
        *shouldChangeScene = true;
        *newScene = 2; // SCENE_GAME
       }
    }

    // Desactivar el estado pressed cuando se suelta el botón
    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT){
        menu->noteCountMinusBtn.isPressed = false;
        menu->noteCountPlusBtn.isPressed = false;
        menu->durationMinusBtn.isPressed = false;
        menu->durationPlusBtn.isPressed = false;
        menu->startGameBtn.isPressed = false;
    }

    // Manejar teclas
    if (event->type == SDL_KEYDOWN){
        switch (event->key.keysym.sym){
            case SDLK_ESCAPE:
                *shouldChangeScene = true;
                *newScene = 4; // SCENE_QUIT
                break;
        }
    }
}

/**
 * @brief Renderiza el menú de configuración completo
 */
void config_menu_render(ConfigMenu* menu, SDL_Renderer* renderer){
    // Fondo de la pantalla
    SDL_SetRenderDrawColor(renderer, colorConfigBg.r, colorConfigBg.g, colorConfigBg.b, colorConfigBg.a);
    SDL_RenderClear(renderer);

    if (menu->font){
        // Titulo principal
        SDL_Surface* titleSurface = TTF_RenderUTF8_Blended(menu->font, "SIMONS 80s", colorTextWhite);
        if (titleSurface){
            SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
            if (titleTexture){
                SDL_Rect titleRect = {512 - titleSurface->w / 2, 80, titleSurface->w, titleSurface->h};
                SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
                SDL_DestroyTexture(titleTexture);
            }
            SDL_FreeSurface(titleSurface);
        }

        // Texto cantidad de notas
        char noteText[64];
        snprintf(noteText, sizeof(noteText), "Cantidad de notas: %d", menu->gameConfig->noteCount);
        SDL_Surface* noteSurface = TTF_RenderUTF8_Blended(menu->font, noteText, colorTextWhite);
        if (noteSurface){
            SDL_Texture* noteTexture = SDL_CreateTextureFromSurface(renderer, noteSurface);
            if (noteTexture){
                SDL_Rect noteRect = {512 - noteSurface->w / 2, 170, noteSurface->w, noteSurface->h};
                SDL_RenderCopy(renderer, noteTexture, NULL, &noteRect);
                SDL_DestroyTexture(noteTexture);
            }
            SDL_FreeSurface(noteSurface);
        }

        // Texto Duración
        char durationText[64];
        snprintf(durationText, sizeof(durationText), "Duracion por nota: %d ms", menu->gameConfig->noteDuration);
        SDL_Surface* durationSurface = TTF_RenderUTF8_Blended(menu->font, durationText, colorTextWhite);
        if (durationSurface){
            SDL_Texture* durationTexture = SDL_CreateTextureFromSurface(renderer, durationSurface);
            if (durationTexture){
                SDL_Rect durationRect = {512 - durationSurface->w / 2, 250, durationSurface->w, durationSurface->h};
                SDL_RenderCopy(renderer, durationTexture, NULL, &durationRect);
                SDL_DestroyTexture(durationTexture);
            }
            SDL_FreeSurface(durationSurface);
        }
    }

    // Renderizar todos los botones
    config_render_button(renderer, &menu->noteCountMinusBtn, menu->font);
    config_render_button(renderer, &menu->noteCountPlusBtn, menu->font);
    config_render_button(renderer, &menu->durationMinusBtn, menu->font);
    config_render_button(renderer, &menu->durationPlusBtn, menu->font);
    config_render_button(renderer, &menu->startGameBtn, menu->font);

    SDL_RenderPresent(renderer);
}
