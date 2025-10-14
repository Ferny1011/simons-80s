/**
*@file input.c
*@brief Implementaion del modulo de entrada
*/

#include "../include/input.h"
#include <SDL_assert.h>

void input_init(InputController* input){
    input->lastBarPressed = -1;
    input->keyHeld = false;
    input->mouseClicked = false;
}

int input_process_event(InputController* input, const SDL_Event* event, bool isPaused, bool isGameScene, inputHitTestFn hitTestFn, const void* hitTestUserData){
    if(isPaused || !isGameScene)
        return -1;
    SDL_assert(hitTestFn != NULL);

    /*Teclado: Z/X/C/V */
    if(event->type == SDL_KEYDOWN && !input->keyHeld){
        input->keyHeld = true;
        int barIndex = -1;
        switch(event->key.keysym.sym){
            case SDLK_z: barIndex = 0;
            break;
            case SDLK_x: barIndex = 1;
            break;
            case SDLK_c: barIndex = 2;
            break;
            case SDLK_v: barIndex = 3;
            break;
            default: break;
        }
        if(barIndex >= 0){
            input->lastBarPressed = barIndex;
            return barIndex;
        }
    }
    if(event->type == SDL_KEYUP){
        input->keyHeld = false;
    }

    /*Mouse: clic izquierdo*/
    if(event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT){
        input->mouseClicked = true;
        const int mouseX = event->button.x;
        const int mouseY = event->button.y;
        const int hit = hitTestFn(hitTestUserData, mouseX, mouseY);

        if(hit>=0){
            input->lastBarPressed = hit;
            return hit;
        }
    }

    if(event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT){
        input->mouseClicked = false;
    }
    return -1;
}

