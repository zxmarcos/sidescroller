#ifndef LAYER_H
#define LAYER_H
#include <SDL/SDL.h>
#include "levelmap.h"
#include "property.h"

class LevelMap;
class Layer
{
    friend class LevelMap;
    public:
        Layer();
        virtual ~Layer();
    protected:
        void logInfo();
    private:
        Property props;
        unsigned int *nTileArray;
        char *name;
        int visible;
        float opacity;
        int width;
        int height;
        int nid;
		int parallax;
		int fixed;
};

#endif // LAYER_H
