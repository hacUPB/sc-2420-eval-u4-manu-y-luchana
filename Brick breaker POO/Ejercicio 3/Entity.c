#include "Entity.h"


//Esta es la base para los metodos, en este caso unicamente render
void render_entity(Entity* entity, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, entity->color.r, entity->color.g, entity->color.b, 255);
    SDL_Rect rect = { entity->x, entity->y, entity->width, entity->height };
    SDL_RenderFillRect(renderer, &rect);
}
