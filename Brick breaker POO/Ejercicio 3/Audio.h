// audio.h
#ifndef AUDIO_H
#define AUDIO_H

#include <SDL.h>

typedef struct {
    Uint8* audioData;       // Puntero a los datos de audio
    Uint32 audioLength;     // Longitud de los datos de audio en bytes
    Uint32 audioPosition;   // Posici�n actual en los datos de audio
    SDL_bool audioFinished; // Bandera para verificar si el audio ha terminado
    SDL_AudioDeviceID audioDevice;
    SDL_AudioSpec audioSpec;
    SDL_sem* audioSemaphore;   // Puntero del Sem�foro global para sincronizar el evento de audio
} Audio;

void Audio_init(Audio* audio);
void Audio_destroy(Audio* audio);
void Audio_startPlayback(Audio* audio);

#endif // AUDIO_H

