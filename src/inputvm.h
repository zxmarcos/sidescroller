#ifndef INPUTVM_H
#define INPUTVM_H
#include "game.h"
#include "inputsystem.h"
#include <SDL/SDL.h>
#include <vector>


#define KS_BUF_MAX    10

struct InputStream {
    std::vector<XInputstate*> stream;
    inline void push(XInputstate *p) {
        if (p)
            stream.push_back(p);
    }
};

class InputVM
{
    friend class Game;
    public:
        /** Default constructor */
        InputVM();
        /** Default destructor */
        virtual ~InputVM();

        void addcommand(const char *name, char *fmt);
        void update();
    private:
        void shiftBuffer();
        void push(XInputstate& st);
        bool isLast(XInputstate& st);
        bool isNeutral(XInputstate& st);
        void checkForCommands();
        Uint32 blankcount;
        void updateBuffer();
        Uint32 last;
        XInputstate kbuffer[KS_BUF_MAX];
        int bufpos;
};

#endif // INPUTVM_H
