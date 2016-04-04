#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include "deadstate.h"
#include "game.h"

static struct // Data
{
    ALLEGRO_BITMAP* dead;
    ALLEGRO_AUDIO_STREAM* music;
}
data;

static float alpha = 0;
static int step_count = 0;

static void on_init(void* param)
{
    data.dead = al_load_bitmap("data/dead.tga");
    bg_color = C_BLACK;
}

static void on_end()
{
    al_destroy_bitmap(data.dead);
}

static void on_pause()
{
}

static void on_resume()
{
}

static void on_events(ALLEGRO_EVENT* event)
{
}

static void on_update()
{
    if (alpha < 1.0)
    {
        alpha += 0.05;
    }

    ++step_count;

    if (step_count > 300)
    {
        game_over();
    }
}

static void on_draw()
{
    al_draw_tinted_bitmap(data.dead,
        al_map_rgba_f(1.0 * alpha, 1.0 * alpha, 1.0 * alpha, alpha),
        0, 0, 0);
}

struct State* get_dead_state()
{
    static struct State state =
    {
        on_init,
        on_end,
        on_pause,
        on_resume,
        on_events,
        on_update,
        on_draw
    };

    return &state;
}
