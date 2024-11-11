#include "Powerup.h"
#include "Ladrillo.h"
#include "constantes.h"
#include <stdlib.h>
#include <SDL.h>

// Crear un cuadrito de power-up
PowerUp* crear_powerup_cuadrito(int x, int y, int width, int height) {
    PowerUp* cuadrito = (PowerUp*)malloc(sizeof(PowerUp));
    if (cuadrito != NULL) {
        cuadrito->base.x = x;
        cuadrito->base.y = y;
        cuadrito->base.width = width;
        cuadrito->base.height = height;
        cuadrito->activo = false;
        cuadrito->base.color = (SDL_Color){ 0, 0, 255, 255 }; // Blanco
    }
    return cuadrito;
}
PowerUp cuadritos_powerup[5];  // Definición de la variable 
// Generar la lluvia de cuadritos
void generar_lluvia_powerup() {
    for (int i = 0; i < 5; i++) {  // Cambiado a 5 cuadritos
        cuadritos_powerup[i].base.x = rand() % 800;  // Posición aleatoria en el eje X
        cuadritos_powerup[i].base.y = 0;  // Empiezan en la parte superior
        cuadritos_powerup[i].base.width = 10;
        cuadritos_powerup[i].base.height = 10;
        cuadritos_powerup[i].activo = true;
    }
}

void actualizar_lluvia_powerup(Bola* bola, int* score) {
    for (int i = 0; i < 5; i++) {  // Cambiado a 5 cuadritos
        if (cuadritos_powerup[i].activo) {
            cuadritos_powerup[i].base.y += 1;  // Velocidad de caída lenta

            // Colisión con la bola
            if (bola->base.x < cuadritos_powerup[i].base.x + cuadritos_powerup[i].base.width &&
                bola->base.x + bola->base.width > cuadritos_powerup[i].base.x &&
                bola->base.y < cuadritos_powerup[i].base.y + cuadritos_powerup[i].base.height &&
                bola->base.y + bola->base.height > cuadritos_powerup[i].base.y) {

                // Aquí solo desactivamos el cuadrito en lugar de liberar memoria
                cuadritos_powerup[i].activo = false;  // Desactivar cuadrito
                *score += 3;  // Sumar 3 puntos al score total
            }

            // Si el cuadrito sale de la pantalla, desactivarlo
            if (cuadritos_powerup[i].base.y > 600) {  // Suponiendo una pantalla de altura 600px
                cuadritos_powerup[i].activo = false;  // Solo desactivar
            }
        }
    }
}

void verificar_colision_powerup_bola(PowerUp* powerup, Bola* bola) {
    if (!powerup->activo) return;  // Si no está activo, no hacer nada

    SDL_Rect rect_powerup = { powerup->base.x, powerup->base.y, powerup->base.width, powerup->base.height };
    SDL_Rect rect_bola = { bola->base.x, bola->base.y, bola->base.width, bola->base.height };

    if (SDL_HasIntersection(&rect_powerup, &rect_bola)) {
        powerup->activo = 0;   // Desactivar el power-up tras la colisión
        score += 3;  // Sumar 3 puntos al puntaje global
    }
}

void render_powerup(PowerUp* cuadritos_powerup, SDL_Renderer* renderer) {
    for (int i = 0; i < 5; i++) {  // Cambiado a 5 cuadritos
        if (cuadritos_powerup[i].activo) {
            SDL_Rect rect = { cuadritos_powerup[i].base.x, cuadritos_powerup[i].base.y,
                              cuadritos_powerup[i].base.width, cuadritos_powerup[i].base.height };
            render_entity((Entity*)cuadritos_powerup, renderer); // Llama a la función de renderizado de la entidad
        }
    }
}
