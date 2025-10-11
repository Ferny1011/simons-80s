#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <math.h>
#undef main

// crea un beep 8-bit cuadrado
static Mix_Chunk* make_square_u8(int freq_hz, int ms, int volume_0_255) {
    const int sr = 11025;
    int n = (sr * ms) / 1000;
    if (n <= 0) n = 1;
    Uint8* data = (Uint8*)malloc(n);
    if (!data) return NULL;

    double period = (double)sr / (double)freq_hz;
    Uint8 hi = (Uint8)(128 + (volume_0_255/2));
    Uint8 lo = (Uint8)(128 - (volume_0_255/2));
    for (int i=0; i<n; ++i) {
        data[i] = (fmod(i, period) < period/2.0) ? hi : lo;
    }

    Mix_Chunk* ch = (Mix_Chunk*)malloc(sizeof(Mix_Chunk));
    ch->allocated = 1;
    ch->abuf = data;
    ch->alen = (Uint32)n;
    ch->volume = MIX_MAX_VOLUME;
    return ch;
}

int main(int argc, char* argv[]) {
    // VIDEO + TTF
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) { fprintf(stderr,"SDL_Init: %s\n", SDL_GetError()); return 1; }
    if (TTF_Init() == -1) { fprintf(stderr,"TTF_Init: %s\n", TTF_GetError()); SDL_Quit(); return 1; }

    // SDL_mixer 8-bit mono ¿
    if (Mix_OpenAudio(11025, AUDIO_U8, 1, 1024) < 0) {
        fprintf(stderr,"Mix_OpenAudio: %s\n", Mix_GetError());
        TTF_Quit(); SDL_Quit(); return 1;
    }

    SDL_Window* win = SDL_CreateWindow("SIMON 80s - prueba audio", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Texto
    TTF_Font* font = TTF_OpenFont("fnt/PressStart2P.ttf", 32);
    SDL_Color col = {255,128,255,255};
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, "SIMON 80s", col);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    SDL_Rect dst = {40,40,0,0}; SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);

    // ---- preparar 4 tonos (Z, X, C, V) ----
    Mix_Chunk* toneZ = make_square_u8(440, 250, 200);  // A4
    Mix_Chunk* toneX = make_square_u8(523, 250, 200);  // C5
    Mix_Chunk* toneC = make_square_u8(659, 250, 200);  // E5
    Mix_Chunk* toneV = make_square_u8(784, 250, 200);  // G5

    int running = 1; SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = 0;
                if (e.key.keysym.sym == SDLK_z) Mix_PlayChannel(-1, toneZ, 0);
                if (e.key.keysym.sym == SDLK_x) Mix_PlayChannel(-1, toneX, 0);
                if (e.key.keysym.sym == SDLK_c) Mix_PlayChannel(-1, toneC, 0);
                if (e.key.keysym.sym == SDLK_v) Mix_PlayChannel(-1, toneV, 0);
            }
        }
        SDL_SetRenderDrawColor(ren, 0,0,0,255);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, &dst);
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    // liberar
    Mix_FreeChunk(toneZ); Mix_FreeChunk(toneX); Mix_FreeChunk(toneC); Mix_FreeChunk(toneV);
    SDL_DestroyTexture(tex);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
