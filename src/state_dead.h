#ifndef DEADSTATE_H_INCLUDED
#define DEADSTATE_H_INCLUDED

#include "state.h"

struct State* State_Dead(void);

#define STATE_DEAD  State_Dead()

#endif // DEADSTATE_H_INCLUDED
