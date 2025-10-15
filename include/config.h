/**
*@file audio.h
*@brief Modulo generación, modificacion y carga de configuracion
*
*Este modulo se encarga de todo el manejo de la configuracion para ejecutar el programa
*/


#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SDL.h>

#define MODE_SCHOENBERG 0
#define MODE_MOZART 1

typedef struct
{
    int windowW; //tamaño ventana
    int windowH; //tamaño ventana
    int dif; //dificultad
    bool mode; //modo mozart activo o no
    char playerN[32] ; //nombre jugador
    int volumen; //volumen maestro del juego

    ///agregar variables si se necesitan

}Config;

/**
*@brief Carga las configuraciones default predefinidas en el programa
*@param c Puntero a la configuracion
*/
void configurarDefaults(Config* c);

/**
*@brief Valida los parametros cargados en la configuracion, en caso de no estarlo
*los corrige
*@param c Puntero a la configuracion
*/
void validarConfig(Config* c);

/**
*@brief Carga la configuracion que se levanta del archivo cfg
*@param c Puntero a la configuracion
*/
int cargarConfig(Config* out); // 0 ok, -1 usa defaults

/**
*@brief Guarda la configuracion en el archivo cfg
*@param c Puntero constante a la configuracion
*/
int guardarConfig(const Config* c); // 0 ok, -1 error

/**
*@brief Se obtiene el path del archivo de configuracion
*@param n\a
*/
const char* configPath(void); //configurar path

#endif // CONFIG_H_INCLUDED
