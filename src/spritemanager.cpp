
#include <map>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include "spritemanager.h"
#include "sffv2loader.h"
#include "system.h"

using namespace std;
using namespace boost;
using namespace SFFv2;

SpriteManager::SpriteManager() {
    resources.clear();
}

SpriteManager::~SpriteManager() {
    foreach (Resource *res, resources) {
        delete res;
    }
}

SFFV2LoaderPtr SpriteManager::open(const char *str) {
    foreach (Resource *res, resources) {
        if (!stricmp(res->name, str)) {
          //  Log("Recurso já alocado %s\n", str);
            return res->ptr;
        }
    }
    Log("Criando novo recurso %s...\n", str);
    Resource *res = new Resource;
    resources.push_back(res);

    SFFV2LoaderPtr file(new SFFV2Loader);
    res->name = str;
    res->ptr = file;
    file->open(str);

    return res->ptr;
}
