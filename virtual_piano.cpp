#include <iostream>
#include <ncurses.h>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>
#include <cmath>

// Frequencies for musical notes
float freq[7] = {130.81, 146.83, 164.81, 174.61, 196.00, 220.00, 246.94};

// Play a sound using SDL2
void playSound(float frequency, int duration) {
    const int AMPLITUDE = 28000; // Volume
    const int SAMPLE_RATE = 44100; // Sampling rate

    SDL_AudioSpec spec;
    spec.freq = SAMPLE_RATE;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = 2048;
    spec.callback = [](void* userdata, Uint8* stream, int len) {
        static float phase = 0.0f;
        float frequency = *(float*)userdata;
        Sint16* buffer = (Sint16*)stream;
        for (int i = 0; i < len / 2; ++i) {
            buffer[i] = (Sint16)(AMPLITUDE * sin(phase));
            phase += 2.0f * M_PI * frequency / SAMPLE_RATE;
            if (phase > 2.0f * M_PI) {
                phase -= 2.0f * M_PI;
            }
        }
    };
    spec.userdata = &frequency;

    // Open the audio device and play sound
    SDL_AudioDeviceID device = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
    if (device == 0) {
        std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_PauseAudioDevice(device, 0); // Start playback
    std::this_thread::sleep_for(std::chrono::milliseconds(duration)); // Wait for duration
    SDL_CloseAudioDevice(device); // Stop playback
}

int main() {
    // Initialize SDL2 for sound
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Initialize ncurses for key input
    initscr();        // Start ncurses mode
    cbreak();         // Disable line buffering
    noecho();         // Don't echo input characters
    keypad(stdscr, TRUE); // Enable function keys

    printw("Press keys A, S, D, F, J, K, L to play notes.\n");
    printw("Press Q to quit.\n");
    refresh();

    bool running = true;
    while (running) {
        int ch = getch(); // Get a key press
        switch (ch) {
            case 'a': case 'A': playSound(freq[0], 500); break;
            case 's': case 'S': playSound(freq[1], 500); break;
            case 'd': case 'D': playSound(freq[2], 500); break;
            case 'f': case 'F': playSound(freq[3], 500); break;
            case 'j': case 'J': playSound(freq[4], 500); break;
            case 'k': case 'K': playSound(freq[5], 500); break;
            case 'l': case 'L': playSound(freq[6], 500); break;
            case 'q': case 'Q': running = false; break; // Exit the loop
        }
    }

    // Cleanup
    endwin(); // End ncurses mode
    SDL_Quit(); // Cleanup SDL

    return 0;
}
