/*
  deadstate.c

  This is the final part of the game. Usually it just shows an image of Luna
  falling dead, and pauses for about a half-minute.
*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include "dead_state.h"
#include "../engine.h"

static ALLEGRO_BITMAP *dead_img;
static ALLEGRO_AUDIO_STREAM *music;
static float alpha = 0;

static void state_init(void)
{
  dead_img = al_load_bitmap("data/dead.tga");
  music = al_load_audio_stream("data/youdied.ogg", 2, 4086);

  if (music != NULL)
  {
    al_attach_audio_stream_to_mixer(music, al_get_default_mixer());
  }

  set_bg_color(C_BLACK);
}

static void state_end(int exiting)
{
  al_destroy_bitmap(dead_img);

  if (music != NULL)
  {
    al_destroy_audio_stream(music);
  }
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
  // Fade-in animation
  if (alpha < 1.0)
  {
    alpha += 0.05;
  }
  else
  {
    game_rest(25.0);
    game_over();
  }
}

static void state_draw(void)
{
  al_draw_tinted_bitmap(dead_img, al_map_rgba_f(1.0 * alpha, 1.0 * alpha,
    1.0 * alpha, alpha), 0, 0, 0);
}

struct State* Dead_State(void)
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
