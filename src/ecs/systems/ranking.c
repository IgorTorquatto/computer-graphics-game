#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#include "utils/print.h"
#include "ranking.h"

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
    if (distanciaTotal <= 0.0f) return;
    if (rankingCount < MAX_RANKING) {
        rankingList[rankingCount].distanciaTotal = distanciaTotal;
        rankingCount++;
    } else if (distanciaTotal > rankingList[rankingCount - 1].distanciaTotal) {
        rankingList[rankingCount - 1].distanciaTotal = distanciaTotal;
    } else {
        return; // n�o entrou no ranking
    }
    qsort(rankingList, rankingCount, sizeof(RankingEntry), compareDesc);
}

void ranking_save() {
    FILE *file = fopen("ranking.dat", "wb");
    if (!file) {
        perror("Erro ao abrir ranking.dat para salvar");
        return;
    }
    if (fwrite(&rankingCount, sizeof(int), 1, file) != 1) {
        perror("Erro ao salvar rankingCount");
    }
    if (fwrite(rankingList, sizeof(RankingEntry), rankingCount, file) != (size_t)rankingCount) {
        perror("Erro ao salvar rankingList");
    }
    fclose(file);
    print_success("Ranking salvo com %d entradas.", rankingCount);
}



void ranking_load() {
    print_info("ranking_load called.");
    // Limpar lista antes de tentar carregar
    ranking_init();

    FILE *file = fopen("ranking.dat", "rb");
    if(!file) {
        // Arquivo não existe, lista mantém zerada
        return;
    }

    if (fread(&rankingCount, sizeof(int), 1, file) != 1) {
        fclose(file);
        ranking_init();
        return;
    }

    if (rankingCount > MAX_RANKING || rankingCount < 0) {
        fclose(file);
        ranking_init();
        return;
    }

    if (fread(rankingList, sizeof(RankingEntry), rankingCount, file) != (size_t)rankingCount) {
        fclose(file);
        ranking_init();
        return;
    }

    fclose(file);
}

void ranking_draw(int windowWidth, int windowHeight) {
    //print_info("ranking_draw called. rankingCount = %d\n", rankingCount); debug

    int yStart = windowHeight / 2 + 80;
    char buffer[128];

    if(rankingCount == 0) {
        // Caso sem entradas no ranking
        const char *msg = "Jogue para aparecer no ranking";
        int x = (windowWidth - getTextWidth(msg, GLUT_BITMAP_HELVETICA_18)) / 2;
        drawText(msg, x, yStart, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f, windowHeight);
    } else {
        for(int i=0; i<rankingCount; i++) {
            snprintf(buffer, sizeof(buffer), "%d. score: %.1f metros", i+1, rankingList[i].distanciaTotal);
            int x = (windowWidth - getTextWidth(buffer, GLUT_BITMAP_HELVETICA_18)) / 2;
            drawText(buffer, x, yStart - i*30, GLUT_BITMAP_HELVETICA_18,
                     1.0f, 1.0f, 1.0f, windowHeight);
        }
    }
}

int ranking_getCount() {
    return rankingCount;
}
