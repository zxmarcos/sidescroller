#ifndef COLOBJECT_H
#define COLOBJECT_H


class ColObject
{
    public:
        ColObject(int x_ = 0, int y_ = 0, int w_ = 0, int h_ = 0);

        virtual ~ColObject();
        void SetColor(int r_, int b_, int g_);
        void SetRect(int x_, int y_, int w_, int h_);

        // checa colisão contra outro objeto
        int operator==(ColObject& right);
        void Draw(int x, int y);
        void DrawRef(int x, int y, int disx, int disy);
    private:
        int x;
        int y;
        int w;
        int h;
        int r, g, b;

    public:
        void IncX(int delta) {
            x += delta;
        }
        void IncY(int delta) {
            y += delta;
        }

        inline int GetX() const { return x; }
        inline int GetY() const { return y; }
        inline int GetW() const { return w; }
        inline int GetH() const { return h; }

        ColObject(ColObject *p) { x = p->x; y = p->y; w = p->w; h = p->h; }

};


#endif // COLOBJECT_H
