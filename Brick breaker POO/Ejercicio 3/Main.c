#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include "bola.h"
#include "paleta.h"
#include "ladrillo.h"
#include "powerup.h"
#include "audio.h" 
#include "constantes.h"



// Función principal del juego
int main(Audio *audio) {
    // Inicialización de SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Window* window = SDL_CreateWindow("Brick Breaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Audio_init(&audio);
    // Crear la bola, la paleta y los ladrillos
    Bola* bola = crear_bola(400, 300, 20, 20, 3, -3);
    Paleta* paleta = crear_paleta(350, 500, 20, 100, 15);
    Ladrillo* ladrillos[5][10];
    PowerUp* powerup = crear_powerup_cuadrito(rand() % WINDOW_WIDTH, 0, 20, 20, 2);

    generar_lluvia_powerup();

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            ladrillos[i][j] = crear_ladrillo(j * 80, i * 40, 40, 80);
        }
    }

    bool game_is_running = true;
    while (game_is_running) {
        // Procesar eventos
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                game_is_running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                mover_paleta(paleta, event.key.keysym.sym);
            }
        }

        // Actualizar la bola (añade la colisión con la paleta)
        update_bola(bola, &game_is_running, paleta);
        verificar_colision_bola_paleta(bola, paleta, &audio); // Pasa el audio a la función de colisión
        check_bola_brick_collision(bola, ladrillos, &audio);  // Pasa el audio a la función de colisión de ladrillos
        actualizar_lluvia_powerup(&bola, &score);

        // Limpiar pantalla
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render_powerup(cuadritos_powerup, renderer);

        // Renderizar la bola, la paleta y los ladrillos
        render_entity(bola, renderer);
        render_paleta(paleta, renderer);
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 10; j++) {
                render_ladrillo(ladrillos[i][j], renderer); // Renderiza todos los ladrillos
            }
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 10; j++) {
                if (ladrillos[i][j]->destruido) {
                    render_ladrillo(ladrillos[i][j], renderer);
                }
            }
        }

        // Actualizar pantalla
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Aproximadamente 60 FPS
    }

    // Limpiar recursos
    Audio_destroy(&audio); // Libera los recursos de audio
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
