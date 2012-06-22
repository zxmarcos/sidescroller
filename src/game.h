#ifndef GAME_H
#define GAME_H
#include <cstdarg>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <list>
#include "entity.h"

class Game
{
    public:
        Game();
        virtual ~Game();
        int init();
        void close();
        void run();

    private:
        SDL_Surface *debugSurface;
        TTF_Font *debugFont;
        int debugLine;
        void debugPrintf(const char *fmt, ...);
        void process();
        void debug();

        void updateEntity(Entity *ent);
        void handle_slope(Entity *ent, int type);
        void process_events();

        std::list<Entity*> entitys;

};

#endif // GAME_H
