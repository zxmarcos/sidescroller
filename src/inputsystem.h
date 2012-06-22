#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "inputbackend.h"

class InputSystem;

extern InputSystem *g_Input;

enum {
    KS_UP = 0, // não pressionada
    KS_PRESSED,
    KS_HOLDING,
    KS_RELEASE,
};

struct Keystate {
    unsigned char state;
    unsigned time;
    Keystate(char st = KS_UP, int ntime = 0) {
        state = st;
        time = ntime;
    }
    bool operator==(Keystate& right) {
        if (state == right.state)
            return true;
        return false;
    }
};


struct XInputstate {
    Keystate sym[__INPUT_MAX__];
};


class InputVM;
class InputSystem
{
    friend class Game;
    friend class InputVM;
    public:
        InputSystem();
        ~InputSystem();
        void update();
        int isRunning();

        struct Keystate getstate(int i);
    private:
        unsigned tolerance;
        struct keystate_internal {
            unsigned pressed : 1;
            unsigned holding : 1;
            unsigned release : 1;
            unsigned int time;
        } states[__INPUT_MAX__];
        int running;
        InputBackend be;

};

#endif // INPUTSYSTEM_H
