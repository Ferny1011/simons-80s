#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

// Constantes de configuración
#define MIN_NOTES 3
#define MAX_NOTES 8
#define MIN_DURATION 2000
#define MAX_DURATION 5000
#define DURATION_STEP 500

// Colores para interfaz
extern SDL_Color colorConfigBg;
extern SDL_Color colorButtonNormal;
extern SDL_Color colorButtonPressed;
extern SDL_Color colorTextWhite;

typedef struct {
    int noteCount;
    int noteDuration;
} GameConfig;

typedef struct {
    SDL_Rect rect;
    char text[64];
    bool isPressed;
} Button;

typedef struct {
    Button noteCountMinusBtn;
    Button noteCountPlusBtn;
    Button durationMinusBtn;
    Button durationPlusBtn;
    Button startGameBtn;
    TTF_Font* font;
    GameConfig* gameConfig;
} ConfigMenu;

void config_init(GameConfig* config);
void config_menu_init(ConfigMenu* menu, GameConfig* config, TTF_Font* font);
void config_menu_cleanup(ConfigMenu* menu);
void config_menu_handle_event(ConfigMenu* menu, SDL_Event* event, bool* shouldChangeScene, int* newScene);
void config_menu_render(ConfigMenu* menu, SDL_Renderer* renderer);

bool config_point_in_button(Button* btn, int x, int y);
void config_render_button(SDL_Renderer* renderer, Button* btn, TTF_Font* font);

#endif // MENU_H_INCLUDED
