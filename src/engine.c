#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include "engine.h"
#include "state.h"

// Globals
ALLEGRO_FONT *font;
int keys[ALLEGRO_KEY_MAX];
const struct Engine_Conf *MAINCONF;

static struct // Engine variables
{
  ALLEGRO_DISPLAY *display;
  ALLEGRO_BITMAP *buffer;
  ALLEGRO_TIMER *timer;
  ALLEGRO_EVENT_QUEUE *event_queue;
  ALLEGRO_COLOR bg_color;
  int initialized;
  struct State *states[MAX_STATES * 2];     // Stack of states
  struct State *loaded_states[MAX_STATES];  // Initialized states
  struct State_Machine sm;
}
engine;

static int current_state, loaded_count, engine_active;

static void change_state(struct State *s, void *param)
{
  if (!s->loaded)
    {
      if (loaded_count < MAX_STATES)
        {
          s->_load(param);
          s->loaded = TRUE;
          engine.loaded_states[loaded_count++] = s;
        }
      else
        {
          puts("change_state(): Unable to load state (reached MAX_STATES)");
          return;
        }
    }

  s->_enter(param);

  if (engine.states[current_state] != NULL)
    {
      engine.states[current_state]->_exit();
    }

  engine.states[current_state] = s;

  al_flush_event_queue(engine.event_queue);
}

static void push_state(struct State *s, void *param)
{
  if (current_state == (MAX_STATES * 2))
    {
      puts("push_state(): State stack is full");
      return;
    }

  if (!s->loaded)
    {
      if (loaded_count < MAX_STATES)
        {
          s->_load(param);
          s->loaded = TRUE;
          engine.loaded_states[loaded_count++] = s;
        }
      else
        {
          puts("change_state(): Unable to load state (reached MAX_STATES)");
          return;
        }
    }

  s->_enter(param);

  if (engine.states[current_state] != NULL)
    {
      engine.states[current_state]->_pause();
    }

  engine.states[++current_state] = s;

  al_flush_event_queue(engine.event_queue);
}

static void pop_state(void)
{
  if (current_state > 0)
    {
      engine.states[current_state]->_exit();
      engine.states[--current_state]->_resume();
    }
}

// Updates the aspect ratio when going full-screen or windowed
static void
aspect_ratio_transform(void)
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

int engine_init(struct Engine_Conf *conf)
{
  if (engine.initialized)
    {
      return 1;
    }

  // Initialize Allegro and stuff
  al_init();

  if (!al_install_keyboard())
    {
      puts("engine_init(): Failed to initialize the keyboard...");
      return 0;
    }

  if (!al_install_mouse())
    {
      puts("engine_init(): Failed to initialize the mouse...");
      return 0;
    }

  if (al_install_audio())
    {
      if (!al_init_acodec_addon())
        {
          puts("engine_init(): Failed to initialize codecs...");
        }

      al_reserve_samples(1);
    }
  else
    {
      puts("engine_init(): Failed to initialize audio...");
    }

  // Add-ons
  if (!al_init_image_addon())
    {
      puts("engine_init(): Failed to initialize image addon...");
      return 0;
    }

  al_init_font_addon();
  al_init_primitives_addon();

  if (conf->fullscreen)
    {
      al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    }

  // Initialize variables...
  engine.display = al_create_display(conf->width, conf->height);

  if (!engine.display)
    {
      puts("engine_init(): Failed to create a display window...");
      return 0;
    }

  al_set_window_title(engine.display, conf->title);

  MAINCONF = conf;
  aspect_ratio_transform();

  al_add_new_bitmap_flag(ALLEGRO_MAG_LINEAR);

  if (conf->buffer)
    {
      engine.buffer = al_create_bitmap(conf->width, conf->height);
      al_set_new_bitmap_flags(0);
    }

  font = al_create_builtin_font();

  engine.timer = al_create_timer(1.0 / conf->framerate);
  engine.event_queue = al_create_event_queue();

  engine.sm.change_state = change_state;
  engine.sm.push_state = push_state;
  engine.sm.pop_state = pop_state;

  set_bg_color(al_map_rgb(192, 192, 192));

  srand(time(NULL));

  engine.initialized = TRUE;

  return 1;
}

void engine_run(struct State *s)
{
  int redraw = FALSE;

  if (engine_active)
    {
      return;
    }

  change_state(s, NULL);

  // Generate display events
  al_register_event_source(engine.event_queue,
                           al_get_display_event_source(engine.display));

  // Timer events
  al_register_event_source(engine.event_queue,
                           al_get_timer_event_source(engine.timer));

  // Keyboard events
  al_register_event_source(engine.event_queue,
                           al_get_keyboard_event_source());

  // Mouse events
  al_register_event_source(engine.event_queue,
                           al_get_mouse_event_source());

  al_start_timer(engine.timer);
  engine_active = TRUE;

  // Main game loop
  while (engine_active)
    {
      ALLEGRO_EVENT event;
      al_wait_for_event(engine.event_queue, &event);

      // Event processing
      engine.states[current_state]->_events(&event, &engine.sm);

      // If the close button was pressed...
      if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
          engine_active = FALSE;
          break;
        }
      else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
          keys[event.keyboard.keycode] = TRUE;

          // F4 key will toggle full-screen
          if (event.keyboard.keycode == ALLEGRO_KEY_F4)
            {
              al_stop_timer(engine.timer);

              if (al_get_display_flags(engine.display)
                  & ALLEGRO_FULLSCREEN_WINDOW)
                {
                  al_toggle_display_flag(engine.display,
                                         ALLEGRO_FULLSCREEN_WINDOW, 0);
                }
              else
                {
                  al_toggle_display_flag(engine.display,
                                         ALLEGRO_FULLSCREEN_WINDOW, 1);
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
          engine.states[current_state]->_update(&engine.sm);
          redraw = TRUE;
        }

      if (redraw && engine_active
          && al_is_event_queue_empty(engine.event_queue))
        {
          redraw = FALSE;

          if (MAINCONF->buffer)
            {
              al_set_target_bitmap(engine.buffer);
            }
          else
            {
              al_set_target_backbuffer(engine.display);
            }

          al_clear_to_color(engine.bg_color);

          engine.states[current_state]->_draw(&engine.sm);

          if (MAINCONF->buffer)
            {
              al_set_target_backbuffer(engine.display);
              al_clear_to_color(C_BLACK);
              al_draw_bitmap(engine.buffer, 0, 0, 0);
            }

          al_flip_display();
        }
    }

  while (loaded_count > 0)
    {
      engine.loaded_states[--loaded_count]->_free();
    }

  al_destroy_display(engine.display);
  al_destroy_timer(engine.timer);
  al_destroy_event_queue(engine.event_queue);
  al_destroy_font(font);

  if (MAINCONF->buffer)
    {
      al_destroy_bitmap(engine.buffer);
    }
}

void game_over(void)
{
  engine_active = FALSE;
}

void set_bg_color(ALLEGRO_COLOR c)
{
  engine.bg_color = c;
}

void game_rest(double secs)
{
  al_rest(secs);
  al_flush_event_queue(engine.event_queue);
}
