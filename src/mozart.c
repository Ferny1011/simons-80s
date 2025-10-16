#include "../include/mozart.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
*@brief
*@param
*@param
*@return
*/
bool cargarSecMozart(MozartGame* g, const char* path)
{
    FILE* f = fopen(path, "r");

    if (!f)
    {
        fprintf(stderr, "No se pudo abrir secuencia: %s\n", path);
        g->loaded = false;
        return false;
    }

    g->seqLen = 0;

    while (fscanf(f, "%d", &g->seq[g->seqLen]) == 1 && g->seqLen < MAX_MOZART_SEQ)
    {
        if (g->seq[g->seqLen] < 0 || g->seq[g->seqLen] > 3)
        {
            fprintf(stderr, "Valor fuera de rango en linea %d, ignorado.\n", g->seqLen + 1);
            continue;
        }

        g->seqLen++;
    }

    fclose(f);

    g->loaded = (g->seqLen > 0);

    return g->loaded;
}

/**
*@brief
*@param
*@return
*/
void iniciarMozartM(MozartGame* g)
{
    g->current = 0;
    g->playerStep = 0;
    g->timer = 0;
    g->state = MOZ_SHOW;
    g->waiting = false;

    if (!g->loaded)
    {
        fprintf(stderr, "No hay secuencia cargada para Mozart.\n");
    }
}

/**
*@brief
*@param
*@param
*@param
*@param
*@return
*/
void actualizarMozartM(MozartGame* g, EqRenderer* eq, AudioSystem* audio, InputController* in)
{
    if (!g->loaded)
        return;

    Uint32 now = SDL_GetTicks();

    int showDelay = 600;

    switch (g->state)
    {
        //mostrar secuencia
        case MOZ_SHOW:
            if (!g->waiting) {
                eq_triggerPulse(eq, g->seq[g->current]);
                audio_playTone(audio, g->seq[g->current]);
                g->timer = now;
                g->waiting = true;
            }
            else if (now - g->timer > (Uint32)showDelay)
            {
                g->current++;
                g->waiting = false;
            }

            if (g->current >= g->seqLen)
            {
                g->state = MOZ_WAIT;
                g->playerStep = 0;
            }
            break;

        //esperar jgdr
        case MOZ_WAIT:
        {
            SDL_Event e;
            while (SDL_PollEvent(&e))
            {
                int hit = input_process_event(in, &e, false, true, (inputHitTestFn)eq_detectHit, eq);
                if (hit >= 0)
                {
                    eq_triggerPulse(eq, hit);
                    audio_playTone(audio, hit);

                    if (hit == g->seq[g->playerStep])
                    {
                        g->playerStep++;

                        if (g->playerStep >= g->seqLen)
                        {
                                g->state = MOZ_WIN;
                                g->timer = now;
                        }
                    }
                    else
                    {
                        g->state = MOZ_FAIL;
                        g->timer = now;
                    }
                }
            }
            break;
        }

        //error jgdr
        case MOZ_WIN:
            if (now - g->timer > 1000)
            {
                printf("Secuencia completada correctamente (Mozart mode)\n");
                g->state = MOZ_SHOW;
                g->current = 0;
            }
            break;

        case MOZ_FAIL:
            if (now - g->timer > 1000)
            {
                printf("Fallaste la secuencia (Mozart mode)\n");
                g->state = MOZ_SHOW;
                g->current = 0;
                g->playerStep = 0;
            }
            break;
    }
}

/**
*@brief
*@param
*@param
*@return
*/
void renderMozartM(MozartGame* g, EqRenderer* eq)
{
    eq_render(eq);
}
