# JUEGO FINAL: BRICK BREAKER 
Para el desarrollo de esta nueva parte del proyecto y luego de haber realizado toda la parte de la investigación de la unidad 4, logramos añadir a nuestro juego BRICK BREAKER (originalmente de luciana) dos sonidos que funcionan por medio de un solo hilo. Conservamos el sonido "tap.wav" del ejercicio #8 y buscamos uno nuevo llamado "destruir.wav" que simula el sonido de un ladrillo partiendose. 

**Todo el código del proyecto  está completo en las carpetas de este repositorio**

## Parte 1: crear los archivos .c y .h 
Para continuar con lo visto la unidad anterior y con la idea de seguir aplicando POO a nuestro juego, creamos dos archivos más: audio.c y audio.h . 

En el archivo **audio.h** definimos las variables y funciones necesarias para manejar la reproducción de audio en una aplicación usando la biblioteca SDL: 
```c
#ifndef AUDIO_H
#define AUDIO_H

#include <SDL.h>

typedef struct {
    Uint8* audioData;       // Puntero a los datos de audio
    Uint32 audioLength;     // Longitud de los datos de audio en bytes
    Uint32 audioPosition;   // Posicion actual en los datos de audio
    SDL_bool audioFinished; // Bandera para verificar si el audio ha terminado
    SDL_AudioDeviceID audioDevice;
    SDL_AudioSpec audioSpec;
    SDL_sem* audioSemaphore;   // Puntero del Semaforo global para sincronizar el evento de audio
    const char* fileName;    // Nombre del archivo de audio
} Audio;

// Constantes para los archivos de audio
extern const char* TAP_AUDIO_FILE;
extern const char* DESTRUIR_AUDIO_FILE;

void Audio_init(Audio* audio, const char* fileName);
void Audio_destroy(Audio* audio);
void Audio_startPlayback(Audio* audio);

#endif // AUDIO_H
```
Al momento de la planeación del ejercicio, decidimos que el evento en este caso que estaría vinculado al hilo del audio serían las colisiones, es decir, cuando el cuadrito del juego colisiona con la paleta movil o con los ladrillos se reproducirá un sonido. 

Aunque el evento es el mismo, el sonido  cambia según contra qué objeto colisiona el cuadrito. Si el cuadrito golpea la paleta se reproduce "tap.wav" y si colisiona contra un ladrillo reproduce "destruir.wav". Basandonos en la definición de polimorfismo pero sin aplicarlo directamente, decidimos experimenntar si era posible que el hilo lograra reproducir el sonido según el tipo de colisión que se presentara y lo desarrollamos de la siguiente manera : 

En la estructura del audio **(typedef struct Audio)** definimos nuevas variables como **fileName**, la cual es una cadena que representa el nombre del archivo de audio. Es esencial para controlar que tipo de audio debe reproducirse en cada caso. En el archivo audio.c podemos ver más a detalle la importancia de esta nueva variable. 

Tambien creamos constantes externas que representan los nombres de los archivos de audio, en este caso son **TAP_AUDIO_FILE y DESTRUIR_AUDIO_FILE**. 

Por último creamos la función **Audio_init** que inicializa una estructura de audio con los datos del archivo que se especifica con **fileName**, la función **Audio_destroy** libera los recursos usados y la función **Audio_starPlayback** se encarga de iniciar la reproducción del audio. 

El archivo **audio.c** implementa la funcionalidad definida en el archivo .h, inicializando las funciones necesarias para reproducir y parar el audio y liberar los recursos utilizados. 
```c
#include "audio.h"
#include <stdio.h>
#include <stdbool.h>

const char* TAP_AUDIO_FILE = "tap.wav";
const char* DESTRUIR_AUDIO_FILE = "destruir.wav";

void AudioCallback(void* userdata, Uint8* stream, int len);

int audio_thread(void* arg) {
    Audio* audio = (Audio*)arg;
    const char* audioFilePath = audio->fileName;  // Ruta del archivo de audio

    while (true) {
        SDL_SemWait(audio->audioSemaphore);  // Espera hasta que se active el semaforo

        // Reinicia la posicion del audio y la bandera de terminado
        audio->audioPosition = 0;
        audio->audioFinished = SDL_FALSE;

        // Carga el archivo de audio
        
        if (SDL_LoadWAV(audioFilePath, &audio->audioSpec, &audio->audioData, &audio->audioLength) == NULL) {
            printf("Unable to load WAV file: %s\n", SDL_GetError());
            continue;
        }

        // Inicia la reproduccion de audio
        SDL_PauseAudioDevice(audio->audioDevice, 0);
        printf("Audio playback started\n");

        // Espera a que termine la reproduccion de audio
        while (!audio->audioFinished) {
            SDL_Delay(10); // mini espera para no sobrecargar el CPU
        }

        // Detener el dispositivo de audio
        SDL_PauseAudioDevice(audio->audioDevice, 1);

        // Libera el archivo de audio despues de la reproduccion
        SDL_FreeWAV(audio->audioData);
        printf("Audio finished\n");
    }
    return 0;
}


void AudioCallback(void* userdata, Uint8* stream, int len) {
    Audio* audio = (Audio*)userdata;

    if (audio->audioPosition >= audio->audioLength) {
        audio->audioFinished = SDL_TRUE;  // Marca como terminado cuando se alcanza el final
        return;
    }

    int remainingBytes = audio->audioLength - audio->audioPosition;
    int bytesToCopy = (len < remainingBytes) ? len : remainingBytes;

    SDL_memcpy(stream, audio->audioData + audio->audioPosition, bytesToCopy);
    audio->audioPosition += bytesToCopy;
}


void Audio_init(Audio* audio, const char* fileName) {
    audio->fileName = fileName;  // Asignar el archivo de sonido correspondiente

    // Configuracion del formato de audio
    audio->audioSpec.freq = 44100;
    audio->audioSpec.format = AUDIO_S16SYS;
    audio->audioSpec.channels = 1;
    audio->audioSpec.samples = 2048;
    audio->audioSpec.callback = AudioCallback;
    audio->audioSpec.userdata = audio;

    // Abre el dispositivo de audio
    audio->audioDevice = SDL_OpenAudioDevice(NULL, 0, &audio->audioSpec, NULL, 0);
    if (audio->audioDevice == 0) {
        fprintf(stderr, "Unable to open audio device: %s\n", SDL_GetError());
        return false;
    }

    // Inicializa el semaforo para el hilo de audio
    audio->audioSemaphore = SDL_CreateSemaphore(0);

    // Crea el hilo de audio
    SDL_Thread* thread = SDL_CreateThread(audio_thread, "AudioThread", audio);
    if (thread == NULL) {
        printf("Unable to create audio thread: %s\n", SDL_GetError());
    }
    else {
        SDL_DetachThread(thread);
    }
}

void Audio_destroy(Audio* audio) {
    SDL_CloseAudioDevice(audio->audioDevice);
    SDL_DestroySemaphore(audio->audioSemaphore);
}

void Audio_startPlayback(Audio* audio) {
    SDL_SemPost(audio->audioSemaphore);
}
```
Como te comentabamos anteriormente, en este archivo asignamos los nombres de cada sonido y los llamamos por medio de **audioFilePath**: 

**Asignación de nombres**:

![image](https://github.com/user-attachments/assets/af6a71e0-abea-4a0d-b641-0fb180ac239b)

**Utilización de los nombres por medio de audioFilePath**:

![image](https://github.com/user-attachments/assets/a3674233-21a6-495e-90ff-2342681e8568)

Algo que debemos destacar de este archivo es la función **audio_thread**, la cual es un hilo independiente que se crea para la reproducción del sonido. Tambien puedes ver que usamos la tecnica  del semáforo para evitar que los audios  suenen cuando no deben hacerlo. Aquí  se intenta cargar el archivo .WAV usando SDL_LoadWAV y en caso de que no se pueda acceder al archivo en la carpeta del proyecto hay un debug que imprime el error y continúa el código sin reproducir el sonido. 

En **Audio_init** configura todos las especificaciones del audio (frecuencia, formato, etc), crea el semáforo y el hilo de audio, este último se encarga de esperar en el semáforo hasta que este se active y se pueda reproducir el audio. 

Ahora **Audio_destroy** libera el dispositivo de audio por medio de **SDL_CloseAudioDevice** y luego se destruye el semáforo **SDL_DestroySemaphore** para liberar los recursos al finalizar. 

## Parte 2: Las colisiones y sus respectivos sonidos
Para esta parte nosotras decidimos no llamar la reproducción del sonido desde el main.c sino desde los archivos .c de la bola (cuadrito) y los ladrillos. 

Primero incluimos el ![image](https://github.com/user-attachments/assets/e05686d2-c985-4126-b034-c050e54d5485) en ambos archivos para que reconociera todos sus elementos. y luego en la función que se encarga de detectar la colision añadimos la funcionalidad del audio de la siguiente manera: 

En **Bola.c**
```c
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
```

Dentro de sus parametros incluimos al puntero **Audio tapAudio** para reproducir el sonido de la colision cuando ocurre. Luego de que ese **if** verifica la colision entre el cuadrito y la paleta y su respuesta es el cambio de dirección del cuadrito y la **reproducción del sonido** por medio de **Audio_startPlayback(tapAudio)**. Lo que está dentro de este parentesis indica cual de los dos audios debe reproducirse. 

En **Ladrillo.c** funciona de la misma manera pero esta vez el puntero es **Audio destruirAudio** y luego de verificar la colision se reproduce el sonido por medio de **Audio_startPlayback(destruirAudio)**: 
```c
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
```
Gracias al uso de los debugs podemos mirar desde la consola el comportamiento de los archivos de audio y luego de correr el código exitosamente la consola se ve así: 
![brick](https://github.com/user-attachments/assets/d243e4bf-cbd2-40f4-bb89-a953d7914168)

