/**
*@file audio.c
*@brief Implementacion del sistema de audio 8-bits para SIMON 80s
*
*Este modulo sintetiza tonos tipo "chiptune" directamente en memoria
*utilizando SDL_Mixer con formato AUDIO_U8 mono. No usa archivos WAV:
*los tonos se generan por codigo imitando chips clasicos(NES, GameBoy,
*Commodore 64, PC Speaker, etc) mediante ondas de pulso con distintos
*duty-cycles, slides y envolventes
*
*Autor: devFQ
*Fecha: Octubre 2025
*Version: 1.1
*/

#include "../include/audio.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>

/**
*@brief Genera un tono tipo "pulse wave" con duty variable, leve slide y
envolvente
*
*Crea un buffer de audio con formato AUDIO_S16SYS(compatible con la mayoria de
*de dispositivos modernos), emulando el timbre
caracteristico
*de las consolas 8-bit. Usa un duty-cycle configurable, deslizamiento de tono
*y un envelope simple de ataque y decaimiento para evitar clics digitales
*
*@param baseFrequencyHz Frecuencia base del tono
*@param durationMs Duracion total del tono(ms)
*@param sampleRate Frecuencia de muestreo (Hz)
*@param attackMs Duracion de la fase de ataque (ms)
*@param releaseMs Duracion de la fase de decaimiento (ms)
*@param dutyCycle01 Porcentaje del ciclo en alto [0.0 - 1.0]
*@param slideSemitones Deslizamiento en semitonos (negativo = tono descendente)
*@return Mix_Chunk* tone puntero con el tono generado (se libera con free + Mix_FreeChunk)
*/
static Mix_Chunk* generate_pulse_tone(float baseFrequencyHz, int durationMs, int sampleRate, int attackMs, int releaseMs, float dutyCycle01, float slideSemitones){
    const int monoSamples = (durationMs * sampleRate)/1000;
    if(monoSamples <= 0)
        return NULL;

    Sint16* mono = (Sint16*)malloc(monoSamples* sizeof(Sint16));
    if(!mono)
        return NULL;

    if(dutyCycle01 < 0.1f)
        dutyCycle01 = 0.1f;
    if(dutyCycle01 > 0.9f)
        dutyCycle01 = 0.9f;


    const int amplitude = 7000; /*mantener bajo por sensacion "8-bit"*/
    const float invRate = 1.0f / (float)sampleRate;

    const int attackSamples = (attackMs * sampleRate)/ 1000;
    const int releaseSamples = (releaseMs * sampleRate) / 1000;

    float phase = 0.0f;

    for (int n = 0; n < monoSamples; ++n){
        /*progreso 0..1 dentro del tono*/
        float t = (float)n/(float)monoSamples;

        /*Deslizamiento progresivo en semitonos*/
        float slide = slideSemitones * t;
        float freq = baseFrequencyHz * powf(2.0f, slide / 12.0f);

        /*Avance de fase*/
        phase += freq * invRate;
        if(phase >= 1.0f)
            phase -= floorf(phase);

        /*Pulso con duty variable*/
        int sample = (phase < dutyCycle01) ? amplitude : -amplitude;

        /*Envolvente lineal (ataque + release)*/
        float envelope = 1.0f;
        if(n < attackSamples && attackSamples > 0){
            envelope = sqrtf((float)n/(float)attackSamples);
        }else if(n> monoSamples - releaseSamples && releaseSamples > 0){
            int rel = monoSamples - n;
            envelope = sqrtf((float)rel / (float)releaseSamples);
        }

        mono[n] = (Sint16)(sample*envelope);
    }

    const int stereoSample = monoSamples * 2;
    Sint16* stereo = (Sint16*)malloc(stereoSample*sizeof(Sint16));
    if(!stereo){
        free(mono);
        return NULL;
    }
    for(int i = 0, j =0; i < monoSamples; ++i){
        Sint16 v = mono[i];
        stereo[j++]=v;
        stereo[j++] =v;
    }
    free(mono);
    Mix_Chunk* tone = Mix_QuickLoad_RAW((Uint8*)stereo,stereoSample*(int)sizeof(Sint16));
    return tone;
}


/**
*@brief Libera todos los tonos existentes del sistema sin cerrar el mixer
*/
static void free_all_tones(AudioSystem* audio){
    for(int i = 0; i < audio->toneCount; ++i){
        if(audio->toneSamples[i]){
            free(audio->toneSamples[i]->abuf);
            Mix_FreeChunk(audio->toneSamples[i]);
            audio->toneSamples[i] = NULL;
        }
    }
}


/**
*@brief (Re)genera los tonos del sistema segun las tablas actuales (frecuencia, duty,slide)
*@param audio Sistema de audio activo
*@return true si todos los tonos fueron regenerados correctamente
*/
static bool regenerate_tones(AudioSystem* audio){
    free_all_tones(audio);

    Mix_Volume(-1,(int)(MIX_MAX_VOLUME*0.60f)); //nivel global moderado

    for(int i = 0; i < audio->toneCount; ++i){
        const int attackMs = 2;
        const int releaseMs = 18;
        audio->toneSamples[i] = generate_pulse_tone(
            audio->baseFrequenciesHz[i],
            audio->toneDurationMs,
            audio->sampleRate,
            attackMs,
            releaseMs,
            audio->dutyPerBar01[i],
            audio->slidePerBarSemis[i]
        );

        if(!audio->toneSamples[i]){
            fprintf(stderr, "[ERROR] No se pudo regenerar el tono %d\n",i);
            return false;
        }
    }
    return true;
}

/**
 * @brief Actualiza la duración base y aplica el algoritmo de escalado progresivo
 *
 * Implementa el algoritmo donde cada nota adicional reduce la duración un 3%:
 * - 1 nota: 100% de la duración base
 * - 2 notas: 97% de la duración base
 * - 3 notas: 94% de la duración base
 * - etc.
 *
 * @param audio Sistema de audio activo
 * @param baseDurationMs Nueva duración base en milisegundos
 * @return true si se actualizó correctamente
 */

bool audio_setNoteDuration(AudioSystem* audio, int baseDurationMs) {
    if (!audio || baseDurationMs < 2000 || baseDurationMs > 5000) {
        return false;
    }

    // Aplicar algoritmo de escalado: cada nota adicional resta 3%
    float scaleFactor = 1.0f - ((audio->toneCount - 1) * 0.03f);

    // Asegurar que el factor no sea menor al 70% (máximo 10 notas teóricamente)
    if (scaleFactor < 0.7f) {
        scaleFactor = 0.7f;
    }

    // Calcular duración final escalada
    int scaledDuration = (int)(baseDurationMs * scaleFactor);

    printf("[AUDIO] Duración base: %d ms, Factor escala: %.2f, Duración final: %d ms (%d notas)\n",
           baseDurationMs, scaleFactor, scaledDuration, audio->toneCount);

    // Actualizar duración del sistema
    audio->toneDurationMs = scaledDuration;

    // Regenerar todos los tonos con la nueva duración
    return regenerate_tones(audio);
}

/**
*@brief Carga los parametros de frecuencia/duty/slide segun el perfil activo
*@param audio Sistema de audio donde se aplicaran los parametros
*@param profile perfil seleccionado
*/
static void apply_profile_defults(AudioSystem* audio, AudioProfile profile){
    int suggestedDur = audio->toneDurationMs;
    int userBaseDuration = audio->toneDurationMs;

    switch (profile)
    {
    case AUDIO_PROFILE_CLASSIC_ARCADE:
        suggestedDur = 110;
        {
            float f[8] = {392.0f,523.25f,659.25f,784.0f,880.0f,987.77f,1046.5f,1318.5f};
            float d[8] = {0.25f,0.25f,0.50f,0.25f,0.25f,0.50f,0.25f,0.50f};
            float s[8] = {-0.35f,-0.30f,-0.40f,-0.30f,-0.35f,-0.25f,-0.40f,-0.30f};
            for(int i=0;i<audio->toneCount;i++){audio->baseFrequenciesHz[i]=f[i];
                audio->dutyPerBar01[i]=d[i];audio->slidePerBarSemis[i]=s[i];}
        }
        break;

    case AUDIO_PROFILE_NES_PULSE:
        suggestedDur = 100;
        {
            float f[8] = {440.0f,523.25f,659.25f,880.0f,1046.5f,1318.5f,1568.0f,1760.0f};
            float d[8] = {0.125f,0.25f,0.50f,0.125f,0.25f,0.50f,0.125f,0.25f};
            float s[8] = {-0.30f,-0.30f,-0.35f,-0.30f,-0.30f,-0.35f,-0.30f,-0.30f};
            for(int i=0;i<audio->toneCount;i++){audio->baseFrequenciesHz[i]=f[i];
                audio->dutyPerBar01[i]=d[i];audio->slidePerBarSemis[i]=s[i];}
        }
        break;

    case AUDIO_PROFILE_GAMEBOY:
        suggestedDur = 90;
        {
            float f[8] = {392.0f,494.0f,587.3f,784.0f,880.0f,988.0f,1174.7f,1318.5f};
            float d[8] = {0.125f,0.25f,0.125f,0.25f,0.125f,0.25f,0.125f,0.25f};
            float s[8] = {-0.15f,-0.12f,-0.18f,-0.15f,-0.12f,-0.18f,-0.15f,-0.12f};
            for(int i=0;i<audio->toneCount;i++){audio->baseFrequenciesHz[i]=f[i];
                audio->dutyPerBar01[i]=d[i];audio->slidePerBarSemis[i]=s[i];}
        }
        break;

    case AUDIO_PROFILE_C64_PULSE:
        suggestedDur = 120;
        {
            float f[8] = {330.0f,392.0f,494.0f,659.25f,784.0f,987.77f,1046.5f,1318.5f};
            float d[8] = {0.25f,0.50f,0.25f,0.50f,0.25f,0.50f,0.25f,0.50f};
            float s[8] = {-0.25f,-0.25f,-0.25f,-0.30f,-0.25f,-0.25f,-0.30f,-0.25f};
            for(int i=0;i<audio->toneCount;i++){audio->baseFrequenciesHz[i]=f[i];
                audio->dutyPerBar01[i]=d[i];audio->slidePerBarSemis[i]=s[i];}
        }
        break;

    case AUDIO_PROFILE_PC_SPEAKER:
        suggestedDur = 100;
        {
            float f[8] = {400.0f,500.0f,650.0f,800.0f,900.0f,1000.0f,1200.0f,1400.0f};
            float d[8] = {0.50f,0.50f,0.50f,0.50f,0.50f,0.50f,0.50f,0.50f};
            float s[8] = {0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
            for(int i=0;i<audio->toneCount;i++){audio->baseFrequenciesHz[i]=f[i];
                audio->dutyPerBar01[i]=d[i];audio->slidePerBarSemis[i]=s[i];}
        }
        break;
    }

    audio->toneDurationMs = userBaseDuration;
}


bool audio_init(AudioSystem* audio, int toneCount, int sampleRate,int toneDurationMs){
    if(toneCount < 1)
        toneCount = 1;
    if(toneCount > 8)
        toneCount = 8;

    audio->toneCount = toneCount;
    audio->sampleRate = sampleRate;
    audio->toneDurationMs = toneDurationMs;
    audio->currentProfile = AUDIO_PROFILE_CLASSIC_ARCADE;

    if (Mix_OpenAudio(sampleRate, AUDIO_S16SYS, 2, 512) < 0) {
    fprintf(stderr, "[ERROR] Mix_OpenAudio: %s\n", Mix_GetError());
    return false;
}
    int gotFreq = 0, gotCh = 0; Uint16 gotFmt = 0;
    Mix_QuerySpec(&gotFreq,&gotFmt,&gotCh);
    printf("[AUDIO] MIXER activo: freq=%d, fmt=0x%X, ch=%d\n",gotFreq,gotFmt,gotCh);

    apply_profile_defults(audio,audio->currentProfile);
    if(!regenerate_tones(audio)){
        fprintf(stderr,"[ERROR] Fallo la generacion de tonos.\n");
        Mix_CloseAudio();
        return false;
    }

    printf("[INFO] Sistema de audio inicializado(%d tonos, %d Hz, %d ms)\n",toneCount, sampleRate, toneDurationMs);
    return true;
}


bool audio_setProfile(AudioSystem* audio, AudioProfile profile){
    if(!audio)
        return false;
    apply_profile_defults(audio,profile);
    audio->currentProfile = profile;
    return regenerate_tones(audio);

}


void audio_playTone(AudioSystem* audio, int toneIndex){
    if(!audio || toneIndex < 0 || toneIndex >= audio->toneCount)
        return;
    Mix_PlayChannel(-1,audio->toneSamples[toneIndex],0);
}


void audio_shutdown(AudioSystem* audio){
    free_all_tones(audio);
    Mix_CloseAudio();
    printf("[INFO]Sistema de audio 8-bit finalizado correctamente.\n");
}
