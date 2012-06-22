#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H
#include "sffv2loader.h"
#include <map>

#include <boost/shared_ptr.hpp>
using namespace std;
using namespace boost;
using namespace SFFv2;

typedef boost::shared_ptr<SFFV2Loader> SFFV2LoaderPtr;

class SpriteManager
{

    public:
        /** Default constructor */
        SpriteManager();
        /** Default destructor */
        ~SpriteManager();

        SFFV2LoaderPtr open(const char *str);
    private:
        struct Resource {
            const char *name;
            SFFV2LoaderPtr ptr;
        };
        list<Resource*> resources;
};

extern SpriteManager *g_SpriteManager;

#endif // SPRITEMANAGER_H
