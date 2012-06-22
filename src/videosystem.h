#ifndef VIDEOSYSTEM_H
#define VIDEOSYSTEM_H

#include <SDL/SDL.h>
#include "framerater.h"
#include <stack>


#define SCALE_SCREEN    0

class VideoSystem;

/* Classe global para acessar funções do video */
extern VideoSystem *g_Video;

struct PostBlit {
    SDL_Rect *r_src;
    SDL_Rect *r_dst;
    SDL_Surface *src;
    PostBlit(SDL_Surface *s, SDL_Rect *s_r, SDL_Rect *d_r) {
        src=s;
        r_src=s_r;
        r_dst=d_r;
    }
};

class VideoSystem
{
    public:
        VideoSystem();
        virtual ~VideoSystem();

        void init(int w, int h, int depth, bool fullscreen);
        const int GetW() const;
        const int GetH() const;

        int getfps();

        SDL_Surface *Surface(int w, int h, int bpp);
        void Blit(SDL_Surface *src, SDL_Rect *sr, SDL_Rect *dr);
        void SetClearColor(int r, int g, int b);
        void clear();
        void show();

        void begin();
        void end();
        void drawGrid();
        void drawAxis(int x, int y);

        void postblit(SDL_Surface *src, SDL_Rect *sr, SDL_Rect *dr);

        Uint32 Color(int r, int g, int b);
        // algumas primitivas
        void DrawRect(SDL_Rect *r, Uint32 color);
        void DrawRectAlpha(SDL_Rect *r, Uint32 color);

    private:
        bool isInitialized;
        FrameRater *framerater;
#if SCALE_SCREEN
        SDL_Surface *screen, *scaled;
#else
        SDL_Surface *screen;
#endif
        int width;
        int height;
        int bpp;

        std::stack<PostBlit*> postBlits;

        Uint32 clearColor;
};



#endif // VIDEOSYSTEM_H
