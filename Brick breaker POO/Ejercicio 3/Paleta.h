#ifndef PALETA_H
#define PALETA_H
#include "Entity.h"


// Definimos la estructura de la bola
typedef struct Paleta {
    Entity base;
    int velocidad;
}Paleta;

Paleta* crear_paleta(const int x, int y, int width, int heigth, int velocidad);
void mover_paleta(Paleta* paleta,int dirección);
void destruir_paleta(Paleta* this);
void render_paleta(Paleta* paleta, SDL_Renderer* renderer);
#endif //BOLA_H
