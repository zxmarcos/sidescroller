/* Implementação do sistema de colisão
 *
 */
#include <SDL/SDL.h>
#include "colobject.h"
#include "videosystem.h"
#include "system.h"

ColObject::ColObject(int x_, int y_, int w_, int h_)
{
    SetRect(x_, y_, w_, h_);
}

ColObject::~ColObject()
{
    //dtor
}

void ColObject::Draw(int dx, int dy)
{
    SDL_Rect rect;

    int sx, fw;
    int sy, fh;
    if (x <= dx)
    {
        sx = 0;
        fw = x + w - dx;
        if (fw < 0)
            fw = 0;
    }
    else
    {
        sx = x - dx;
        fw = w;
    }


    if (y <= dy)
    {
        sy = 0;
        fh = y + h - dy;
        if (fh < 0)
            fh = 0;
    }
    else
    {
        sy = y - dy;
        fh = h;
    }


    rect.x = sx;
    rect.y = sy;
    rect.w = fw;
    rect.h = fh;

    g_Video->DrawRect(&rect, g_Video->Color(r, g, b));
}

void ColObject::DrawRef(int dx, int dy, int disx, int disy)
{
    int xb = x;
    int yb = y;
    x = disx + x;
    y = disx + y;
    Draw(dx, dy);
    x = xb;
    y = yb;
}

void ColObject::SetRect(int x_, int y_, int w_, int h_)
{
    x = x_;
    y = y_;
    w = w_;
    h = h_;
}


int ColObject::operator==(ColObject& right)
{
    ColObject *a, *b;
#if 0
    Log("[%d,%d,%d,%d] - [%d,%d,%d,%d]\n",
        x, y, w, h,
        right.x, right.y, right.w, right.h
        );
#endif

    if (x >= right.x)
    {
        a = &right;
        b = this;
    }
    else
    {
        a = this;
        b = &right;
    }

    // colisão dentro do eixo X
    if ((b->x >= a->x) && (b->x <= a->x + a->w))
    {
        // A está abaixo de B
        if (a->y >= b->y)
        {
            if (a->y <= (b->y + b->h))
                return 1;
        }
        else
        {
            if (b->y <= (a->y + a->h))
                return 1;
        }
    }
    return 0;
}

void ColObject::SetColor(int r_, int b_, int g_)
{
    r = r_;
    g = g_;
    b = b_;
}
