/**
*@file audio.h
*@brief Modulo de generacion y reproduccion de tonos 8-bits estilo arcade
*
*Este modulo sintetiza tonos cortos (onda cuadrada) en memoria utilizando SDL_mixer,
*para recrear el sonido caracteristico de los juegos electronicos de los años 80
*/

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include <SDL_mixer.h>
#include <stdbool.h>

/**
*@enum AudioProfile
*@brief Perfiles sonoros representativos de la epoca 8-bit
*/
typedef enum {
    AUDIO_PROFILE_CLASSIC_ARCADE = 0, //pulso con caida leve (bleeps redondos)
    AUDIO_PROFILE_NES_PULSE, // Duty 12.5/25/50%, timbre nasal tipico de NES
    AUDIO_PROFILE_GAMEBOY, // GB chiptune: pulso apretado y corto
    AUDIO_PROFILE_C64_PULSE, // pulso 25/50% con sustain
    AUDIO_PROFILE_PC_SPEAKER // cuadrada dura, sin slide, mas seca
} AudioProfile;


/**
*@struct AudioSystem
*@brief Representa el estado general del sistema de audio 8-bits
*/
typedef struct{
    Mix_Chunk* toneSamples[8]; //Arreglo de tonos precargados (uno por barra)
    int toneCount; //Cantidad de tonos cargados
    int sampleRate; //Frecuencia de muestreo (Hz)
    int toneDurationMs; // Duracion de cada tono (milisegundos)

    /*parametros segun el perfil activo*/
    float baseFrequenciesHz[8]; //Frecuencias base por barra
    float dutyPerBar01[8]; //Duty-cycle (0..1) por barra
    float slidePerBarSemis[8]; //Deslizamiento en semitonos por barra

    AudioProfile currentProfile; // perfil sonoro activo
} AudioSystem;


/**
*@brief Inicializa SDL_mixer y genera tonos 8-bit para cada barra del ecualizador
*aplica el perfil por defecto (Classic Arcade) y genera los tonos base
*@param audio Puntero al sistema de audio que se inicializara
*@param toneCount Cantidad de tonos a generar (max 8)
*@param sampleRate Frecuencia de muestreo
*@param toneDurationMs Duracion de cada tono en milisegundos
*@return true si la incializacion fue exitosa, false si hubo error
*/
bool audio_init(AudioSystem* audio, int toneCount, int sampleRate, int toneDurationMs);


/**
*@brief Reproduce el tono correspondiente al indice indicado
*@param audio Puntero al sistema de audio
*@param toneIndex Indice del tono
*/
void audio_playTone(AudioSystem* audio, int toneIndex);


/**
*@brief Cambia el perfil sonoro(NES, Arcade, GameBoy, etc) y regenera tonos
*Si el perfil requiere otra sampleRate, reabre el mixer de forma segura
*@param audio Puntero al sistema de audio ya inicializado con audio_init()
*@param profile Perfil deseado
*@return true si pudo aplicarse (y regenerar tonos) correctamente
*/
bool audio_setProfile(AudioSystem* audio, AudioProfile profile);


/**
*@brief Libera los recursos de audio y cierra SDL_mixer
*@param audio Puntero al sistema de audio a cerrar
*/
void audio_shutdown(AudioSystem* audio);

/**
*@brief Escala la duración de las notas segun la cantidad de las mismas
*/
bool audio_setNoteDuration(AudioSystem* audio, int newDurationMs);

#endif // AUDIO_H_INCLUDED
