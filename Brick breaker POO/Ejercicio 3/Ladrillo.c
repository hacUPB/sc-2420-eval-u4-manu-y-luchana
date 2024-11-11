#include "Ladrillo.h"
#include "Bola.h"
#include "audio.h"


Ladrillo* crear_ladrillo(int x, int y, int height, int width) {
    Ladrillo* ladrillo = (Ladrillo*)malloc(sizeof(Ladrillo));
    if (!ladrillo) return NULL;
    ladrillo->base.x = x;
    ladrillo->base.y = y;
    ladrillo->base.width = width;
    ladrillo->base.height = height;
    ladrillo->destruido = false; // Asegúrate de inicializarlo a false
    ladrillo->base.color = (SDL_Color){ 255, 0, 0, 255 }; // Color rojo
    return ladrillo;
}

int score = 0;
bool activar_lluvia_powerup = false;

// Función para verificar la colisión entre la bola y los ladrillos
void check_bola_brick_collision(Bola* bola, Ladrillo* ladrillos[5][10], Audio* destruirAudio) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            Ladrillo* ladrillo = ladrillos[i][j]; // Cambia a puntero directo
            if (!ladrillo->destruido) {
                // Verificación de colisión entre la bola y el ladrillo
                if (bola->base.x < ladrillo->base.x + ladrillo->base.width &&
                    bola->base.x + bola->base.width > ladrillo->base.x &&
                    bola->base.y < ladrillo->base.y + ladrillo->base.height &&
                    bola->base.y + bola->base.height > ladrillo->base.y) {

                    // Si colisiona, destruimos el ladrillo y cambiamos la dirección de la bola
                    ladrillo->destruido = true;
                    bola->dy *= -1;  // Cambiamos la dirección vertical de la bola
                    score += 10;  // Incrementar el score por ladrillo destruido
                    if (score % 50 == 0 && score > 0) {  // Activar solo cuando el score sea un múltiplo de 40
                        activar_lluvia_powerup = true;
                    }
                    printf("Puntaje: %d\n", score); // Mostrar el puntaje
                    Audio_startPlayback(destruirAudio);

                }
            }
        }
    }
}


void render_ladrillo(Ladrillo* ladrillo, SDL_Renderer* renderer) {
    if (!ladrillo->destruido) {
        render_entity((Entity*)ladrillo, renderer); // Llama a la función de renderizado de la entidad
        // Guardamos el color original del renderer
        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

        // Establecemos el color blanco para el contorno
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanco

        // Obtenemos las dimensiones del ladrillo (asumiendo que es un rectángulo)
        SDL_Rect rect_contorno;
        rect_contorno.x = ladrillo->base.x;        // Posición x del ladrillo
        rect_contorno.y = ladrillo->base.y;        // Posición y del ladrillo
        rect_contorno.w = ladrillo->base.width;    // Ancho del ladrillo
        rect_contorno.h = ladrillo->base.height;     // Alto del ladrillo

        // Dibuja el contorno
        SDL_RenderDrawRect(renderer, &rect_contorno);

        // Restauramos el color original del renderer
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
    }
}

void destruir_ladrillo(Ladrillo* this) {
    free(this);
}



