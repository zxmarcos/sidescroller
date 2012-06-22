#ifndef FRAMERATER_H
#define FRAMERATER_H

#include <SDL/SDL.h>

class FrameRater
{
    public:
        FrameRater(int _fps = 60);
        virtual ~FrameRater();

        /**
        * Inicia o sistema do limitador
        */
        void init();
        /**
        * Termina o sistema de limitador
        */
        void close();
        /**
        * Função chamada no inicio de um frame
        */
        void begin();
        /**
        * Função chamada no fim de um frame, aqui
        * que os cálculos acontecem para limitar o FPS
        */
        void end();
        int average();
    protected:
    private:
        unsigned begintick;
        unsigned endtick;
        int ticks;
        int delay;
        int	counter;
        int frames;
        int last;
        int maxfps;
        volatile int _avfps;
        SDL_TimerID id;

        static Uint32 timer(Uint32 interval, void *param);
};

#endif // FRAMERATER_H
