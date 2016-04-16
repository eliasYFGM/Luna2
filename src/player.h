#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

struct Player
{
    float x, y;
    float yspeed;
    int dir;

    struct // Sprite data
    {
        ALLEGRO_BITMAP* stand;
        ALLEGRO_BITMAP* walk;
        ALLEGRO_BITMAP* flying;
        int frame;
    }
    sprite;
};

struct Player* create_player(float x, float y);
void destroy_player(struct Player*);

void player_update(struct Player*);
void player_draw(struct Player*);

extern int go_down;

#endif // PLAYER_H_INCLUDED
