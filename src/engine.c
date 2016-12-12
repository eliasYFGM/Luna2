#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include "engine.h"
#include "state.h"

// Globals
const struct Game_Config *maincfg;
ALLEGRO_FONT *font;
int keys[ALLEGRO_KEY_MAX];

// The state number that is currently updating
static int current_state;

static struct // Engine variables
{
  ALLEGRO_DISPLAY *display;
  ALLEGRO_BITMAP *buffer;
  ALLEGRO_TIMER *timer;
  ALLEGRO_EVENT_QUEUE *event_queue;
  ALLEGRO_COLOR bg_color;
  int initialized;
  int is_running;
  struct State *states[MAX_STATES];
}
engine;

// Updates the aspect ratio when going full-screen or windowed
static void aspect_ratio_transform(void)
{
  int window_w = al_get_display_width(engine.display);
  int window_h = al_get_display_height(engine.display);

  float sw = (window_w / (float) GAME_W);
  float sh = (window_h / (float) GAME_H);
  float scale = (sw < sh ? sw : sh);

  float scale_w = ((float) GAME_W * scale);
  float scale_h = ((float) GAME_H * scale);
  int scale_x_pos = (window_w - scale_w) / 2;
  int scale_y_pos = (window_h - scale_h) / 2;

  ALLEGRO_TRANSFORM trans;
  al_identity_transform(&trans);
  al_build_transform(&trans, scale_x_pos, scale_y_pos, scale, scale, 0);
  al_use_transform(&trans);
}

int game_init(struct Game_Config *cfg)
{
  if (engine.initialized)
  {
    puts("WARNING: Calling game_init() more than once");
    return 1;
  }

  // Initialize Allegro and stuff
  al_init();

  if (!al_install_keyboard())
  {
    puts("ERROR: Could not initialize the keyboard...");
    return 0;
  }

  if (!al_install_mouse())
  {
    puts("ERROR: Could not initialize the mouse...");
    return 0;
  }

  if (cfg->audio)
  {
    if (!al_install_audio())
    {
      puts("ERROR: Could not initialize audio...");
      return 0;
    }

    if (!al_init_acodec_addon())
    {
      puts("ERROR: Could not initialize acodec addon...");
      return 0;
    }

    al_reserve_samples(1);
  }

  // Add-ons
  if (!al_init_image_addon())
  {
    puts("ERROR: Could not initialize image addon...");
    return 0;
  }

  al_init_font_addon();
  al_init_primitives_addon();

  if (cfg->fullscreen)
  {
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
  }

  // Initialize variables...
  engine.display = al_create_display(cfg->width, cfg->height);

  if (!engine.display)
  {
    puts("ERROR: Could not create a display window...");
    return 0;
  }

  al_set_window_title(engine.display, cfg->title);

  maincfg = cfg;
  aspect_ratio_transform();

  al_add_new_bitmap_flag(ALLEGRO_MAG_LINEAR);

  if (cfg->buffer)
  {
    engine.buffer = al_create_bitmap(cfg->width, cfg->height);
    al_set_new_bitmap_flags(0);
  }

  font = al_create_builtin_font();

  engine.timer = al_create_timer(1.0 / cfg->framerate);
  engine.event_queue = al_create_event_queue();

  set_bg_color(BG_COLOR_DEFAULT);

  engine.initialized = TRUE;

  return 1;
}

void game_run(struct State *first)
{
  int redraw = 0;

  if (engine.is_running)
  {
    puts("WARNING: Calling game_run() more than once");
    return;
  }

  change_state(first);

  // Generate display events
  al_register_event_source(engine.event_queue,
    al_get_display_event_source(engine.display));

  // Timer events
  al_register_event_source(engine.event_queue,
    al_get_timer_event_source(engine.timer));

  // Keyboard events
  al_register_event_source(engine.event_queue, al_get_keyboard_event_source());

  // Mouse events
  al_register_event_source(engine.event_queue, al_get_mouse_event_source());

  al_start_timer(engine.timer);
  engine.is_running = TRUE;

  // Main game loop
  while (engine.is_running)
  {
    ALLEGRO_EVENT event;
    al_wait_for_event(engine.event_queue, &event);

    // Event processing
    engine.states[current_state]->_events(&event);

    // If the close button was pressed...
    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
      engine.is_running = FALSE;
      break;
    }
    else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
    {
      keys[event.keyboard.keycode] = TRUE;

      // Escape key will end the game
      if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
      {
        engine.is_running = FALSE;
        break;
      }

      // F4 key will toggle full-screen (maintains aspect ratio)
      if (event.keyboard.keycode == ALLEGRO_KEY_F4)
      {
        al_stop_timer(engine.timer);

        if (al_get_display_flags(engine.display) & ALLEGRO_FULLSCREEN_WINDOW)
        {
          al_toggle_display_flag(engine.display, ALLEGRO_FULLSCREEN_WINDOW, 0);
        }
        else
        {
          al_toggle_display_flag(engine.display, ALLEGRO_FULLSCREEN_WINDOW, 1);
        }

        aspect_ratio_transform();

        al_start_timer(engine.timer);
      }
    }
    else if (event.type == ALLEGRO_EVENT_KEY_UP)
    {
      keys[event.keyboard.keycode] = FALSE;
    }
    else if (event.type == ALLEGRO_EVENT_TIMER)
    {
      engine.states[current_state]->_update();
      redraw = TRUE;
    }

    if (redraw && al_event_queue_is_empty(engine.event_queue))
    {
      redraw = FALSE;

      if (maincfg->buffer)
      {
        al_set_target_bitmap(engine.buffer);
      }
      else
      {
        al_set_target_backbuffer(engine.display);
      }

      al_clear_to_color(engine.bg_color);

      engine.states[current_state]->_draw();

      if (maincfg->buffer)
      {
        al_set_target_backbuffer(engine.display);
        al_clear_to_color(C_BLACK);
        al_draw_bitmap(engine.buffer, 0, 0, 0);
      }

      al_flip_display();
    }
  }

  while (current_state >= 0)
  {
    engine.states[current_state--]->_end(TRUE);
  }

  al_destroy_display(engine.display);
  al_destroy_timer(engine.timer);
  al_destroy_event_queue(engine.event_queue);
  al_destroy_font(font);

  if (maincfg->buffer)
  {
    al_destroy_bitmap(engine.buffer);
  }
}

void change_state(struct State *s)
{
  if (engine.states[current_state] != NULL)
  {
    engine.states[current_state]->_end(FALSE);
  }

  engine.states[current_state] = s;
}

void push_state(struct State *s)
{
  if (current_state < (MAX_STATES - 1))
  {
    if (engine.states[current_state] != NULL)
    {
      engine.states[current_state]->_pause();
    }

    engine.states[++current_state] = s;
  }
  else
  {
    puts("WARNING: Can't add new state (current_state = MAX_STATES)");
  }
}

void pop_state(void)
{
  if (current_state > 0)
  {
    engine.states[current_state]->_end(FALSE);
    engine.states[--current_state]->_resume();
  }
  else
  {
    puts("WARNING: Can't remove any more states (current_state = 0)");
  }
}

void game_over(void)
{
  engine.is_running = FALSE;
}

void set_bg_color(ALLEGRO_COLOR c)
{
  engine.bg_color = c;
}

void game_rest(double secs)
{
  al_stop_timer(engine.timer);
  al_rest(secs);
  al_start_timer(engine.timer);
}
