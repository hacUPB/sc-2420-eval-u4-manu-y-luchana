#ifndef LADRILLO_H
#define LADRILLO_H

#include "Entity.h" 
#include <SDL.h>
#include "Bola.h"
#include <stdbool.h>

typedef struct Ladrillo {
    Entity base;    
    bool destruido;
} Ladrillo;

extern bool activar_lluvia_powerup;
extern int score;// lo cree extern para que cualquier clase pudiera acceder a este dato
Ladrillo ladrillos[5][10];
Ladrillo* crear_ladrillo(int x, int y, int height, int width);  
void destruir_ladrillo(Ladrillo* this);
void render_ladrillo(Ladrillo* ladrillo, SDL_Renderer* renderer);
void check_bola_brick_collision(struct Bola* bola, struct Ladrillo* ladrillos[5][10]);
#endif
