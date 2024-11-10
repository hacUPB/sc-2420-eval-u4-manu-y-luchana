[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/XglgMq0o)
# Documentación del Proyecto
---
Estudiante 1:  Manuela Buriticá Bedoya
Id:  000505665
---
---
Estudiante 2: Luciana Gutierrez Posada
Id: 000507574
---



-
## Ejercicio 6: esperar un hilo.
Para esperar que un hilo en particular termine se debe utilizar la función **pthread_join**. Luego de analizar el código , cuando llaman a la función **pthread_join** permiten que el hilo principal espere a que los hilos threadID1 y threadID2 terminen su ejecución antes de que el programa termine. 

Cada llamada a esta función usa dos arguementos, el primero es el hilo en cuestión  (threadID1 y threadID2) hace que el hilo principal espere hasta que cada hilo haya terminado su ejecución. 

Las líneas encargadas de esperar que un hilo en particular termine son: 
```c
pthread_join(threadID1, NULL);
pthread_join(threadID2, NULL);
```

**Código 1**
```c
pthread_create(&threadID1,NULL,&imprime, &threadParam1);
pthread_join(threadID1,NULL);
pthread_create(&threadID2,NULL,&imprime, &threadParam2);
pthread_join(threadID2,NULL);
```
En este código los hilos no trabajan paralelamente, en vez de eso, threadID2 espera hasta que threadID1 termine.

**Código 2**
```c
pthread_create(&threadID1, NULL, &imprime, &threadParam1);
pthread_create(&threadID2, NULL, &imprime, &threadParam2);
pthread_join(threadID1, NULL);
pthread_join(threadID2, NULL);
```
A diferencia del código 1, ambos hilos trabajan en paralelo y se ejecutan simultaneamente. Los beneficios de este funcionamiento son que su ejecución es más rápida que la del primer código.

## Ejercicio 8: ejemplo de referencia para la evaluación.
Luego de ver el funcionamiento del programa, el usuario debe presionar **la letra P** para que este reproduzca el sonido.  Las funciones que permiten que el audio se reproduzca con éxito son **AudioCallback, play_audio y process_input**.

**Adjuntamos pruebas del archivo funcionando:**

![image](https://github.com/user-attachments/assets/4e37c257-5a33-4d22-8940-c01b8eafe4b4)

Lo que podemos observar cuando presionamos la letra P y se reproduce el sonido es que el funcionamiento del juego se detiene un momento, tira un mensaje por consola que dice "audio finished" y luego reactiva el juego. Esto lo que nos quiere decir es que el juego solo puede hacer una acción al tiempo y no dos de manera paralela. 

Lo primero que se nos ocurre para solucionar este problema es precisamente la implementación de hilos, los cuales se encargan de que un programa pueda"hacer varias cosas al mismo tiempo”, cada cosa con un hilo independiente.  

Como mencionamos anteriormente, creamos un hilo independiente para cargar y reproducir un archivo de audio. A continuación el código modificado para que realice varias funciones en paralelo, en este caso 2: 

**El hilo creado fue el siguiente:**
```c
int audio_thread(void* arg) {
    audioContext.audioPosition = 0;
    audioContext.audioFinished = SDL_FALSE;

    if (SDL_LoadWAV("tap.wav", &audioSpec, &audioContext.audioData, &audioContext.audioLength) == NULL) {
        printf("Unable to load WAV file: %s\n", SDL_GetError());
        return -1;
    }

    SDL_PauseAudioDevice(audioDevice, 0);

    while (!audioContext.audioFinished) {
        SDL_Delay(10);
    }

    SDL_FreeWAV(audioContext.audioData);
    return 0;
}
```
Este hilo configura la posición y el estado del audio, usa la función SDL_LoadWAV para cargar el archivo, inicia la reproducción con SDL_PauseAudioDevice, tambien espera hasta que audioFinished sea verdadero para indicar que el audio ha terminado y por último se libera el archivo de audio despues de reproducirlo. 

**Código completo**
```c
#include <stdio.h> 
#include <stdbool.h>
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_thread.h>
#include "constants.h"

typedef struct {
    Uint8* audioData;       // Puntero a los datos de audio
    Uint32 audioLength;     // Longitud de los datos de audio en bytes
    Uint32 audioPosition;   // Posición actual en los datos de audio
    SDL_bool audioFinished; // Bandera para verificar si el audio ha terminado
} AudioContext;

SDL_AudioDeviceID audioDevice;
SDL_AudioSpec audioSpec;
AudioContext audioContext;

int audio_thread(void* arg) {
    audioContext.audioPosition = 0;
    audioContext.audioFinished = SDL_FALSE;

    // Carga el archivo de audio
    if (SDL_LoadWAV("tap.wav", &audioSpec, &audioContext.audioData, &audioContext.audioLength) == NULL) {
        printf("Unable to load WAV file: %s\n", SDL_GetError());
        return -1;
    }
    printf("Audio loaded successfully\n");

    // Inicia la reproducción de audio
    SDL_PauseAudioDevice(audioDevice, 0);
    printf("Audio playback started\n");

    // Espera a que termine la reproducción de audio
    while (!audioContext.audioFinished) {
        SDL_Delay(10); // Pequeña espera para no sobrecargar el CPU
    }

    // Libera el archivo de audio después de la reproducción
    SDL_FreeWAV(audioContext.audioData);
    printf("Audio finished\n");

    return 0;
}

void AudioCallback(void* userdata, Uint8* stream, int len) {
    AudioContext* audioContext = (AudioContext*)userdata;

    if (audioContext->audioPosition >= audioContext->audioLength) {
        audioContext->audioFinished = SDL_TRUE;
        printf("AudioCallback: Audio finished flag set\n");
        return;
    }

    int remainingBytes = audioContext->audioLength - audioContext->audioPosition;
    int bytesToCopy = (len < remainingBytes) ? len : remainingBytes;

    SDL_memcpy(stream, audioContext->audioData + audioContext->audioPosition, bytesToCopy);
    audioContext->audioPosition += bytesToCopy;
}


void play_audio(void) {
    if (audioContext.audioData != NULL) {
        SDL_FreeWAV(audioContext.audioData); // Libera datos anteriores si existen
    }
    SDL_Thread* thread = SDL_CreateThread(audio_thread, "AudioThread", NULL);
    if (thread == NULL) {
        printf("Unable to create audio thread: %s\n", SDL_GetError());
    }
    else {
        SDL_DetachThread(thread);
    }
}

int game_is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int last_frame_time = 0;

struct game_object {
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle;

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false;
    }

    audioSpec.freq = 44100;
    audioSpec.format = AUDIO_S16SYS;
    audioSpec.channels = 1;
    audioSpec.samples = 2048;
    audioSpec.callback = AudioCallback;
    audioSpec.userdata = &audioContext;

    audioDevice = SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);
    if (audioDevice == 0) {
        fprintf(stderr, "Unable to open audio device: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        game_is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            game_is_running = false;
        if (event.key.keysym.sym == SDLK_LEFT)
            paddle.vel_x = -400;
        if (event.key.keysym.sym == SDLK_RIGHT)
            paddle.vel_x = +400;
        if (event.key.keysym.sym == SDLK_p) {
            play_audio(); // Inicia la reproducción de audio en un hilo independiente
        }
        break;
    case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT)
            paddle.vel_x = 0;
        break;
    }
}

void setup(void) {
    ball.width = 15;
    ball.height = 15;
    ball.x = 20;
    ball.y = 20;
    ball.vel_x = 300;
    ball.vel_y = 300;

    paddle.width = 100;
    paddle.height = 20;
    paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
    paddle.y = WINDOW_HEIGHT - 40;
    paddle.vel_x = 0;
    paddle.vel_y = 0;
}

void update(void) {
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);

    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;
    last_frame_time = SDL_GetTicks();

    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;
    paddle.x += paddle.vel_x * delta_time;
    paddle.y += paddle.vel_y * delta_time;

    if (ball.x <= 0 || ball.x + ball.width >= WINDOW_WIDTH)
        ball.vel_x = -ball.vel_x;
    if (ball.y < 0)
        ball.vel_y = -ball.vel_y;

    if (ball.y + ball.height >= paddle.y && ball.x + ball.width >= paddle.x && ball.x <= paddle.x + paddle.width)
        ball.vel_y = -ball.vel_y;

    if (paddle.x <= 0)
        paddle.x = 0;
    if (paddle.x >= WINDOW_WIDTH - paddle.width)
        paddle.x = WINDOW_WIDTH - paddle.width;

    if (ball.y + ball.height > WINDOW_HEIGHT) {
        ball.x = WINDOW_WIDTH / 2;
        ball.y = 0;
    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect ball_rect = { (int)ball.x, (int)ball.y, (int)ball.width, (int)ball.height };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    SDL_Rect paddle_rect = { (int)paddle.x, (int)paddle.y, (int)paddle.width, (int)paddle.height };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle_rect);

    SDL_RenderPresent(renderer);
}

void destroy_window(void) {
    SDL_CloseAudioDevice(audioDevice);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* args[]) {
    game_is_running = initialize_window();
    setup();

    while (game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    return 0;
}

```
**Uso del semáforo**

El uso del semaforo se podría usar en este código para sincronizar la detección de un evento con el inicio de la reproducción del audio en el hilo, el evento en este caso es presionar la tecla P. esta implementación nos ayuda a evitar que el audio se reproduzca sin motivo. 

**Implementación del semáforo en el código**

```c
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_thread.h>
#include "constants.h"

typedef struct {
    Uint8* audioData;       // Puntero a los datos de audio
    Uint32 audioLength;     // Longitud de los datos de audio en bytes
    Uint32 audioPosition;   // Posición actual en los datos de audio
    SDL_bool audioFinished; // Bandera para verificar si el audio ha terminado
} AudioContext;

SDL_AudioDeviceID audioDevice;
SDL_AudioSpec audioSpec;
AudioContext audioContext;
SDL_sem* audioSemaphore;   // Puntero del Semáforo global para sincronizar el evento de audio

int audio_thread(void* arg) {
    while (true) {
        // Espera a que el semáforo sea incrementado por el hilo principal
        SDL_SemWait(audioSemaphore);

        // Reinicia la posición del audio y la bandera de terminado
        audioContext.audioPosition = 0;
        audioContext.audioFinished = SDL_FALSE;

        // Carga el archivo de audio
        if (SDL_LoadWAV("tap.wav", &audioSpec, &audioContext.audioData, &audioContext.audioLength) == NULL) {
            printf("Unable to load WAV file: %s\n", SDL_GetError());
            continue;
        }

        // Inicia la reproducción de audio
        SDL_PauseAudioDevice(audioDevice, 0);
        printf("Audio playback started\n");

        // Espera a que termine la reproducción de audio
        while (!audioContext.audioFinished) {
            SDL_Delay(10); // Pequeña espera para no sobrecargar el CPU
        }

        // Libera el archivo de audio después de la reproducción
        SDL_FreeWAV(audioContext.audioData);
        printf("Audio finished\n");
    }

    return 0;
}

void AudioCallback(void* userdata, Uint8* stream, int len) {
    AudioContext* audioContext = (AudioContext*)userdata;

    if (audioContext->audioPosition >= audioContext->audioLength) {
        audioContext->audioFinished = SDL_TRUE;
        printf("AudioCallback: Audio finished flag set\n");
        return;
    }

    int remainingBytes = audioContext->audioLength - audioContext->audioPosition;
    int bytesToCopy = (len < remainingBytes) ? len : remainingBytes;

    SDL_memcpy(stream, audioContext->audioData + audioContext->audioPosition, bytesToCopy);
    audioContext->audioPosition += bytesToCopy;
}

void play_audio(void) {
    // Incrementa el semáforo para que el hilo de audio comience la reproducción
    SDL_SemPost(audioSemaphore);
}

int game_is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int last_frame_time = 0;

struct game_object {
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle;

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false;
    }

    audioSpec.freq = 44100;
    audioSpec.format = AUDIO_S16SYS;
    audioSpec.channels = 1;
    audioSpec.samples = 2048;
    audioSpec.callback = AudioCallback;
    audioSpec.userdata = &audioContext;

    audioDevice = SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);
    if (audioDevice == 0) {
        fprintf(stderr, "Unable to open audio device: %s\n", SDL_GetError());
        return false;
    }

    // Inicializa el semáforo para el hilo de audio
    audioSemaphore = SDL_CreateSemaphore(0);

    // Crea el hilo de audio
    SDL_Thread* thread = SDL_CreateThread(audio_thread, "AudioThread", NULL);
    if (thread == NULL) {
        printf("Unable to create audio thread: %s\n", SDL_GetError());
    } else {
        SDL_DetachThread(thread);
    }

    return true;
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        game_is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            game_is_running = false;
        if (event.key.keysym.sym == SDLK_LEFT)
            paddle.vel_x = -400;
        if (event.key.keysym.sym == SDLK_RIGHT)
            paddle.vel_x = +400;
        if (event.key.keysym.sym == SDLK_p) {
            play_audio(); // Inicia la reproducción de audio en el hilo de audio sincronizado con el semáforo
        }
        break;
    case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT)
            paddle.vel_x = 0;
        break;
    }
}

void setup(void) {
    ball.width = 15;
    ball.height = 15;
    ball.x = 20;
    ball.y = 20;
    ball.vel_x = 300;
    ball.vel_y = 300;

    paddle.width = 100;
    paddle.height = 20;
    paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
    paddle.y = WINDOW_HEIGHT - 40;
    paddle.vel_x = 0;
    paddle.vel_y = 0;
}

void update(void) {
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);

    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;
    last_frame_time = SDL_GetTicks();

    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;
    paddle.x += paddle.vel_x * delta_time;
    paddle.y += paddle.vel_y * delta_time;

    if (ball.x <= 0 || ball.x + ball.width >= WINDOW_WIDTH)
        ball.vel_x = -ball.vel_x;
    if (ball.y < 0)
        ball.vel_y = -ball.vel_y;

    if (ball.y + ball.height >= paddle.y && ball.x + ball.width >= paddle.x && ball.x <= paddle.x + paddle.width)
        ball.vel_y = -ball.vel_y;

    if (paddle.x <= 0)
        paddle.x = 0;
    if (paddle.x >= WINDOW_WIDTH - paddle.width)
        paddle.x = WINDOW_WIDTH - paddle.width;

    if (ball.y + ball.height > WINDOW_HEIGHT) {
        ball.x = WINDOW_WIDTH / 2;
        ball.y = 0;
    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect ball_rect = { (int)ball.x, (int)ball.y, (int)ball.width, (int)ball.height };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    SDL_Rect paddle_rect = { (int)paddle.x, (int)paddle.y, (int)paddle.width, (int)paddle.height };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle_rect);

    SDL_RenderPresent(renderer);
}

void destroy_window(void) {
    SDL_CloseAudioDevice(audioDevice);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroySemaphore(audioSemaphore); // Destruye el semáforo
    SDL_Quit();
}

int main(int argc, char* args[]) {
    game_is_running = initialize_window();
    setup();

    while (game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    return 0;
}
```
El código inicia con el **audioSemaphore** en 0, el **audio_thread** espera en **SDL_SemWait(audio_semaphore)** hasta que el semáforo sea incrementado por el hilo principal. En el **process_input**, cuando se presiona la tecla P y es detectado, el semáforo se incrementa con **SDL_SemPost(audioSemaphore)** para inicar la reproducción del sonido en el hilo independiente. 

Con ayuda de la IA y cierta investigación, decimos que el semáforo ayuda a resolver este problema porque controla cuando el hilo de audio debe reproducirse, basandose en la señal del hilo principal: 

- El hilo principal aumenta el valor del semáforo cuando se detecta el evento (presionar la tecla), dando la señal de que el hilo del audio puede iniciar la reproducción.
- Cuando el semáforo le indica, el hilo del audio procede a reproducir el audio  y espera a que vuelvan a presionar la tecla. 
