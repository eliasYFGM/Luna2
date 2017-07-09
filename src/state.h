#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

struct State_Machine;

// Main state structure
struct State
{
  void (*_load)(void *param);
  void (*_free)(void);

  void (*_enter)(void *param);
  void (*_exit)(void);

  void (*_pause)(void);
  void (*_resume)(void);

  void (*_events)(ALLEGRO_EVENT *ev, struct State_Machine *sm);
  void (*_update)(struct State_Machine *sm);
  void (*_draw)(struct State_Machine *sm);

  // Private and used only by engine.c
  // Indicates if the state was already initialized.
  int loaded;
};

#endif // STATE_H_INCLUDED
