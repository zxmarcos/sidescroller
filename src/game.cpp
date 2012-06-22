/*
 * 16/06/2012 - Renderização dos cenários otimizada, se antes para
 *              cada render em posição diferente nós renderizavamos tudo,
 *              agora renderizamos apenas uma vez em uma distancia de 16 pixels.
 *              Faltando fazer o mesmo para o eixo y
 */
#include <list>
#include <SDL/SDL_ttf.h>
#include <string>
#include <cstdlib>
#include "game.h"
#include "levelmap.h"
#include "camera.h"
#include "videosystem.h"
#include "inputsystem.h"
#include "inputbackend.h"
#include "inputvm.h"
#include "system.h"
#include "entity.h"
#include "spritemanager.h"

using namespace std;

#define DEBUG_SHOW  1

#define TEST_ENT 0

LevelMap *g_Map;
VideoSystem *g_Video;
InputSystem *g_Input;
SpriteManager *g_SpriteManager;
AnimManager *g_AnimManager;
Camera *g_Camera;
Entity *g_Ent, *VE[TEST_ENT];
InputVM *iVM;


Game::Game() {
    Log("Criando a classe principal...\n");
}

Game::~Game() {
    #if TEST_ENT
    for(int i=0;i<TEST_ENT;i++)
        delete VE[i];
    #endif
    delete g_Ent;
    delete g_SpriteManager;
    delete g_Input;
    delete g_Map;
    delete g_Video;
    delete g_Camera;
}

int Game::init()
{
    g_Video = new VideoSystem();
    g_Input = new InputSystem();
    g_Camera = new Camera();
    g_Video->init(320, 240, 16, false);
    g_SpriteManager = new SpriteManager();
    g_AnimManager = new AnimManager();


    TTF_Init();

    debugSurface = g_Video->Surface(320*2, 240/4, 16);
   // SDL_SetColorKey(debugSurface, SDL_SRCCdOLORKEY|SDL_RLEACCEL, 0);
    debugFont = TTF_OpenFont("DroidSansMono.ttf", 12);

    g_Map = new LevelMap();
    g_Ent = new Entity();
    for(int i=0;i<TEST_ENT;i++)
        VE[i]=new Entity();

    g_Ent->open("samus.lua");
    g_Map->open("smb3.tmx");

    g_Camera->setMap(g_Map);
    g_Camera->setMainEntity(g_Ent);

    g_Video->SetClearColor(255,255,0);

    iVM = new InputVM();

    entitys.push_back(g_Ent);

#if TEST_ENT
    for(int i=0;i<TEST_ENT;i++)
        VE[i]->open("samus.lua");
    for(int i=0;i<TEST_ENT;i++)
        g_Camera->addEntity(VE[i]);
    for(int i=0;i<TEST_ENT;i++) {
        VE[i]->setx((int)(VE[i]->x()+rand()%(TEST_ENT*3))%(g_Map->w()*16));
        VE[i]->sety((int)(VE[i]->x()+rand()%(TEST_ENT*3))%(g_Map->h()*16));
    }
    for(int i=0;i<TEST_ENT;i++)
        entitys.push_back(VE[i]);
#endif
    return 1;
}
void Game::close()
{
    SAFE_DELETE(g_Camera);
    SAFE_DELETE(g_Input);
    SAFE_DELETE(g_Video);
}
void Game::run() {
    while (g_Input->isRunning()) {

        g_Video->begin();
        g_Input->update();
        iVM->update();
        g_Ent->update();
#if TEST_ENT
        for(int i=0;i<TEST_ENT;i++)
        VE[i]->update();
#endif
        g_Camera->update();



        process_events();
#if DEBUG_SHOW
        SDL_FillRect(debugSurface, NULL, g_Video->Color(0,0,0));
        debugPrintf("FPS %d", g_Video->getfps());
        debug();
#endif
        g_Video->show();

        g_Video->end();
    }
}


void Game::handle_slope( Entity *ent, int type ) {
    int footx = ent->x();
    int footy = ent->y();

    int xin = footx % 16;
    int yin = footy % 16;

    bool toright = (ent->x() >= ent->lastx());

    int y = footy;
    int x = footx;

    // saltando???
    if (ent->vely() >= 0) {
        switch (type) {
            case TILE_SLOPER:
                y = y + (16 - yin) - xin - (toright ? 1 : 0);
                break;
            case TILE_SOLID:
                if (toright)
                    y -= xin;
                break;
        }
    }
    ent->setFaced(false);
    ent->setGrounded( true);
    ent->sety(y);
    ent->setx(x);
}

// eventos fisicos do jogo
void Game::process_events()
{
    for (list<Entity*>::iterator i=entitys.begin(); i != entitys.end(); i++) {
        Entity *ent = *i;
        if (ent->x() > g_Map->xmax()) {
            ent->setx(g_Map->xmax()-g_Map->tilew());
            ent->setvelx(0);
        } else if (ent->x() < 0) {
            ent->setx(g_Map->tilew());
            ent->setvelx(0);
        }

        if (ent->y() > g_Map->ymax()) {
            ent->sety(g_Map->ymax()-g_Map->tileh());
            ent->setvely(0);
        }

        int footx = ent->x();
        int footy = ent->y();

        int headx = ent->x();
        int heady = ent->y() - ent->height();
        int facex1 = ent->x() + (ent->isFacing() ? - (ent->width() / 2) : ent->width() / 2);
        int facey1 = ent->y();
        int facey2 = ent->y() - ent->height();

        //int xin = footx % 16;
        //int yin = footy % 16;

        int head = g_Map->typeAt(headx, heady);
        int foot = g_Map->typeAt(footx, footy);
        int face = 0;
        bool collided = false;
        bool slope = false;

        for (int i = facey2; i < facey1; i += g_Map->tileh()) {
            if (g_Map->typeAt(facex1, i) == TILE_SOLID) {
                face = 1;
                break;
            }
        }

        // detecta axis adjacentes
        if (foot == TILE_SOLID && (g_Map->typeAt(footx, footy - 15) == TILE_SLOPER))
            slope = true;
#if 1
        for (list<Entity*>::iterator i=entitys.begin(); i != entitys.end(); i++) {
            Entity *xent = *i;
#else
        foreach (Entity *xent, entitys)  {
#endif
            if (xent != ent) {
                collided = ent->checkCol(xent);
                if (collided)
                    break;
            }
        }

        ent->setHeaded(head == TILE_SOLID);
        ent->setFaced(face ? true : false);
        if (slope) {
            handle_slope(ent, foot);
        } else
        switch (foot) {
            case TILE_SOLID:
            case TILE_GROUNDABLE:

                ent->setGrounded(true);
                break;
            case TILE_SLOPER:
            case TILE_SLOPEL:
            case TILE_SLOPEFLAG:
            case TILE_SLOPELINK:
                handle_slope(ent, foot);
                break;

            default:
                ent->setGrounded(false);
                break;
        }

        ent->setCollided(collided);
#if DEBUG_SHOW

#if 0
        debugPrintf("debug para %s", ent->myname());
        debugPrintf("in %2d,%2d, %d", xin, yin, slope);
        debugPrintf("collided %d, faced %d", collided?1:0, face);
        debugPrintf("foot %d,%d = %s", footx, footy, TileTypeName[foot]);
        debugPrintf("head %d,%d = %s", headx, heady, TileTypeName[head]);
        debugPrintf("vel %f,%f", ent->velx(), ent->vely());
        debugPrintf("pos %f,%f", ent->x(), ent->y());
#endif
#endif
    }

}

void Game::debugPrintf(const char *fmt, ...) {
#if DEBUG_SHOW
    va_list va;
    va_start(va, fmt);
    char buffer[256];
    memset(buffer, 0, 256);
    vsprintf(buffer, fmt, va);
    SDL_Color fg;
    fg.r = 255*0.7;
    fg.g = 255*0.7;
    fg.b = 255*0.7;
    SDL_Rect r;
    r.x = 0;
    r.y = debugLine * 13;
    r.w = debugSurface->w;
    r.h = 12;
    SDL_Surface *text = TTF_RenderText_Solid(debugFont, buffer, fg);
    SDL_BlitSurface(text, NULL, debugSurface, &r);
    SDL_FreeSurface(text);
    debugLine++;
    if (debugLine >= 20)
        debugLine = 0;
    va_end(va);
#endif
}

void Game::debug()
{
#if DEBUG_SHOW
    debugPrintf("SX ULDRABCXYZ");
    char _buf[256];
    sprintf(_buf, "%d%d %d%d%d%d%d%d%d%d%d%d",
            g_Input->be.getstate(IBS_START),
            g_Input->be.getstate(IBS_SELECT),
            g_Input->be.getstate(IBS_UP),
            g_Input->be.getstate(IBS_LEFT),
            g_Input->be.getstate(IBS_DOWN),
            g_Input->be.getstate(IBS_RIGHT),
            g_Input->be.getstate(IBS_BUTTON_A),
            g_Input->be.getstate(IBS_BUTTON_B),
            g_Input->be.getstate(IBS_BUTTON_C),
            g_Input->be.getstate(IBS_BUTTON_X),
            g_Input->be.getstate(IBS_BUTTON_Y),
            g_Input->be.getstate(IBS_BUTTON_Z)
            );
    debugPrintf(_buf);

#if 1
    string *s = new string();

    for (int i = iVM->bufpos-1; i >= 0 ; i--) {
        int thiscmd = 0;
        for (int n = 0; n < __INPUT_MAX__; n++) {
            if (iVM->kbuffer[i].sym[n].state == KS_UP)
                continue;
            else
            if (iVM->kbuffer[i].sym[n].state == KS_PRESSED) {
                if (thiscmd) s->append("+"); ++thiscmd;
                s->append(InputSymbolName[n]);
            }
            else
            if (iVM->kbuffer[i].sym[n].state == KS_RELEASE) {
                if (thiscmd) s->append("+"); ++thiscmd;
                s->append("~");
                s->append(InputSymbolName[n]);
            }
            else
            if (iVM->kbuffer[i].sym[n].state == KS_HOLDING) {
                if (thiscmd) s->append("+"); ++thiscmd;
                s->append("/");
                s->append(InputSymbolName[n]);
            }
        }
        s->append(",");
        thiscmd = 0;
    }

    const char *str = s->c_str();
    debugPrintf(str);
#if 1
    if (s->size() > 1) {
        s->append("\n");
        Log(str);
    }
#endif
    //if (str)
    //delete str;
    //if (s)
    //delete s;
#endif
    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.w = 200;
    r.h = debugLine * 13;

    g_Video->postblit(debugSurface, &r, NULL);
    debugLine = 0;
#endif
}
