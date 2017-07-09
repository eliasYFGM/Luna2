#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED

#include "state.h"

struct State* State_Game(void);

#define STATE_GAME  State_Game()

struct Tile
{
  // Position in the tileset
  int left, top;

  // With and height
  int w, h;

  // Position in-game
  float x, y;
};

// Tiles that are on-screen (not full list)
extern struct Tile *vtiles[];

// How many of them?
extern int vtile_count;

// Camera vars
extern float view_x;
extern float view_y;

#endif // GAMESTATE_H_INCLUDED
