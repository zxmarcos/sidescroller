#include "inputbackend.h"
#include <SDL/SDL.h>

InputBackend::InputBackend() {
}

InputBackend::~InputBackend() {
}

const char *InputSymbolName[__INPUT_MAX__] = {
    "",
    "s", "S", "U", "D", "B", "F",
    "a", "b", "c", "x", "y", "z"
};

int InputBackend::init() {
    memset(&stateTable, 0, sizeof(stateTable));
    memset(&bindTable, 0, sizeof(bindTable));
    lastPoll = 0;
    bindTable[IBS_START] = SDLK_RETURN;
    bindTable[IBS_SELECT] = SDLK_SPACE;
    bindTable[IBS_UP] = SDLK_UP;
    bindTable[IBS_DOWN] = SDLK_DOWN;
    bindTable[IBS_LEFT] = SDLK_LEFT;
    bindTable[IBS_RIGHT] = SDLK_RIGHT;
    bindTable[IBS_BUTTON_A] = SDLK_a;
    bindTable[IBS_BUTTON_B] = SDLK_s;
    bindTable[IBS_BUTTON_C] = SDLK_d;
    bindTable[IBS_BUTTON_X] = SDLK_z;
    bindTable[IBS_BUTTON_Y] = SDLK_x;
    bindTable[IBS_BUTTON_Z] = SDLK_c;
    bindTable[EXIT_CODE] = SDLK_ESCAPE;
    return 1;
}

void InputBackend::close() {
}

int InputBackend::poll() {
    // tempo mínimo de polling
    if ((SDL_GetTicks() - lastPoll) < 60)
        return 0;

    SDL_PumpEvents();
    keystate = SDL_GetKeyState(&numkeys);
    // verificamos simbolo por simbolo
    for (int i = IBS_START; i < __INPUT_MAX__; i++) {
        // pega o valor codificado do simbolo
        unsigned int word = bindTable[i];

        // por agora, só suportamos o teclado
        if (keystate[word])
            stateTable[i] = 1;
        else
            stateTable[i] = 0;
    }
    lastPoll = SDL_GetTicks();
    return 1;
}

int InputBackend::getstate(InputSymbol inp) {
    return stateTable[inp];
}
int InputBackend::bind(InputSymbol inp, int code) {
    bindTable[inp] = code;
    return 1;
}

int InputBackend::find() {
    SDL_PumpEvents();
    keystate = SDL_GetKeyState(&numkeys);
    // encontra a primeira tecla pressionada
    for (int i = 0; i < numkeys; i++) {
        if (keystate[i])
            return i;
    }
    return 0;
}
