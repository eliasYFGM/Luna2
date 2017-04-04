/**
  * state_dead.c
  *
  * This is the final part of the game. Usually it just shows an image of Luna
  * falling dead, and pauses for about a half-minute.
  */

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include "state_dead.h"
#include "engine.h"

static ALLEGRO_BITMAP *dead_img;
static ALLEGRO_AUDIO_STREAM *music;
static float alpha = 0;

/*******************************************************************************
  state_init(param) - State initialization
  Executed ONLY ONCE by: change_state(), push_state()
*******************************************************************************/
static void
state_init(void *param)
{
  dead_img = al_load_bitmap("data/dead.tga");
  music = al_load_audio_stream("data/youdied.ogg", 2, 4086);

  if (music != NULL)
  {
    al_attach_audio_stream_to_mixer(music, al_get_default_mixer());
  }

  set_bg_color(C_BLACK);
}

/*******************************************************************************
  state_end() - State "shutdown"
  Executed when [engine_active = FALSE]
*******************************************************************************/
static void
state_end(void)
{
  al_destroy_bitmap(dead_img);

  if (music != NULL)
  {
    al_destroy_audio_stream(music);
  }
}

/*******************************************************************************
  state_enter(param)
  Executed by: change_state(), push_state()
*******************************************************************************/
static void
state_enter(void *param)
{
}

/*******************************************************************************
  state_exit()
  Executed by: change_state(), pop_state()
*******************************************************************************/
static void
state_exit(void)
{
}

/*******************************************************************************
  state_pause()
  Executed by: push_state()
*******************************************************************************/
static void
state_pause(void)
{
}

/*******************************************************************************
  state_resume()
  Executed by: pop_state()
*******************************************************************************/
static void
state_resume(void)
{
}

/*******************************************************************************
  state_events(ev)
  For event handling - keyboard, mouse, joystick, etc.
*******************************************************************************/
static void
state_events(ALLEGRO_EVENT *ev)
{
}

/*******************************************************************************
  state_update()
  Runs every step/frame - for game logic
*******************************************************************************/
static void
state_update(void)
{
  // Fade-in animation
  if (alpha < 1.0)
  {
    alpha += 0.05;
  }
  else
  {
    game_rest(25.0);
    engine_active = FALSE;
  }
}

/*******************************************************************************
  state_draw()
  For drawing to the screen - huds, sprites, backgrounds, etc.
*******************************************************************************/
static void
state_draw(void)
{
  al_draw_tinted_bitmap(dead_img, al_map_rgba_f(1.0 * alpha, 1.0 * alpha,
    1.0 * alpha, alpha), 0, 0, 0);
}

/*******************************************************************************
  Definition of the state function
*******************************************************************************/
struct State*
State_Dead(void)
{
  static struct State state =
  {
    state_init,
    state_end,
    state_enter,
    state_exit,
    state_pause,
    state_resume,
    state_events,
    state_update,
    state_draw
  };

  return &state;
}
