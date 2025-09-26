#include "ranking.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

extern int getTextWidth(const char *text, void *font);
extern void drawText(const char *text, int x, int y, void *font, float r, float g, float b, int windowHeight);

static RankingEntry rankingList[MAX_RANKING];
static int rankingCount = 0;

void ranking_init() {
    rankingCount = 0;
}

int compareDesc(const void *a, const void *b) {
    float diff = ((RankingEntry *)b)->distanciaTotal - ((RankingEntry *)a)->distanciaTotal;
    return (diff > 0) - (diff < 0);
}

void ranking_add(float distanciaTotal) {
    if (rankingCount < MAX_RANKING) {
        rankingList[rankingCount].distanciaTotal = distanciaTotal;
        rankingCount++;
    } else if (distanciaTotal > rankingList[rankingCount - 1].distanciaTotal) {
        rankingList[rankingCount - 1].distanciaTotal = distanciaTotal;
    } else {
        return; // não entrou no ranking
    }
    qsort(rankingList, rankingCount, sizeof(RankingEntry), compareDesc);
}

void ranking_save() {
    FILE *file = fopen("ranking.dat", "wb");
    if(!file) return;
    fwrite(&rankingCount, sizeof(int), 1, file);
    fwrite(rankingList, sizeof(RankingEntry), rankingCount, file);
    fclose(file);
}

void ranking_load() {
    FILE *file = fopen("ranking.dat", "rb");
    if(!file) {
        ranking_init();
        return;
    }
    fread(&rankingCount, sizeof(int), 1, file);
    fread(rankingList, sizeof(RankingEntry), rankingCount, file);
    fclose(file);
}

void ranking_draw(int windowWidth, int windowHeight) {
    int yStart = windowHeight / 2 + 80;
    char buffer[128];
    for(int i=0; i<rankingCount; i++) {
        snprintf(buffer, sizeof(buffer), "%d. Distancia: %.1f metros", i+1, rankingList[i].distanciaTotal);
        int x = (windowWidth - getTextWidth(buffer, GLUT_BITMAP_HELVETICA_18)) / 2;
        drawText(buffer, x, yStart - i*30, GLUT_BITMAP_HELVETICA_18,
                 1.0f, 1.0f, 1.0f, windowHeight);
    }
}
