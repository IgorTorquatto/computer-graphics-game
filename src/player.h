#ifndef PLAYER_H
#define PLAYER_H

typedef enum { P_RUNNING, P_JUMPING, P_SLIDING } PlayerState;

typedef struct {
    int lane;
    float x, y, z;
    float vy;
    PlayerState state;
    float width, height, depth;
} Player;

void initPlayer(Player *p);
void updatePlayer(Player *p, float dt);
static void endSlide(void *data);
void handlePlayerInput(Player *p, unsigned char key);
void handlePlayerSpecial(Player *p, int key);
void drawPlayer(const Player *p);

#endif

