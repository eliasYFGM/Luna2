#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "core.h"
#include "state.h"

// Globals
struct Game_Config* main_config;
ALLEGRO_FONT* font;
int keys[ALLEGRO_KEY_MAX];

// The state that is currently updating
static int current_state;

static struct // Game variables
{
  ALLEGRO_DISPLAY* display;
  ALLEGRO_BITMAP* buffer;
  ALLEGRO_TIMER* timer;
  ALLEGRO_EVENT_QUEUE* event_queue;
  ALLEGRO_COLOR bg_color;
  int initialized;
  int is_running;
  struct State* states[MAX_STATES];
}
game;

// Updates the aspect ratio when going full-screen or windowed
static void aspect_ratio_transform()
{
  int window_w = al_get_display_width(game.display);
  int window_h = al_get_display_height(game.display);

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

int game_init(struct Game_Config* config)
{
  if (game.initialized)
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

  if (config->audio)
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

  if (config->fullscreen)
  {
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
  }

  // Initialize variables...
  game.display = al_create_display(config->width, config->height);

  if (!game.display)
  {
    puts("ERROR: Could not create a display window...");
    return 0;
  }

  al_set_window_title(game.display, config->title);

  main_config = config;
  aspect_ratio_transform();

  al_add_new_bitmap_flag(ALLEGRO_MAG_LINEAR);

  if (config->buffer)
  {
    game.buffer = al_create_bitmap(config->width, config->height);
    al_set_new_bitmap_flags(0);
  }

  font = al_create_builtin_font();

  game.timer = al_create_timer(1.0 / config->framerate);
  game.event_queue = al_create_event_queue();

  set_bg_color(BG_COLOR_DEFAULT);

  game.initialized = TRUE;

  return 1;
}

void game_run(struct State* state)
{
  int redraw = 0;

  if (game.is_running)
  {
    puts("WARNING: Calling game_run() more than once");
    return;
  }

  change_state(state, NULL);

  // Generate display events
  al_register_event_source(game.event_queue,
    al_get_display_event_source(game.display));

  // Timer events
  al_register_event_source(game.event_queue,
    al_get_timer_event_source(game.timer));

  // Keyboard events
  al_register_event_source(game.event_queue, al_get_keyboard_event_source());

  // Mouse events
  al_register_event_source(game.event_queue, al_get_mouse_event_source());

  al_start_timer(game.timer);
  game.is_running = TRUE;

  // Main game loop
  while (game.is_running)
  {
    ALLEGRO_EVENT event;
    al_wait_for_event(game.event_queue, &event);

    // Event processing
    game.states[current_state]->_events(&event);

    // If the close button was pressed...
    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
      game.is_running = FALSE;
      break;
    }
    else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
    {
      keys[event.keyboard.keycode] = TRUE;

      // Escape key will end the game
      if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
      {
        game.is_running = FALSE;
        break;
      }

      // F4 key will toggle full-screen (maintains aspect ratio)
      if (event.keyboard.keycode == ALLEGRO_KEY_F4)
      {
        al_stop_timer(game.timer);

        if (al_get_display_flags(game.display) & ALLEGRO_FULLSCREEN_WINDOW)
        {
          al_toggle_display_flag(game.display, ALLEGRO_FULLSCREEN_WINDOW, 0);
        }
        else
        {
          al_toggle_display_flag(game.display, ALLEGRO_FULLSCREEN_WINDOW, 1);
        }

        aspect_ratio_transform();

        al_start_timer(game.timer);
      }
    }
    else if (event.type == ALLEGRO_EVENT_KEY_UP)
    {
      keys[event.keyboard.keycode] = FALSE;
    }
    else if (event.type == ALLEGRO_EVENT_TIMER)
    {
      game.states[current_state]->_update();
      redraw = TRUE;
    }

    if (redraw && al_event_queue_is_empty(game.event_queue))
    {
      redraw = FALSE;

      if (main_config->buffer)
      {
        al_set_target_bitmap(game.buffer);
      }
      else
      {
        al_set_target_backbuffer(game.display);
      }

      al_clear_to_color(game.bg_color);

      game.states[current_state]->_draw();

      if (main_config->buffer)
      {
        al_set_target_backbuffer(game.display);
        al_clear_to_color(C_BLACK);
        al_draw_bitmap(game.buffer, 0, 0, 0);
      }

      al_flip_display();
    }
  }

  int i;
  for (i=0; i<MAX_STATES; ++i)
  {
    if (game.states[i] != NULL)
    {
      game.states[i]->_end(TRUE);
    }
  }

  al_destroy_display(game.display);
  al_destroy_timer(game.timer);
  al_destroy_event_queue(game.event_queue);
  al_destroy_font(font);

  if (main_config->buffer)
  {
    al_destroy_bitmap(game.buffer);
  }
}

void game_over()
{
  game.is_running = FALSE;
}

void set_bg_color(ALLEGRO_COLOR color)
{
  game.bg_color = color;
}

void game_rest(double secs)
{
  al_stop_timer(game.timer);

  al_rest(secs);

  al_start_timer(game.timer);
}

void change_state(struct State* state, void* param)
{
  if (game.states[current_state] != NULL)
  {
    game.states[current_state]->_end(FALSE);
  }

  game.states[current_state] = state;
  game.states[current_state]->_init(param);
}

void push_state(struct State* state, void* param)
{
  if (current_state < (MAX_STATES - 1))
  {
    if (game.states[current_state] != NULL)
    {
      game.states[current_state]->_pause();
    }

    game.states[++current_state] = state;
    game.states[current_state]->_init(param);
  }
  else
  {
    puts("WARNING: Can't add new state (current_state = MAX_STATES)");
  }
}

void pop_state()
{
  if (current_state > 0)
  {
    game.states[current_state]->_end(FALSE);
    game.states[current_state] = NULL;
    game.states[--current_state]->_resume();
  }
  else
  {
    puts("WARNING: Can't remove any more states (current_state = 0)");
  }
}
