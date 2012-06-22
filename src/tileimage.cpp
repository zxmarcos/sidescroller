#include "tileimage.h"
#include "system.h"
#include "videosystem.h"

TileImage::TileImage(TileSet *parent) {
    tset = parent;
    filename = NULL;
    trans = NULL;
    width = 0;
    height = 0;
}

TileImage::~TileImage() {
    if (surface)
        SDL_FreeSurface(surface);
    SAFE_DELETE(trans);
    SAFE_DELETE(filename);
}

void TileImage::LogInfo() {
    Log("source: %s\n", filename);
    Log("trans: %s\n", trans);
    Log("width: %d\n", width);
    Log("height: %d\n", height);
}


void TileImage::DrawTile(SDL_Surface *dst, int id, int sx, int sy) {
    SDL_Rect r, dstr;
    // tile inválido
    if (id > (tset->nTiles))
        return;

    int line = id / tset->nXTiles;
    int pos = id - (line * tset->nXTiles);

    r.x = pos * tset->tileWidth;
    r.y = line * tset->tileHeight;
    r.w = tset->tileWidth;
    r.h = tset->tileHeight;

    dstr.x = sx;
    dstr.y = sy;
    dstr.w = r.w;
    dstr.h = r.h;
    SDL_UpperBlit(surface, &r, dst, &dstr);
}
