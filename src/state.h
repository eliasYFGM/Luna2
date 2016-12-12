#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

// Main state structure
struct State
{
  void (*_end)(int exiting);

  void (*_pause)(void);
  void (*_resume)(void);

  void (*_events)(ALLEGRO_EVENT *ev);
  void (*_update)(void);
  void (*_draw)(void);
};

#endif // STATE_H_INCLUDED
