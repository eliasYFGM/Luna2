/**
  * state_scare.c
  *
  * Blocks the game for a second before entering the "creepy" part.
  */

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include "state_scare.h"
#include "engine.h"

static ALLEGRO_BITMAP *image;
static ALLEGRO_SAMPLE *noise;

static int step_count = 0;

/*******************************************************************************
  state_init(param) - State initialization
  Executed ONLY ONCE by: change_state(), push_state()
*******************************************************************************/
static void
state_init(void *param)
{
}

/*******************************************************************************
  state_end() - State "shutdown"
  Executed when [engine_active = FALSE]
*******************************************************************************/
static void
state_end(void)
{
}

/*******************************************************************************
  state_enter(param)
  Executed by: change_state(), push_state()
*******************************************************************************/
static void
state_enter(void *param)
{
  image = al_load_bitmap("data/zalgopie.png");
  noise = al_load_sample("data/noise.wav");

  if (noise != NULL)
  {
    al_play_sample(noise, 1.0, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
  }

  set_bg_color(C_BLACK);
}

/*******************************************************************************
  state_exit()
  Executed by: change_state(), pop_state()
*******************************************************************************/
static void
state_exit(void)
{
  // Change the game's background color to dark red
  set_bg_color(al_map_rgb(30, 0, 0));

  al_destroy_bitmap(image);
  al_destroy_sample(noise);
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
  ++step_count;

  if (step_count >= mainconf->framerate)
  {
    // Back to the game
    pop_state();
  }
}

/*******************************************************************************
  state_draw()
  For drawing to the screen - huds, sprites, backgrounds, etc.
*******************************************************************************/
static void
state_draw(void)
{
  al_draw_text(font, C_WHITE, GAME_W / 2, GAME_H / 2,
    ALLEGRO_ALIGN_CENTER, "zalgopie.png");

  if (image)
  {
    al_draw_bitmap(image, 0, 0, 0);
  }
}

/*******************************************************************************
  Definition of the state function
*******************************************************************************/
struct State*
State_Scare(void)
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
