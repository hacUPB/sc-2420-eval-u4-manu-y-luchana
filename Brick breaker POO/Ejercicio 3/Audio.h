#ifndef AUDIO_H
#define AUDIO_H

#include <SDL.h>

typedef struct {
    Uint8* audioData;       // Puntero a los datos de audio
    Uint32 audioLength;     // Longitud de los datos de audio en bytes
    Uint32 audioPosition;   // Posición actual en los datos de audio
    SDL_bool audioFinished; // Bandera para verificar si el audio ha terminado
    SDL_AudioDeviceID audioDevice;
    SDL_AudioSpec audioSpec;
    SDL_sem* audioSemaphore;   // Puntero del Semáforo global para sincronizar el evento de audio
    const char* fileName;    // Nombre del archivo de audio
} Audio;

// Constantes para los archivos de audio
extern const char* TAP_AUDIO_FILE;
extern const char* DESTRUIR_AUDIO_FILE;

void Audio_init(Audio* audio, const char* fileName);
void Audio_destroy(Audio* audio);
void Audio_startPlayback(Audio* audio);

#endif // AUDIO_H
