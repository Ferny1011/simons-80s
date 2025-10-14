/**
*@file input.h
*@brief Modulo de entrada del usuario (teclado + mouse)
*
*Este modulo normaliza las entradas SDL a un formato uniforme para el juego:
*convierte las teclas y clics del mouse en "disparos de barra"
*Implementa antirrebote basico y un mapeo fijo de teclas
*/

#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include <SDL.h>
#include <stdbool.h>

/**
*@struct InputController
*@brief Estado basico del sistema de entrada
*/

typedef struct{
    int lastBarPressed; //Ultimo indice de barra presionado o -1 si ninguno
    bool keyHeld; //Indica si una tecla sigue presionada (para antirrebote)
    bool mouseClicked; //Flag temporal para clics de mouse
}InputController;



/**
*@brief Firma de la funcion de hit test
Devuelve:
* -indice de barra (>=0) si (x,y) cae dentro de una barra
* - -1 si no golpea ninguna
*/
typedef int (*inputHitTestFn)(const void* userData, int mouseX, int mouseY);



/**
*@brief incializa el controlador de entrada
*/
void input_init(InputController* input);


/**
*@brief Procesa un evento SDL y devuelve la barra activada ( si corresponde)
*
*@param input Controlador de entrada
*@param event Evento SDL recibido
*@param isPaused si true, ignora entrada
*@param isGameScene si true, Procesa mapeo de juego
*@param hitTestFn Funcion obligatoria para detectar clic sobre barras
*@param hitTestUserData Puntero a datos que necesita el hit test
*@return Indice de barra si hubo accion; -1 si no hubo
*/

int input_process_event(InputController* input, const SDL_Event* event, bool isPaused, bool isGameScene, inputHitTestFn hitTestFn, const void* hitTestUserData);


#endif // INPUT_H_INCLUDED
