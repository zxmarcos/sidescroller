/* Sistema de video
 *
 */
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
//#include <SDL/SDL_ttf.h>
#include "videosystem.h"
#include "system.h"
#include "camera.h"
#include <cmath>


#define MAX_FPS 60

#if SCALE_SCREEN
extern void scale2x(SDL_Surface *src, SDL_Surface *dst);
extern void simple2x(SDL_Surface *src, SDL_Surface *dst);
#endif

VideoSystem::VideoSystem() {
    isInitialized = false;
}

VideoSystem::~VideoSystem() {
    //dtor
}


int VideoSystem::getfps() {
    if (framerater)
        return framerater->average();
    return 0;
}

void VideoSystem::init(int w, int h, int depth, bool fullscreen) {
    Log("Iniciando sistema de video...\n");
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0)
        LogFatal("Erro ao iniciar o sistema de video:\n%s", SDL_GetError());
    atexit(SDL_Quit);

    SDL_WM_SetCaption(ENGINE_NAME, NULL);

    Uint32 flags = SDL_SWSURFACE | SDL_DOUBLEBUF;
    if (fullscreen)
        flags |= SDL_FULLSCREEN;

    width = w;
    height = h;
    bpp = depth;
#if SCALE_SCREEN
    scaled = SDL_SetVideoMode(width*2, height*2, bpp, flags);
    if (!scaled)
        LogFatal("Erro ao criar a janela:\n%s", SDL_GetError());
    screen = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, bpp,
                                  scaled->format->Rmask,
                                  scaled->format->Gmask,
                                  scaled->format->Bmask,
                                  scaled->format->Amask
                                  );
#else
    screen = SDL_SetVideoMode(width, height, bpp, flags);
    if (!screen)
        LogFatal("Erro ao criar a janela:\n%s", SDL_GetError());
#endif


    // Inicia a SDL_image
    flags = IMG_INIT_PNG;
    if ((IMG_Init(flags) & flags) != flags)
        LogFatal("Erro ao iniciar SDL_Image:\n%s", IMG_GetError());

    // Incia o sistema limitador de frames com 60 frames por segundo
    framerater = new FrameRater(MAX_FPS);
    framerater->init();
}

void VideoSystem::SetClearColor(int r, int g, int b) {
    clearColor = SDL_MapRGB(screen->format, r, g, b);
}

void VideoSystem::clear() {
    SDL_FillRect(screen, NULL, clearColor);
}

void VideoSystem::postblit(SDL_Surface *src, SDL_Rect *sr, SDL_Rect *dr) {
    postBlits.push(new PostBlit(src, sr, dr));
}

void VideoSystem::show() {
#if SCALE_SCREEN
    //scale2x(screen, scaled);
    simple2x(screen, scaled);
    while (!postBlits.empty()) {
        PostBlit *p = postBlits.top();
        postBlits.pop();
        SDL_BlitSurface(p->src, NULL, scaled, NULL);
        SAFE_DELETE(p);
    }
    SDL_Flip(scaled);
#else
    while (!postBlits.empty()) {
        PostBlit *p = postBlits.top();
        postBlits.pop();
        SDL_BlitSurface(p->src, NULL, screen, NULL);
        SAFE_DELETE(p);
    }
    SDL_Flip(screen);
#endif
}


void VideoSystem::drawGrid() {
    int xstart = 16 - g_Camera->x() % 16;
    for (int i = 0; i < GetW()/16; i++) {
        SDL_Rect r;
        r.x = xstart + i * 16;
        r.y = 0;
        r.w = 1;
        r.h = GetH();
        SDL_FillRect(screen, &r, Color(255,0,0));
    }
    for (int i = 0; i < GetH()/16; i++) {
        SDL_Rect r;
        r.x = 0;
        r.y = i*16;
        r.w = GetW();
        r.h = 1;
        SDL_FillRect(screen, &r, Color(255,0,0));
    }
}

void VideoSystem::drawAxis(int x, int y)
{


    SDL_Rect r;
#if 0
    Uint32 col =  Color(0,0,255);
    r.x = x - 3;
    r.y = y;
    r.w = 6;
    r.h = 1;
    SDL_FillRect(screen, &r, col);
    r.x = x;
    r.y = y-3;
    r.w = 1;
    r.h = 6;
    SDL_FillRect(screen, &r, col);
#else
    Uint32 col =  Color(0,255,0);
    r.x = x-1;
    r.w = 2;
    r.h = 2;
    r.y = y-1;
    DrawRect(&r, Color(0,0,255));
    r.x = x;
    r.y = y;
    r.w = 1;
    r.h = 1;
    SDL_FillRect(screen, &r, col);
#endif
}

void VideoSystem::Blit(SDL_Surface *src, SDL_Rect *sr, SDL_Rect *dr)
{
    SDL_BlitSurface(src, sr, screen, dr);
}


Uint32 VideoSystem::Color(int r, int g, int b)
{
    return SDL_MapRGB(screen->format, r, g, b);
}

const int VideoSystem::GetW() const
{
    return width;
}
const int VideoSystem::GetH() const
{
    return height;
}

void VideoSystem::begin()
{
    framerater->begin();
}

void VideoSystem::end()
{
    framerater->end();
}

void VideoSystem::DrawRect(SDL_Rect *r, Uint32 color)
{
    if (!r)
        return;

    SDL_Rect s;
    s.x = r->x;
    s.w = r->w;
    s.y = r->y;
    s.h = 1;
    SDL_FillRect(screen, &s, color);
    s.x = r->x;
    s.w = r->w;
    s.y = r->y + r->h;
    s.h = 1;
    SDL_FillRect(screen, &s, color);
    s.x = r->x;
    s.w = 1;
    s.y = r->y;
    s.h = r->h;
    SDL_FillRect(screen, &s, color);
    s.x = r->x + r->w;
    s.w = 1;
    s.y = r->y;
    s.h = r->h;
    SDL_FillRect(screen, &s, color);
}

void VideoSystem::DrawRectAlpha(SDL_Rect *r, Uint32 color) {
#if 0
    SDL_Surface *surf = Surface(r->w, r->h, screen->format->BitsPerPixel);
    if (!surf)
        return;

    for (int y=0;y<surf->h;y++) {
        Uint16 *src = (Uint16*)(screen->pixels + ((r->y+y)*screen->pitch) + (r->x*2));
        Uint16 *dst = (Uint16*)(surf->pixels + (y*surf->pitch));
        for (int x=0;x<surf->w;x++) {
            Uint8 r1, g1, b1;
            Uint8 r2, g2, b2;
            int r3, g3, b3;
            Uint16 px;
            SDL_GetRGB(*src, screen->format, &r1, &g1, &b1);
            SDL_GetRGB(color, surf->format, &r2, &g2, &b2);
            r3 = std::min(255, ((r1*256)/256 + (r2*256)/256));
            g3 = std::min(255, ((g1*256)/256 + (g2*256)/256));
            b3 = std::min(255, ((b1*256)/256 + (b2*256)/256));
            px = SDL_MapRGB(surf->format, r3, g3, b3);
            *dst = px;
            src++;
        }
    }
    Blit(surf, NULL, r);
    SDL_FreeSurface(surf);
#endif
}

SDL_Surface *VideoSystem::Surface(int w, int h, int bpp)
{
    SDL_Surface *surf = NULL;
    surf =  SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, bpp,
                                screen->format->Rmask,
                                screen->format->Gmask,
                                screen->format->Bmask,
                                screen->format->Amask
                                );
    return surf;
}


