#ifndef CAMERA_H
#define CAMERA_H

#include <list>
#include "levelmap.h"
#include "entity.h"

class Camera;

extern Camera *g_Camera;
class Camera
{
    public:
        Camera();
        virtual ~Camera();

        void setMainEntity(Entity *ent);
        void setMap(LevelMap *map);

        void addEntity(Entity *ent);

        void TrackMotion(int x1, int y1, int x2, int y2, float xvel, float yvel);
        void update();
        int x() const;
        int y() const;
    private:

        void drawEntitys();
        Entity *cent;
        std::list<Entity*> entities;
        bool isTracking;
        float xPos;
        float yPos;
        int xTrackPos1, xTrackPos2;
        int yTrackPos1, yTrackPos2;
        float xVel;
        float yVel;
        int trackCounter;
        LevelMap *pMap;
};

#endif // CAMERA_H
