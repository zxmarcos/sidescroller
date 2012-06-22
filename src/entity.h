#ifndef ENTITY_H
#define ENTITY_H

#include "xyobject.h"
#include "spritemap.h"
#include "animmap.h"
#include <lua.hpp>

#define LUA_EXPORT(n)   static int le_##n(lua_State *L)
#define LUA_IMPLEMENT(c,n)   int c :: le_##n(lua_State *L)
#define LUA_OBJECT(c)                       \
    c *_this;                               \
    lua_getglobal(L, c::lua_global_name);   \
    _this = (c *) lua_touserdata(L, -1);
#define LUA_REGISTER(l, c, m)   lua_register(l, #m, c::le_##m)

class Entity : public XYObject
{
    static const char *lua_global_name;
    public:
        /** Default constructor */
        Entity();
        /** Default destructor */
        ~Entity();
        int open(const char *filename);
        void close();

        void update();
        void drawAt(int dx, int dy);
        void drawBox(ColObjectGroup *col);

        virtual int width() const;
		virtual int height() const;
		const char *myname();

		bool isFacing() const;
		void setFaced(bool v);
		void setGrounded(bool v);
		void setHeaded(bool v);
		void setCollided(bool v);

		bool checkCol(Entity *right);

    protected:
        const char *name;
        int xdraw;
        int ydraw;
        bool facing;
        bool isloaded;
        // colisões
        bool faced;
        bool grounded;
        bool headed;
        bool collided;
        AnimMap *anim;
        SpriteMap *sprite;
        lua_State *L;

        bool checkBoxes(ColObjectGroup *a, ColObjectGroup *b, int bx, int by);
        // funções exportadas para o script
        LUA_EXPORT(ChangeAnim);
        LUA_EXPORT(GetAnim);
        LUA_EXPORT(Input);
        LUA_EXPORT(SetX);
        LUA_EXPORT(SetY);
        LUA_EXPORT(AddX);
        LUA_EXPORT(AddY);
        LUA_EXPORT(GetX);
        LUA_EXPORT(GetY);
        LUA_EXPORT(SetVelX);
        LUA_EXPORT(SetVelY);
        LUA_EXPORT(AddVelX);
        LUA_EXPORT(AddVelY);
        LUA_EXPORT(GetVelX);
        LUA_EXPORT(GetVelY);
        LUA_EXPORT(IsGrounded);
        LUA_EXPORT(IsFaced);
        LUA_EXPORT(IsHeaded);
        LUA_EXPORT(SetFacing);
        LUA_EXPORT(IsFacing);
        LUA_EXPORT(IsHited);
};


#endif // ENTITY_H
