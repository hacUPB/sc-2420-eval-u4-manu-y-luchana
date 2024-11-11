#include "Bola.h"
#include "Paleta.h"
#include "constantes.h"
#include <stdio.h>
#include "audio.h"
Bola* crear_bola(const int x, int y, int height, int width, int dx, int dy)
{
	Bola* bola = (Bola*)malloc(sizeof(Bola));
	if (!bola) return NULL;
	bola->base.x = x;
	bola->base.y = y;
	bola->base.height = height;
	bola->base.width = width;
	bola->dx = dx;
	bola->dy = dy;
	bola->base.color = (SDL_Color){ 255, 255, 255, 255 }; // Blanco
	return bola;
}
void update_bola(Bola* bola, bool* game_is_running) {
	bola->base.x += bola->dx;
	bola->base.y += bola->dy;

	if (bola->base.x <= 0 || bola->base.x >= WINDOW_WIDTH - bola->base.width) {
		bola->dx *= -1; // Cambia dirección en el eje X
	}
	if (bola->base.y <= 0) {
		bola->dy *= -1; // Cambia dirección en el eje Y
	}
	if (bola->base.y >= WINDOW_HEIGHT - bola->base.height) {
		printf("¡La bola cayo! Fin del juego.\n");
		*game_is_running = false; // Termina el juego si la bola cae
	}
}
void verificar_colision_bola_paleta(Bola* bola, Paleta* paleta, Audio* tapAudio) {
	if (bola->base.x + bola->base.width >= paleta->base.x &&
		bola->base.x <= paleta->base.x + paleta->base.width &&
		bola->base.y + bola->base.height >= paleta->base.y &&
		bola->base.y <= paleta->base.y + paleta->base.height) {
		bola->dy *= -1; // Invertir dirección
		Audio_startPlayback(tapAudio);

		// printf("Colisión con la paleta\n");//debug
	}
}


void render_bola(Bola* bola, SDL_Renderer* renderer) {
	render_entity((Entity*)bola, renderer); // Llama a la función de renderizado de la entidad
}
void destruir_bola(Bola* this)
{
	if (this)
	{
		free(this); // No quiero liberar ningun dato porque solo tendre una bola
	}
}