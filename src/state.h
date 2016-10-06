#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include <allegro5/allegro.h>

// Main state structure
struct State
{
  void (*_init)(void*);
  void (*_end)(int);

  void (*_pause)();
  void (*_resume)();

  void (*_events)(ALLEGRO_EVENT*);
  void (*_update)();
  void (*_draw)();
};

#endif // STATE_H_INCLUDED
