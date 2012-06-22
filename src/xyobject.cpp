/**
 *
 */
#include "xyobject.h"

XYObject::XYObject() {
	xpos = 0;
	ypos = 0;
	xsize = 0;
	ysize = 0;
}

XYObject::~XYObject() {
}

float XYObject::x() const { return xpos; }
float XYObject::y() const { return ypos; }
float XYObject::lastx() const { return xlast; }
float XYObject::lasty() const { return ylast; }
int XYObject::width() const { return xsize; }
int XYObject::height() const { return ysize; }
float XYObject::velx() const { return xvel; }
float XYObject::vely() const { return yvel; }
void XYObject::setx(float x) { xlast = xpos; xpos = x; }
void XYObject::sety(float y) { ylast = ypos; ypos = y; }
void XYObject::setvelx(float x) { xvellast = xvel; xvel = x; }
void XYObject::setvely(float y) { yvellast = yvel; yvel = y; }

