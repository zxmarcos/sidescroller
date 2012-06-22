#ifndef COLOBJECTGROUP_H
#define COLOBJECTGROUP_H

#include <list>
#include "colobject.h"
#include "levelmap.h"

class ColObjectGroup
{
    public:
        ColObjectGroup();
        virtual ~ColObjectGroup();

        void AddObj(ColObject *obj);

        // checa colisão entre um obj e todos os objetos do grupo

        int operator==(ColObject& obj);

        void SetName(const char *str);
        const char *GetName() const;
        void Draw(int x, int y);
        void DrawRef(int x, int y, int disx, int disy);

        std::list<ColObject*> listObjs;
        char *name;

    protected:
    private:


};

#endif // COLOBJECTGROUP_H
