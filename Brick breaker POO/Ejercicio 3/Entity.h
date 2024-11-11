#ifndef ENTITY_H
#define ENTITY_H
#include <SDL.h>

typedef struct Entity
{
    int x;
    int y;
    int height; 
    int width;
   SDL_Color color;     // Color para renderizar
    void (*render)(struct Entity* entity, SDL_Renderer* renderer); // Puntero a función render
} Entity;

void render_entity(Entity* entity, SDL_Renderer* renderer);
#endif