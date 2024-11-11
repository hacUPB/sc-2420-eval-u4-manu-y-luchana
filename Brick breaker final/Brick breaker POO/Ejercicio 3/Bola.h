#ifndef BOLA_H
#define BOLA_H

#include "Entity.h"
#include"Paleta.h"
#include <stdbool.h>

typedef struct {
    Entity base;
    int dx, dy;
    
} Bola;

Bola* crear_bola(int x, int y, int height, int width, int dx, int dy);
void destruir_bola(Bola* bola);
void update_bola(Bola* bola, bool* game_is_running);
extern bool verificar_colision_bola_paleta(Bola* bola, Paleta* paleta);
void render_bola(Bola* bola, SDL_Renderer* renderer);

#endif