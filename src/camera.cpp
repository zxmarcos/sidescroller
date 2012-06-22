#include <cmath>
#include <list>
#include <boost/foreach.hpp>

#include "camera.h"
#include "videosystem.h"
#include "colobject.h"
#include "colobjectgroup.h"
#include "entity.h"
#include "system.h"

Camera::Camera() {
    xPos = 0;
    yPos = 0;
    isTracking = false;
}

Camera::~Camera() {
    //dtor
}

void Camera::setMap(LevelMap *map) {
    pMap = map;
}


void Camera::TrackMotion(int x1, int y1, int x2, int y2, float xvel, float yvel) {
    xTrackPos1 = std::min(x1, x2);
    xTrackPos2 = std::max(x1, x2);
    yTrackPos1 = std::min(y1, y2);
    yTrackPos2 = std::max(y1, y2);
    xVel = xvel;
    yVel = yvel;
    xPos = xTrackPos1;
    yPos = yTrackPos1;
    isTracking = true;
    trackCounter = 0;
}


void Camera::setMainEntity(Entity *ent) {
    cent = ent;
}

void Camera::update() {
    if (isTracking) {
        if (xPos < xTrackPos2)
            xPos += xVel;
        if (yPos < yTrackPos2)
            yPos += yVel;
        ++trackCounter;
    }

    if (cent) {
        int player_x = cent->x();
        int player_y = cent->y();

        const int max_total_x = pMap->w() * pMap->tilew();
		const int max_total_y = pMap->h() * pMap->tileh();

		const int scr_half_x = g_Video->GetW() / 2;
		const int scr_half_y = g_Video->GetH() / 2;

		const int max_x = max_total_x - scr_half_x * 2;
		const int max_y = max_total_y - scr_half_y * 2;

		xPos = player_x - scr_half_x;
		if (xPos < 0)
			xPos = 0;
		else
		if (xPos > max_x)
			xPos = max_x;

        // y pos
        yPos = player_y - scr_half_y;
		if (yPos < 0)
			yPos = 0;
		else
		if (yPos > max_y)
			yPos = max_y;

		pMap->render(xPos, yPos);

		pMap->blit();
		//g_Video->drawGrid();



		int dx = scr_half_x;
		int dy = scr_half_y;

		if (player_x < scr_half_x)
			dx = player_x;
		else
		if (player_x > (max_x + scr_half_x))
			dx = scr_half_x + (player_x - (max_x + scr_half_x));
		else
			dx = scr_half_x;


        if (player_y < scr_half_y)
			dy = player_y;
		else
		if (player_y > (max_y + scr_half_y))
			dy = scr_half_y + (player_y - (max_y + scr_half_y));
		else
			dy = scr_half_y;


		cent->drawAt(dx, dy);
		g_Video->drawAxis(dx, dy);

		drawEntitys();

    }
}

void Camera::drawEntitys() {
    // desenha os outros objetos
    //foreach ( Entity *ent, entities ) {
    for (list<Entity*>::iterator i=entities.begin(); i != entities.end(); i++) {
            Entity *ent = *i;
        // verifica se está dentro da posição x da camera dentro da tela
        if (ent->x() >= xPos && ent->x() <= xPos + g_Video->GetW())
            // verifica se está dentro da posição y da camera dentro da tela
            if (ent->y() >= yPos && ent->y() <= yPos + g_Video->GetH())
                ent->drawAt(ent->x() - xPos, ent->y() - yPos);
    }
}

void Camera::addEntity(Entity *ent) {
    if (ent)
        entities.push_back(ent);
}

int Camera::x() const { return (int) xPos; }
int Camera::y() const { return (int) yPos; }
