#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#include <allegro5/allegro_font.h>

// Color defines
#define C_BLACK     al_map_rgb(0, 0, 0)
#define C_WHITE     al_map_rgb(255, 255, 255)

// Max states to allocate
#define MAX_STATES  8

#ifndef TRUE
#define FALSE   0
#define TRUE    -1
#endif

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

// Defined in state.h
struct State;

/*******************************************************************************
  Main
*******************************************************************************/

// Initializes the engine with a configuration structure.
int engine_init(struct Engine_Conf *conf);

// Run the engine with a starting state, until [engine_active = FALSE].
void engine_run(struct State *s);

/*******************************************************************************
  State manipulation
*******************************************************************************/

// change_state() - Changes the state directly to another.
// "param" is a parameter that will be passed to the new state. Use NULL to not
// pass anything.
void change_state(struct State *s, void *param);

// push_state() - Add a new state to the stack (previous one is 'paused').
// "param" is a parameter that will be passed to the new state. Use NULL to not
// pass anything.
void push_state(struct State *s, void *param);

// pop_state() - removes the last state added with push_state().
void pop_state(void);

/*******************************************************************************
  Misc functions
*******************************************************************************/
void set_bg_color(ALLEGRO_COLOR c);
void game_rest(double secs);

/*******************************************************************************
  Globals
*******************************************************************************/

// Whether the engine is active (started or running),
// set to FALSE to stop.
extern int engine_active;

// Default Allegro font
extern ALLEGRO_FONT *font;

// Array holding key presses, for state_update().
extern int keys[ALLEGRO_KEY_MAX];

// Pointer to the original settings (in main.c).
extern const struct Engine_Conf *mainconf;

#define GAME_W    mainconf->width
#define GAME_H    mainconf->height

// Simple bounding box collision checking (taken from Alex4 source)
#define check_bb_collision(x1,y1,w1,h1,x2,y2,w2,h2) \
  (!( ((x1)>=(x2)+(w2)) || ((x2)>=(x1)+(w1)) || \
      ((y1)>=(y2)+(h2)) || ((y2)>=(y1)+(h1)) ))

#endif // ENGINE_H_INCLUDED
