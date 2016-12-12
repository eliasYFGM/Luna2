/*
  scarestate.c

  Blocks the game for a second before entering the "creepy" part.
*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include "scare_state.h"
#include "../engine.h"

static ALLEGRO_BITMAP *image;
static ALLEGRO_SAMPLE *noise;

static int step_count = 0;

static void state_init(void)
{
  image = al_load_bitmap("data/zalgopie.png");
  noise = al_load_sample("data/noise.wav");

  if (noise != NULL)
  {
    al_play_sample(noise, 1.0, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
  }

  set_bg_color(C_BLACK);
}

static void state_end(int exiting)
{
  al_destroy_bitmap(image);
  al_destroy_sample(noise);

  // Change the game's background color to dark red
  set_bg_color(al_map_rgb(30, 0, 0));
}

static void state_pause(void)
{
}

static void state_resume(void)
{
}

static void state_events(ALLEGRO_EVENT *ev)
{
}

static void state_update(void)
{
  ++step_count;

  if (step_count >= maincfg->framerate)
  {
    // Back to the game
    pop_state();
  }
}

static void state_draw(void)
{
  al_draw_text(font, C_WHITE, GAME_W / 2, GAME_H / 2,
    ALLEGRO_ALIGN_CENTER, "zalgopie.png");

  if (image)
  {
    al_draw_bitmap(image, 0, 0, 0);
  }
}

struct State* Scare_State(void)
{
  static struct State state =
  {
    state_end,
    state_pause,
    state_resume,
    state_events,
    state_update,
    state_draw
  };

  state_init();

  return &state;
}
