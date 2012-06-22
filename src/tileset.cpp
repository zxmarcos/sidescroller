#include "tileset.h"
#include "system.h"

TileSet::TileSet()
{
    //ctor
}

TileSet::~TileSet()
{
    SAFE_DELETE(name);
    SAFE_DELETE(image);
}

void TileSet::LogInfo()
{
    Log("name: %s\n", name);
    Log("gid: %d\n", firstGID);
    Log("width: %d\n", tileWidth);
    Log("height: %d\n", tileHeight);
    Log("spacing: %d\n", spacing);
    Log("margin: %d\n", margin);
}
