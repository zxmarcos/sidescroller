#include "framerater.h"
#include <SDL/SDL.h>

FrameRater::FrameRater(int _fps) : maxfps(_fps) {
}

FrameRater::~FrameRater() {
}

void FrameRater::init() {
    id = SDL_AddTimer(1000, timer, this);
    if (!maxfps)
        maxfps = 60;
    counter = 0;
    ticks = 0;
    begintick = 0;
    endtick = 0;
    last = 0;
    delay = 0;
    _avfps = 60;
}

void FrameRater::close() {
    SDL_RemoveTimer(id);
}

//
void FrameRater::begin() {
    begintick = SDL_GetTicks();
}

void FrameRater::end() {
    endtick = SDL_GetTicks();

    // Calculamos o tempo em ticks do frame
    ticks = endtick - begintick;

    // Calcula o delay para esse frame
    float tps = 1000.0 / maxfps;
    delay = tps - ticks - last;

    // verifica se o delay é maior que zero, fazemos isso porque
    // o frame pode ter sido mais lento que o anterior, logo o
    // delay desse frame terá que ser subtraido no próximo frame.
    if (delay > 0) {
        SDL_Delay(delay);
        last = 0;
    } else {
        last = delay;
    }
    // incrementa o nosso contador de frames por segundo
    ++counter;
}

int FrameRater::average() {
    return _avfps;
}

// sdl timer callback
Uint32 FrameRater::timer(Uint32 interval, void *param) {
    FrameRater *p = reinterpret_cast<FrameRater*>(param);

    p->_avfps = p->counter;
    char title[128];
    sprintf(title, "FPS: %d", p->counter);
    SDL_WM_SetCaption(title, NULL);

    p->counter = 1;
    return interval;
}
