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
        SDL_SemWait(audio->audioSemaphore);  // Espera hasta que se active el semáforo

        // Reinicia la posición del audio y la bandera de terminado
        audio->audioPosition = 0;
        audio->audioFinished = SDL_FALSE;

        // Carga el archivo de audio
        
        if (SDL_LoadWAV(audioFilePath, &audio->audioSpec, &audio->audioData, &audio->audioLength) == NULL) {
            printf("Unable to load WAV file: %s\n", SDL_GetError());
            continue;
        }

        // Inicia la reproducción de audio
        SDL_PauseAudioDevice(audio->audioDevice, 0);
        printf("Audio playback started\n");

        // Espera a que termine la reproducción de audio
        while (!audio->audioFinished) {
            SDL_Delay(10); // Pequeña espera para no sobrecargar el CPU
        }

        // Detener el dispositivo de audio
        SDL_PauseAudioDevice(audio->audioDevice, 1);

        // Libera el archivo de audio después de la reproducción
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

    // Configuración del formato de audio
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

    // Inicializa el semáforo para el hilo de audio
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
