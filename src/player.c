#include <allegro5/allegro.h>
#include "player.h"
#include "game.h"
#include "states/gamestate.h"
#include "states/deadstate.h"

static struct // Player position
{
    float x;
    float y;
    float yspeed;
    int dir;
}
pos =
{
    100, 100, 0, 1
};

static struct // Sprite data
{
    ALLEGRO_BITMAP* stand;
    ALLEGRO_BITMAP* walk;
    ALLEGRO_BITMAP* jump_fall;
    int frame;
}
spr;

int go_down = 0;

// Checks for a tile using current position and width/height
static struct Tile* check_tile(float x, float y)
{
    int i;
    struct Tile* t = NULL;

    for (i=0; i<vtile_count; ++i)
    {
        if (check_bb_collision((pos.x + 15) + x, (pos.y + 20) + y,
            22, 23, vtiles[i]->x, vtiles[i]->y, vtiles[i]->w, vtiles[i]->h))
        {
            t = vtiles[i];
        }
    }

    return t;
}

void player_init()
{
    spr.stand = al_load_bitmap("data/stand.tga");
    spr.walk = al_load_bitmap("data/trotting.tga");
    spr.jump_fall = al_load_bitmap("data/flying.tga");

    spr.frame = 0;
}

void player_end()
{
    al_destroy_bitmap(spr.stand);
    al_destroy_bitmap(spr.walk);
    al_destroy_bitmap(spr.jump_fall);
}

void player_update()
{
    // Moving...
    if (keys.left)
    {
        pos.dir = 0;

        if (pos.y < 480)
        {
            pos.x -= keys.run ? 6 : 3;
        }

        while (check_tile(0, 0))
        {
            ++pos.x;
        }
    }
    else if (keys.right)
    {
        pos.dir = 1;

        if (pos.y < 480)
        {
            pos.x += keys.run ? 6 : 3;
        }

        while (check_tile(0, 0))
        {
            --pos.x;
        }
    }

    // Only jump if Luna is standing on ground
    if (keys.jump && check_tile(0, 1))
    {
        pos.yspeed = -12;
    }

    // Update vertical speed
    pos.yspeed += 0.5;
    if (pos.yspeed > 12)
    {
        pos.yspeed = 12;
    }

    pos.y += pos.yspeed;

    // Stop Luna when overlapping with tiles
    struct Tile* t = check_tile(0, 0);
    if (t)
    {
        if (pos.yspeed > 0)
        {
            pos.y = t->y - 43;
        }
        else if (pos.yspeed < 0)
        {
            pos.y = t->y + t->h - 20;
        }

        pos.yspeed = 0;
    }

    // Update frame of animation
    if (keys.run)
    {
        spr.frame += 2;
    }
    else
    {
        ++spr.frame;
    }

    if (spr.frame >= 16)
    {
        spr.frame = 0;
    }

    // Update camera
    while (pos.x < view_x + 270)
	{
		--view_x;
	}

	while (pos.x > view_x + 320)
	{
		++view_x;
	}

	if (go_down && view_x > 5555)
	{
        while (pos.y > view_y + 222)
        {
            ++view_y;
        }
    }
}

void player_draw()
{
    // On ground
    if (check_tile(0, 1))
    {
        if (keys.left || keys.right)
        {
            al_draw_bitmap_region(spr.walk, spr.frame * 48, 0, 48, 47,
                pos.x - view_x, pos.y-4 - view_y,
                pos.dir == 1 ? 0 : ALLEGRO_FLIP_HORIZONTAL);
        }
        else
        {
            al_draw_bitmap(spr.stand, pos.x - view_x, pos.y - view_y,
            pos.dir == 1 ? 0 : ALLEGRO_FLIP_HORIZONTAL);
        }
    }
    else // Flying or falling
    {
        al_draw_bitmap_region(spr.jump_fall, spr.frame * 48, 0, 48, 56,
            pos.x - view_x, pos.y-4 - view_y,
            pos.dir == 1 ? 0 : ALLEGRO_FLIP_HORIZONTAL);
    }
}
