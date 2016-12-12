#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

struct Player;

struct Keys
{
  int left;
  int right;
  int run;
  int jump;
};

struct Player* create_player(float x, float y, struct Keys *keys);
void destroy_player(struct Player *p);
void player_update(struct Player *p);
void player_draw(struct Player *p);
void player_get_pos(struct Player *p, float *x, float *y);

extern int go_down;

#endif // PLAYER_H_INCLUDED
