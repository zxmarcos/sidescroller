#include "animmap.h"
#include "system.h"
#include "airloader.h"
#include "animmanager.h"

AnimMap::AnimMap() {
    action = NULL;
    frame = NULL;
    elem = 0;
    elemTime = 0;
    time = 0;
#if !USE_ANIMGR
    loader = new AIRLoader();
#endif
}

AnimMap::~AnimMap() {
}

void AnimMap::open(const char *filename) {
#if USE_ANIMGR
    loader = g_AnimManager->open(filename);
#else
    loader->open(filename);
#endif
    set(0);
}

void AnimMap::set(int n) {
    action = loader->get(n);
    if (action) {
        elem = 0;
        elemTime = 0;
        time = 0;
    }
}

// atualiza a animação atual
void AnimMap::update() {
    if (!action)
        return;

    frame = action->vFrames[elem];
    if (elemTime >= frame->time && (frame->time > 0)) {
        elemTime = 0;
        elem++;
        if ((unsigned)elem >= action->vFrames.size())
            elem = 0;
    } else
        elemTime++;
    time++;
}

ColObjectGroup *AnimMap::clsn1() {
    if (frame)
        return frame->clsn1;
    return NULL;
}
ColObjectGroup *AnimMap::clsn2() {
    if (frame)
        return frame->clsn2;
    return NULL;
}
ColObjectGroup *AnimMap::clsn1default() {
    if (frame)
        return frame->defClsn1;
    return NULL;
}
ColObjectGroup *AnimMap::clsn2default() {
    if (frame)
        return frame->defClsn2;
    return NULL;
}


int AnimMap::group() {
    if (!frame)
        return -1;
    return frame->group;
}

int AnimMap::index() {
    if (!frame)
        return -1;
    return frame->index;
}

int AnimMap::x() {
    if (!frame)
        return -1;
    return frame->xaxis;
}
int AnimMap::y() {
    if (!frame)
        return -1;
    return frame->yaxis;
}
bool AnimMap::fliph() {
    if (!frame)
        return false;
    return frame->hflip;
}
bool AnimMap::flipv() {
    if (!frame)
        return false;
    return frame->vflip;
}
int AnimMap::number() {
    if (!action)
        return -1;
    return action->number;
}

