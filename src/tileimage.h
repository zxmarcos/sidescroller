#ifndef TILEIMAGE_H
#define TILEIMAGE_H

#include "levelmap.h"
#include "tileset.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

class LevelMap;
class TileSet;

class TileImage
{
    friend class TileSet;
    friend class LevelMap;
    public:
        TileImage(TileSet *parent);
        virtual ~TileImage();
        void DrawTile(SDL_Surface *dst, int id, int sx, int sy);;
    protected:
        void LogInfo();
    private:
        TileSet *tset;
        SDL_Surface *surface;
        char *filename;
        char *trans;
        int width;
        int height;
};

#endif // TILEIMAGE_H
