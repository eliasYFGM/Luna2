#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#include <allegro5/allegro_font.h>

// Color defines
#define C_BLACK     al_map_rgb(0, 0, 0)
#define C_WHITE     al_map_rgb(255, 255, 255)

#define BG_COLOR_DEFAULT  al_map_rgb(192, 192, 192)

// Max states to allocate
#define MAX_STATES  8

#ifndef TRUE
#define FALSE   0
#define TRUE    -1
#endif

// Defined in state.h
struct State;

struct Game_Config
{
  // Argument list
  int argc;
  char **argv;

  // Other needed parameters
  char *title;
  int width;
  int height;
  int framerate;
  int fullscreen;
  int audio;
  int buffer;
};

// Main
int game_init(struct Game_Config *cfg);
void game_run(struct State *first);

// State routines
void change_state(struct State *s);
void push_state(struct State *s);
void pop_state(void);

// Other
void game_over(void);
void set_bg_color(ALLEGRO_COLOR c);
void game_rest(double secs);

// Default fixed-width font
extern ALLEGRO_FONT *font;

// Array holding key presses, only for the state's _update() function
extern int keys[ALLEGRO_KEY_MAX];

// Pointer to the original settings (in main.c)
extern const struct Game_Config *maincfg;

#define GAME_W    maincfg->width
#define GAME_H    maincfg->height

// Simple bounding box collision checking (taken from Alex4 source)
#define check_bb_collision(x1,y1,w1,h1,x2,y2,w2,h2) \
  (!( ((x1)>=(x2)+(w2)) || ((x2)>=(x1)+(w1)) || \
      ((y1)>=(y2)+(h2)) || ((y2)>=(y1)+(h1)) ))

#endif // ENGINE_H_INCLUDED
