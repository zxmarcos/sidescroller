#include <boost/foreach.hpp>

#include "colobjectgroup.h"
#include "system.h"


using namespace std;

#define MODULE_NAME "ColObjectGroup:"

ColObjectGroup::ColObjectGroup() {
}

ColObjectGroup::~ColObjectGroup() {
    foreach (ColObject *obj, listObjs)
        SAFE_DELETE(obj);
}

void ColObjectGroup::AddObj(ColObject *obj) {
    if (!obj) {
        Log(MODULE_NAME "Tentando adicionar objeto nulo!\n");
        return;
    }
    listObjs.push_back(obj);
}

// checa colisão entre um obj e todos os objetos do grupo
int ColObjectGroup::operator==(ColObject& obj) {
    foreach (ColObject *objb, listObjs)
        if (obj == *objb)
            return 1;
    return 0;
}

void ColObjectGroup::Draw(int x, int y) {
    foreach (ColObject *obj, listObjs)
        obj->Draw(x, y);
}

void ColObjectGroup::SetName(const char *str) {
    name = (char *) str;
}
const char *ColObjectGroup::GetName() const {
    return name;
}
