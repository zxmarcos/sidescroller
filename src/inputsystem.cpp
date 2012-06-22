#include "inputsystem.h"
#include "inputbackend.h"
#include "system.h"
#include <SDL/SDL.h>

InputSystem::InputSystem() {
    running = 1;
    tolerance = 1;
    be.init();
    for (int i = 0; i < __INPUT_MAX__; i++) {
        states[i].holding = 0;
        states[i].release = 0;
        states[i].pressed = 0;
    }
}

InputSystem::~InputSystem() {
}

//
void InputSystem::update() {
    SDL_Event event;
    // checa pelo sinal de fechar a janela
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = 0;
                break;
        }
    }
    be.poll();
    if (be.getstate(EXIT_CODE))
        running = 0;

    // atualiza os estados
    for (int i = 0; i < __INPUT_MAX__; i++) {
        int pressed = be.getstate((InputSymbol)i);

        // está pressionada?
        if (pressed) {
            // se já estiver pressionada desde a ultima atualização,
            // então temos um holding state
            if (states[i].pressed) {
                // verifica a tolerância ao keyrepeat
                ++states[i].time;
                if (states[i].time >= tolerance) {
                    states[i].pressed = 0;
                    states[i].release = 0;
                    states[i].holding = 1;
                    // reinicia o contador
                    states[i].time = 1;
                }
            } else if (states[i].holding)
                ++states[i].time;
            else
            {
                states[i].pressed = 1;
                states[i].release = 0;
                states[i].holding = 0;
                states[i].time = 0;
            }
        } else {
            // tecla solta
            if (states[i].pressed || states[i].holding) {
                states[i].pressed = 0;
                states[i].holding = 0;
                states[i].release = 1;
                states[i].time = 0;
            } else {
                // nada
                states[i].pressed = 0;
                states[i].holding = 0;
                if (++states[i].time >= tolerance)
                    states[i].release = 0;
            }
        }
    }
}

Keystate InputSystem::getstate(int i) {
    if (states[i].pressed)
        return Keystate(KS_PRESSED);
    else if (states[i].holding)
        return Keystate(KS_HOLDING, states[i].time);
    else if (states[i].release)
        return Keystate(KS_RELEASE);
    return Keystate(KS_UP);
}

int InputSystem::isRunning() {
    return running;
}
