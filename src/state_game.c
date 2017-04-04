/**
  * state_game.c
  *
  * The main state that controls everything in the game.
  * Does NOT initialize the modules - for that see game.c
  */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
#include "engine.h"
#include "player.h"
#include "state_game.h"
#include "state_scare.h"
#include "state_dead.h"

struct Tile *vtiles[500];
int vtile_count = 0;

float view_x = 0;
float view_y = 0;

// Structure holding default player keys
static struct Keys default_keys = { 0, 0, 0, 0 };

// Player
static struct Player *player;

// Full list of tiles
static struct Tile *tile_list;
static int tile_count = 0;

static struct // Data
{
  ALLEGRO_BITMAP *bg;
  ALLEGRO_BITMAP *tiles;
  ALLEGRO_BITMAP *tilesred;
  ALLEGRO_BITMAP *cracks;
  ALLEGRO_BITMAP *text;
  ALLEGRO_AUDIO_STREAM *music;
}
game;

static int crack_level = 0;
static int step_count = 0;

// Used to fade out
static float alpha = 0;

// Level width
static int max_width = 0;

static int creepy = FALSE;
static int rush = FALSE;

/*******************************************************************************
  state_init(param) - State initialization
  Executed ONLY ONCE by: change_state(), push_state()
*******************************************************************************/
static void
state_init(void *param)
{
  int i;
  FILE *file_level;

  // Level loading
  file_level = fopen("data/level.txt", "r");

  // First: Count number of tiles (lines) in the file
  while (!feof(file_level))
  {
    int c = fgetc(file_level);

    if (c == '\n')
    {
      ++tile_count;
    }
  }

  // Allocate memory for the tiles
  tile_list = malloc(sizeof(struct Tile) * tile_count);

  fseek(file_level, 0, SEEK_SET);

  // Read tile data
  for (i=0; i<tile_count; ++i)
  {
    int v; // First value of each tile in the .txt is not used

    fscanf(file_level, "%d %d %d %d %d %f %f\n", &v,
           &tile_list[i].left,
           &tile_list[i].top,
           &tile_list[i].w,
           &tile_list[i].h,
           &tile_list[i].x,
           &tile_list[i].y);

    if (tile_list[i].x > max_width)
    {
      max_width = tile_list[i].x + tile_list[i].w;
    }
  }

  fclose(file_level);

  // Load graphics and music
  game.bg = al_load_bitmap("data/bg.tga");
  game.tiles = al_load_bitmap("data/tiles.tga");
  game.tilesred = al_load_bitmap("data/tilesred.tga");
  game.cracks = al_load_bitmap("data/cracks.tga");
  game.text = al_load_bitmap("data/text.tga");
  game.music = al_load_audio_stream("data/music1.ogg", 2, 4096);

  if (game.music != NULL)
  {
    al_attach_audio_stream_to_mixer(game.music, al_get_default_mixer());
    al_set_audio_stream_playmode(game.music, ALLEGRO_PLAYMODE_LOOP);

    // Loop points for the music
    al_set_audio_stream_loop_secs(game.music, 20.274,
      al_get_audio_stream_length_secs(game.music));
  }

  srand(time(NULL));

  player = create_player(100, 100, &default_keys);
}

/*******************************************************************************
  state_end() - State "shutdown"
  Executed when [engine_active = FALSE]
*******************************************************************************/
static void
state_end(void)
{
  al_destroy_bitmap(game.bg);
  al_destroy_bitmap(game.tiles);
  al_destroy_bitmap(game.tilesred);
  al_destroy_bitmap(game.cracks);
  al_destroy_bitmap(game.text);

  if (game.music != NULL)
  {
    al_destroy_audio_stream(game.music);
  }

  free(tile_list);

  destroy_player(player);
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
  if (game.music != NULL)
  {
    al_destroy_audio_stream(game.music);

    game.music = al_load_audio_stream("data/music2.ogg", 2, 4096);

    if (game.music != NULL)
    {
      al_attach_audio_stream_to_mixer(game.music, al_get_default_mixer());
      al_set_audio_stream_playmode(game.music, ALLEGRO_PLAYMODE_LOOP);
    }
  }
}

/*******************************************************************************
  state_events(ev)
  For event handling - keyboard, mouse, joystick, etc.
*******************************************************************************/
static void
state_events(ALLEGRO_EVENT *ev)
{
  if (ev->type == ALLEGRO_EVENT_KEY_DOWN)
  {
    if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT)
    {
      default_keys.left = TRUE;
    }

    if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT)
    {
      default_keys.right = TRUE;
    }

    if (ev->keyboard.keycode == ALLEGRO_KEY_UP)
    {
      default_keys.jump = TRUE;
    }

    if (ev->keyboard.keycode == ALLEGRO_KEY_LSHIFT
    || ev->keyboard.keycode == ALLEGRO_KEY_RSHIFT)
    {
      default_keys.run = TRUE;
    }
  }

  if (ev->type == ALLEGRO_EVENT_KEY_UP)
  {
    if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT)
    {
      default_keys.left = FALSE;
    }

    if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT)
    {
      default_keys.right = FALSE;
    }

    if (ev->keyboard.keycode == ALLEGRO_KEY_UP)
    {
      default_keys.jump = FALSE;
    }

    if (ev->keyboard.keycode == ALLEGRO_KEY_LSHIFT
    || ev->keyboard.keycode == ALLEGRO_KEY_RSHIFT)
    {
      default_keys.run = FALSE;
    }
  }

  if (creepy)
  {
    default_keys.run = FALSE;
  }
}

/*******************************************************************************
  state_update()
  Runs every step/frame - for game logic
*******************************************************************************/
static void
state_update(void)
{
  int i;

  vtile_count = 0;

  for (i=0; i<tile_count; ++i)
  {
    if (tile_list[i].x < (view_x + (GAME_W + tile_list[i].w))
     && tile_list[i].x > (view_x - (tile_list[i].w * 2)))
    {
      vtiles[vtile_count++] = &tile_list[i];
    }
  }

  if ((default_keys.left || default_keys.right) && !creepy)
  {
    if (rush)
    {
      ++crack_level;
    }
    else
    {
      ++step_count;

      // Increment crack level each 75 steps
      if ((step_count % 75) < 1)
      {
        ++crack_level;

        // STATE_SCARE may appear quicker than normal...
        if (view_x > 1000 && rand() % 20 == 1)
        {
          rush = TRUE;
        }
      }
    }

    if (crack_level >= 13)
    {
      if (game.music != NULL)
      {
        al_set_audio_stream_playing(game.music, 0);
      }

      push_state(STATE_SCARE, NULL);
      crack_level = 14;
      default_keys.left = 0;
      default_keys.right = 0;
      default_keys.run = 0;
      default_keys.jump = 0;
      go_down = TRUE;
      creepy = TRUE;
    }
  }

  float x, y;
  player_update(player);
  player_get_pos(player, &x, &y);

  // Update camera
  while (view_x > (max_width - GAME_W) || x < view_x + 270)
  {
    --view_x;
  }

  while (view_x < 0 || x > view_x + 320)
  {
    ++view_x;
  }

  if (go_down && view_x > 5555)
  {
    while (y > view_y + 222)
    {
      ++view_y;
    }
  }

  if (view_y > 3333)
  {
    // Fade out slowly
    alpha += 0.01;

    if (alpha >= 1.0)
    {
      change_state(STATE_DEAD, NULL);
    }
  }
}

/*******************************************************************************
  state_draw()
  For drawing to the screen - huds, sprites, backgrounds, etc.
*******************************************************************************/
static void
state_draw(void)
{
  int i, j;

  if (!creepy)
  {
    int w = al_get_bitmap_width(game.bg);
    int h = al_get_bitmap_height(game.bg);

    for (j=0; j<GAME_H; j+=w)
    {
      for (i=0; i<max_width; i+=h)
      {
        al_draw_bitmap(game.bg, i - view_x * 0.4, j, 0);
      }
    }
  }

  for (i=0; i<vtile_count; ++i)
  {
    al_draw_bitmap_region(creepy ? game.tilesred : game.tiles,
      vtiles[i]->left,
      vtiles[i]->top,
      vtiles[i]->w,
      vtiles[i]->h,
      vtiles[i]->x - view_x,
      vtiles[i]->y - view_y,
    0);

    if (creepy)
    {
      al_draw_bitmap_region(game.cracks,
        416, 0, 32, 32,
        vtiles[i]->x - view_x,
        vtiles[i]->y - view_y,
      0);
    }
    else
    {
      al_draw_bitmap_region(game.cracks,
        crack_level * 32, 0, 32, 32,
        vtiles[i]->x - view_x,
        vtiles[i]->y - view_y,
      0);
    }
  }

  al_draw_bitmap(game.text, 257 - view_x, 289, 0);

  player_draw(player);

  al_draw_filled_rectangle(0, 0, GAME_W, GAME_H,
    al_map_rgba_f(0, 0, 0, alpha));
}

/*******************************************************************************
  Definition of the state function
*******************************************************************************/
struct State*
State_Game(void)
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
