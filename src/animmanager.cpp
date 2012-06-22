
#include <map>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include "animmanager.h"
#include "airloader.h"
#include "system.h"

using namespace std;
using namespace boost;

AnimManager::AnimManager() {
    resources.clear();
}

AnimManager::~AnimManager() {
    foreach (Resource *res, resources) {
        delete res;
    }
}

AIRLoaderPtr AnimManager::open(const char *str) {
    foreach (Resource *res, resources) {
        if (!stricmp(res->name, str)) {
          //  Log("Recurso já alocado %s\n", str);
            return res->ptr;
        }
    }
    Log("Criando novo recurso %s...\n", str);
    Resource *res = new Resource;
    resources.push_back(res);

    AIRLoaderPtr file(new AIRLoader);
    res->name = str;
    res->ptr = file;
    file->open(str);

    return res->ptr;
}
