#ifndef RANKING_H
#define RANKING_H

#define MAX_RANKING 10

typedef struct {
    float distanciaTotal;
} RankingEntry;

void ranking_init();
void ranking_add(float distanciaTotal);
void ranking_save();
void ranking_load();
void ranking_draw(int windowWidth, int windowHeight);
int ranking_getCount();


#endif
