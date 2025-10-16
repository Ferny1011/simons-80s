/**
*@file renderer.h
*@brief Modulo para simular el modo Mozart en el juego
*
*Este modulo se encarga de realizar la carga, inicializacion, actualizacion
*y renderizacion del modo Mozart.
*/

#ifndef MOZART_H_INCLUDED
#define MOZART_H_INCLUDED

#include "renderer.h"
#include "audio.h"
#include "input.h"
#include <stdbool.h>

#define MAX_MOZART_SEQ 128 //secuencia maxima
#define MOZART_PATH "data/mozart.seq" //path del archivo de secuencia (si se cambia de lugar modificar)

//estados de Mozart
typedef enum
{
    MOZ_SHOW, //mostrar seq
    MOZ_WAIT, //esperar entrada
    MOZ_WIN, //seq correcta
    MOZ_FAIL //error jugador

} MozartState;

//variables de juego
typedef struct
{
    int seq[MAX_MOZART_SEQ]; //secuencia maxima
    int seqLen; //longitud de la secuencia
    int current; //indice actual
    int playerStep; //paso del jgdr
    Uint32 timer; //control temp
    MozartState state; //estado del juego
    bool waiting; //flag de espera
    bool loaded; //flag de cargado

} MozartGame;

/**
*@brief
*@param
*@param
*@return
*/
bool cargarSecMozartM(MozartGame* g, const char* path);

/**
*@brief
*@param
*@return
*/
void iniciarMozartM(MozartGame* g);

/**
*@brief
*@param
*@param
*@param
*@param
*@return
*/
void actualizarMozartM(MozartGame* g, EqRenderer* eq, AudioSystem* audio, InputController* in);

/**
*@brief
*@param
*@param
*@return
*/
void renderMozartM(MozartGame* g, EqRenderer* eq);

#endif // MOZART_H_INCLUDED
