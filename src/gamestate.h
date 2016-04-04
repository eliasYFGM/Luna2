#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED

#include "state.h"

struct State* get_game_state();

#define GAME_STATE  get_game_state()

struct Tile
{
    int left;
    int top;
    int w, h;
    float x, y;
};

// Array holding the tiles on-screen
extern struct Tile* vtiles[];

// How many tiles?
extern int vtile_count;

struct Keys
{
    int left;
    int right;
    int run;
    int jump;
};

// Key status
extern struct Keys keys;

// Camera vars
extern float view_x;
extern float view_y;

#endif // GAMESTATE_H_INCLUDED
