#ifndef TILESET_H
#define TILESET_H
#include "levelmap.h"
#include "tileimage.h"
#include "property.h"

class LevelMap;
class TileImage;

class TileSet
{
    friend class LevelMap;
    friend class TileImage;
    public:
        TileSet();
        virtual ~TileSet();
    protected:
        void LogInfo();
    private:
        Property props;
        char *name;
		int *typeArray;
        int firstGID;
        int nTiles;
        int nXTiles;
        int nYTiles;
        int tileWidth;
        int tileHeight;
        int spacing;
        int margin;
        TileImage *image;
};

#endif // TILESET_H
