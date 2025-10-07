#pragma once

typedef enum {
    MODO_MENU,
    MODO_JOGO,
    MODO_PAUSE,
    MODO_GAME_OVER,
    MODO_RANKING
} Modo;

extern Modo modoAtual;
