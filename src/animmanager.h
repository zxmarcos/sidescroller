#ifndef ANIMMANAGER_H
#define ANIMMANAGER_H
#include "airloader.h"
#include <map>
#include <boost/shared_ptr.hpp>
using namespace std;
using namespace boost;

class AIRLoader;
typedef boost::shared_ptr<AIRLoader> AIRLoaderPtr;

class AnimManager
{

    public:
        /** Default constructor */
        AnimManager();
        /** Default destructor */
        ~AnimManager();

        AIRLoaderPtr open(const char *str);
    private:
        struct Resource {
            const char *name;
            AIRLoaderPtr ptr;
        };
        list<Resource*> resources;
};

extern AnimManager *g_AnimManager;

#endif // ANIMMANAGER_H
