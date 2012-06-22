#include "layer.h"
#include "system.h"

Layer::Layer()
{
	parallax = 0;
    //ctor
}

Layer::~Layer()
{
    SAFE_DELETE(nTileArray);
    SAFE_DELETE(name);
}

void Layer::logInfo() {
    Log("name: %s\n", name);
    Log("width: %d\n", width);
    Log("height: %d\n", height);
    Log("visible: %d\n", visible);
    Log("opacity: %f\n", opacity);
}
