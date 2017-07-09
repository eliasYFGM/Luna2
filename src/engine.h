#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#include <allegro5/allegro_font.h>

// Color defines
#define C_BLACK     al_map_rgb(0, 0, 0)
#define C_WHITE     al_map_rgb(255, 255, 255)

// Max states to allocate for the "loaded_states" stack
#define MAX_STATES  10

#ifndef TRUE
#define FALSE       0
#define TRUE        -1
#endif

// state.h
struct State;

struct Engine_Conf
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
  int buffer;
};

// A state can have an extra parameter [*param] when initialized. It is passed
// to state_load() and state_enter() functions.
struct State_Machine
{
  void (*change_state)(struct State *s, void *param);
  void (*push_state)(struct State *s, void *param);
  void (*pop_state)(void);
};

/*****************************************************************************
  Main
*****************************************************************************/

int engine_init(struct Engine_Conf *conf);
void engine_run(struct State *s);

/*****************************************************************************
  Misc.
*****************************************************************************/

void game_over(void);
void set_bg_color(ALLEGRO_COLOR c);
void game_rest(double secs);

/*****************************************************************************
  Globals
*****************************************************************************/

extern ALLEGRO_FONT *font;
extern int keys[ALLEGRO_KEY_MAX];
extern const struct Engine_Conf *MAINCONF;

#define GAME_W      MAINCONF->width
#define GAME_H      MAINCONF->height

// Simple bounding box collision checking (taken from Alex4 source)
#define check_bb_collision(x1,y1,w1,h1,x2,y2,w2,h2) \
  (!(((x1)>=(x2)+(w2)) || ((x2)>=(x1)+(w1)) || \
     ((y1)>=(y2)+(h2)) || ((y2)>=(y1)+(h1))))

#endif // ENGINE_H_INCLUDED
