#include "Paleta.h"
#include "constantes.h"
#include <stdbool.h>


Paleta* crear_paleta(const int x, int y, int height, int width, int velocidad)
{
	Paleta* paleta = (Paleta*)malloc(sizeof(Paleta));
	if (!paleta) return NULL;
	paleta->base.x = x;
	paleta->base.y = y;
	paleta->base.width = width;
	paleta->base.height = height;
	paleta->velocidad = velocidad;
	paleta->base.color = (SDL_Color){ 0, 255, 0, 255 }; // Verde
	
	return paleta;
}
void mover_paleta(Paleta* paleta, int direccion)
{
	if (direccion == SDLK_LEFT && paleta->base.x > 0) {
		paleta->base.x -= paleta->velocidad;
	}
	if (direccion == SDLK_RIGHT && paleta->base.x < WINDOW_WIDTH - paleta->base.width) {
		paleta->base.x += paleta->velocidad;
	}
}
void render_paleta(Paleta* paleta, SDL_Renderer* renderer) {
	render_entity((Entity*)paleta, renderer); // Llama a la función de renderizado de la entidad
}


void destruir_paleta(Paleta* this)
{
	if (this)
	{
		free(this); // No quiero liberar ningun dato porque solo tendre una bola
	}
}